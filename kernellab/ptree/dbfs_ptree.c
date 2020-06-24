#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define MAX_LENGTH 2000 /* Maximum length of ptree message */

MODULE_LICENSE("GPL");

static struct dentry *dir, *inputdir, *ptreedir; /* File descriptor */
static struct task_struct *curr; /* used for process descriptor */

/* Output string for printing ptree message */
char ptree_msg[MAX_LENGTH];
ssize_t ptree_msg_length = 0;

static ssize_t write_pid_to_input(struct file *fp, 
                                const char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
        pid_t input_pid;
        char buffer[MAX_LENGTH];
        ssize_t buffer_length = 0; 

        memset(ptree_msg, 0, MAX_LENGTH); /* Initialize output string */
        memset(buffer, 0, MAX_LENGTH); /* Initialize buffer string */
        sscanf(user_buffer, "%u", &input_pid); /* Read input_pid from user buffer  */

        /* Find task_struct using input_pid. Hint: pid_task */
        if(!(curr = pid_task(find_get_pid(input_pid), PIDTYPE_PID))) {
                printk("Invalid process\n");
                return -1;
        }
        
        /* Tracing process tree from input_pid to init(1) process */
        while((curr -> pid) != 0){
                memset(buffer, 0, MAX_LENGTH);
                buffer_length = snprintf(buffer, MAX_LENGTH, "%s (%d)\n", curr -> comm, curr -> pid);
                if(buffer_length < 0) {
                        printk("Insufficient buffer length\n");
                        return -1;
                }

                snprintf(buffer + buffer_length, MAX_LENGTH - buffer_length, ptree_msg); /* append last ptree message */
                ptree_msg_length += buffer_length;

                strcpy(ptree_msg, buffer); /* update ptree message */

                curr = curr -> parent; /* move up toward root */
        }

        return length;
}

static ssize_t read_output(struct file *fp, 
                                char __user *user_buffer, 
                                size_t length, 
                                loff_t *position)
{
	ssize_t msg_length = simple_read_from_buffer(user_buffer, length, position, ptree_msg, ptree_msg_length);
        return msg_length;
}


static const struct file_operations dbfs_fops = {
        .write = write_pid_to_input,
	.read = read_output,
};


static int __init dbfs_module_init(void)
{
        /* Implement init module code */
        if((dir = debugfs_create_dir("ptree", NULL)) == 0){
                printk("Ptree directory initialization fail\n");
                return -1;
        }
        
        if((inputdir = debugfs_create_file("input", S_IRUSR | S_IWUSR, dir, NULL, &dbfs_fops)) == 0){
                printk("Input file initialization fail\n");
                return -1;
        }

        if((ptreedir = debugfs_create_file("ptree", S_IRUSR | S_IWUSR, dir, NULL, &dbfs_fops)) == 0){
                printk("Ptree file initialization fail\n");
                return -1;
        }


#if 0
        dir = debugfs_create_dir("ptree", NULL);
        
        if (!dir) {
                printk("Cannot create ptree dir\n");
                return -1;
        }

        inputdir = debugfs_create_file("input", , , , );
        ptreedir = debugfs_create_("ptree", , , ); /* Find suitable debugfs API */
#endif
	
	printk("dbfs_ptree module initialize done\n");

        return 0;
}

static void __exit dbfs_module_exit(void)
{
        /* Implement exit module code */
	debugfs_remove(dir);
	printk("dbfs_ptree module exit\n");
}

module_init(dbfs_module_init);
module_exit(dbfs_module_exit);
