/* 
 * tsh - A tiny shell program with job control
 * 
 * <2013-10958, user8>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/* additional function implemented individually */
static size_t sio_strlen(char s[]);
ssize_t sio_puts(char s[]);
void sio_error(char s[]);
void safe_printf(const char* format, ...);
int isnumber(char s[]);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{   
    // Below codes are referenced from textbook and additional funcions were implemented individually
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE]; /* Holds modified command line */
    int bg; /* Should the job run in bg or fg? */
    pid_t pid; /* Process id */
    int state = UNDEF; /* Job state */

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if(argv[0] == NULL){
        return; /* Ignore empty lines */
    }

    /* Determine state */
    switch (bg)
    {
    case 0:
        state = FG;
        break;
    
    case 1:
        state = BG;
        break;

    default:
        break;
    }

    sigset_t mask_all, mask_one, prev_one;

    if(!builtin_cmd(argv)){
        /* Blocking to prevent race problem*/
        if(sigfillset(&mask_all) < 0) exit(0);
        if(sigemptyset(&mask_one) < 0) exit(0);
        if(sigaddset(&mask_one, SIGCHLD) < 0) exit(0);

        /* Block SIGCHILD */
        if(sigprocmask(SIG_BLOCK, &mask_one, &prev_one) < 0) exit(0); 

        if((pid = fork()) < 0) exit(0); /* Failed to create child process */

        else if( pid == 0 ){ /* Child process */
            if(sigprocmask(SIG_SETMASK, &prev_one, NULL) < 0) exit(0); /* Unblock SIGCHILD */
            if(setpgid(0, 0) < 0) exit(0); /* Create new group whose ID is identical to the child process's ID*/
            if(execve(argv[0], argv, NULL) < 0){
                printf("%s: %s\n", argv[0], "Command not found"); /* Invalid command: exit child process */
                exit(0);
            };
        }

        if(sigprocmask(SIG_BLOCK, &mask_all, NULL) < 0) exit(0); /* Parent process */
        if(addjob(jobs, pid, state, cmdline) == 0) exit(0); /* Add the child to the job list*/
        if(sigprocmask(SIG_SETMASK, &prev_one, NULL) < 0) exit(0); /* Unblock SIGCHILD */

        /* Parent waits for foreground jobs to terminate */
        if(!bg){
            waitfg(pid);
        }

        /* Parent prints log message for background jobs */
        else{
            printf("[%d] (%d) ", jobs[maxjid(jobs) - 1].jid, jobs[maxjid(jobs) - 1].pid);
            printf("%s", jobs[maxjid(jobs) - 1].cmdline);
        }

        return;
    }

    else{ /* Built-in command: quit, jobs, bg, fg*/
        int quit_check;
        int jobs_check;
        int bg_check;
        int fg_check;

        quit_check = strcmp("quit", argv[0]);
        jobs_check = strcmp("jobs", argv[0]);
        bg_check = strcmp("bg", argv[0]);
        fg_check = strcmp("fg", argv[0]);

        if(quit_check == 0){
            exit(0);
        }

        else if(jobs_check == 0){
            listjobs(jobs);
        }

        else if(bg_check == 0 || fg_check == 0){
            do_bgfg(argv);
        }
    }

    return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 */
int builtin_cmd(char **argv) 
{   
    /* Built-in command: quit, jobs, bg, fg*/
    int quit_check;
    int jobs_check;
    int bg_check;
    int fg_check;

    quit_check = strcmp("quit", argv[0]);
    jobs_check = strcmp("jobs", argv[0]);
    bg_check = strcmp("bg", argv[0]);
    fg_check = strcmp("fg", argv[0]);

    /* Below check is needed: for example, quit XX */
    if( (quit_check == 0 || jobs_check == 0) && (argv[1] == NULL) ) return 1;

    /* Error handling for bg and fg will be done in do_bgfg */
    else if( (bg_check == 0 || fg_check == 0) ) return 1; 

    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    char *bgfg = NULL;
    bgfg = argv[0];

    pid_t pid;
    int jid;
    struct job_t *job = NULL;

    /* Error handling */
    if( strcmp(bgfg, "bg") == 0 && argv[1] == NULL ) {
        safe_printf("%s\n", "bg command requires PID or %jobid argument");
        return;
    }

    else if( strcmp(bgfg, "fg") == 0 && argv[1] == NULL ) {
        safe_printf("%s\n", "fg command requires PID or %jobid argument");
        return;
    }

    else if( strcmp(bgfg, "bg") == 0 && !isnumber(argv[1]) ){
        safe_printf("%s\n", "bg: argument must be a PID or %jobid");
        return;
    }

    else if( strcmp(bgfg, "fg") == 0 && !isnumber(argv[1]) ){
        safe_printf("%s\n", "fg: argument must be a PID or %jobid");
        return;
    }

    /* Get job information */
    if(argv[1][0] == '%'){ /* jid is given*/
        jid = atoi(&argv[1][1]); 
        job = getjobjid(jobs, jid);

        if(job == NULL) {
            safe_printf("%s%s\n", argv[1], ": No such job");
            return;
        }
        pid = (*job).pid;
    }

    else{ /* pid is given */
        pid = atoi(&argv[1][0]);
        jid = pid2jid(pid);
        job = getjobjid(job, jid);

        if(job == NULL) {
            safe_printf("%s%s%s\n", "(", argv[1], "): No such process");
            return;
        }
    }

    /* bg or fg*/
    if( strcmp(bgfg, "bg") == 0 && (*job).state == ST ) { /* bg: ST -> BG */
        safe_printf("[%d] (%d) ", jid, pid);
        safe_printf("%s", (*job).cmdline);
        
        (*job).state = BG;
        if(kill(-pid, SIGCONT) < 0) exit(0); /* Send SIGCONT signal */
    }

    else if( strcmp(bgfg, "fg") == 0 ){ /* fg: ST -> FG, BG -> FG */

        if( (*job).state == ST ){ 
            (*job).state = FG;
            if(kill(-pid, SIGCONT) < 0) exit(0); /* Send SIGCONT signal */
            waitfg(pid); /* wait until new foreground job terminates */
        }

        else if( (*job).state == BG ){
            (*job).state = FG;
            waitfg(pid); /* wait until new foregroud job terminates */
        }
    }
    
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{   
    struct job_t *job_wait;
    job_wait = getjobpid(jobs, pid); 

    if(job_wait == NULL) return;

    /* Use busy loop for rather than waitpid for waitfg*/
    while(1){
        if((*job_wait).state != FG) {  /* Deleted or stopped case*/
            break;
        }                        
        sleep(1);
    }

    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */


void sigchld_handler(int sig) 
{   
    int olderrrno = errno;
    sigset_t mask_all, prev_all;
    pid_t pid;
    struct job_t *job;
    int status; 
    
    if(sigfillset(&mask_all) < 0) exit(0); /* Block all signal */
    
    /* Reap a zombie child, set option WUNTRACED to deal with child stop case*/
    /* WNOHANG is also needed to catch unterminated child process (ex.SIGCONT comes in)*/
    pid = waitpid(-1, &status, WUNTRACED | WNOHANG); 
    
    /* Invalid pid: waitpid does not terminate */
    /* For example, if child receives SIGCONT, waitpid return is 0 */
    /* If waitpid return is not 0, child stops or terminates */
    if(pid <= 0){ 
        return;
    }

    job = getjobpid(jobs, pid);

    if(sigprocmask(SIG_BLOCK, &mask_all, &prev_all) < 0) exit(0);
    
    if(WIFEXITED(status)){ /* Normal termination, do not print message */
        if(deletejob(jobs, (*job).pid) == 0) exit(0); /* Delete the child from the job list */
        if(sigprocmask(SIG_SETMASK, &prev_all, NULL) < 0) exit(0); /* Restore block list */ 
    }

    else if(WIFSIGNALED(status)){ /* Termination due to signal, print message for termination */
        /* Print termination log */
        safe_printf("%s ", "Job");
        safe_printf("%s", "[");
        safe_printf("%d", (*job).jid );
        safe_printf("%s ", "]");
        safe_printf("%s", "(");
        safe_printf("%d", (*job).pid );
        safe_printf("%s", ") ");
        safe_printf("%s ", "terminated by signal");
        safe_printf("%d\n", SIGINT); /* Termination signal */

        if(deletejob(jobs, (*job).pid) == 0) exit(0); /* Delete the child from the job list */
        if(sigprocmask(SIG_SETMASK, &prev_all, NULL) < 0) exit(0); /* Restore block list */ 
    }

    else if(WIFSTOPPED(status)){ /* Stopped child process */

        if(WSTOPSIG(status) == SIGTSTP){ /* Stop due to TSTP signal, print stop message */
            /* Print stop log */
            safe_printf("%s ", "Job");
            safe_printf("%s", "[");
            safe_printf("%d", (*job).jid );
            safe_printf("%s ", "]");
            safe_printf("%s", "(");
            safe_printf("%d", (*job).pid );
            safe_printf("%s", ") ");
            safe_printf("%s ", "stopped by signal");
            safe_printf("%d\n", SIGTSTP); /* TSTP signal */
        }

        /* Stop the child */
        /* Do not print for other stop: for example, Sleep command*/
        (*job).state = ST;
        if(sigprocmask(SIG_SETMASK, &prev_all, NULL) < 0) exit(0); /* Restore block list */ 
    }

    /* Restore error number */
    errno = olderrrno;
    
    return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{   
    /* Find foreground jobs */
    pid_t pid;

    pid = fgpid(jobs);    
        
    if(kill(-pid, SIGINT) < 0) exit(0); /* Send SIGINT signal */

    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    /* Find foreground jobs */
    pid_t pid;

    if((pid = fgpid(jobs)) == 0) exit(0);

    if(kill(-pid, SIGTSTP) < 0) exit(0); /* Send SIGTSTP signal */
    
    return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("listjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}

/* sio_strlen - Return length of string */
static size_t sio_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0'){
        i++;
    }

    return i;
}

/* Put string */
ssize_t sio_puts(char s[]) 
{
    return write(STDOUT_FILENO, s, sio_strlen(s)); 
}

/* Put error message and exit */
void sio_error(char s[]) 
{
    sio_puts(s);
    exit(1);                                      
}

/* Async safe print */
void safe_printf(const char* format, ...)
{
    char buf[MAXARGS];
    va_list args;

    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    if(write(STDOUT_FILENO, buf, sio_strlen(buf)) == -1){ /* write is async signal safe */
        sio_error("Safe print fail");
        exit(1);
    }; 
    return; 
}

/* check pid or jid is number format: pid should be number and jid should be %(number) */
int isnumber(char s[]){
    int i = 0;
    int l = sio_strlen(s);

    if(s[0] == '%'){
        for(i = 1; i < l; i++){
            if(!isdigit(s[i])) return 0;
        }
    }

    else{
        for(i = 0; i < l; i++){
            if(!isdigit(s[i])) return 0;
        }
    }

    return 1;
}