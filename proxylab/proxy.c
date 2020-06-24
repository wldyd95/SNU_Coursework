/* System programming Lab Assignment #5: Writing a Caching Web Proxy (Proxy Lab)
 * 2020 Spring, Seoul National University
 * 2013-10958 jiyong yu
 * In this lab, we implemented proxy server with following step
 * Part1: Implementing a sequential web proxy
 * Part2: Dealing with multiple concurrent requests
 * Part3: Caching web objects
 * We implemented web proxy using LRU cache eviction policy
 */

#include <stdio.h>
#include "csapp.h"
#include <time.h>

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000  
#define MAX_OBJECT_SIZE 102400
#define DEFAULT_PORT 80
#define CACHE_LENGTH 10

/* You won't lose style points for including this long line in your code */
static const char *request_hdr_format = "GET %s HTTP/1.0\r\n";
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *host_hdr_format = "Host: %s\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";

typedef struct {
    char uri[MAXLINE];
    char obj[MAX_OBJECT_SIZE];

    time_t age; /* Least recently used block has least age */

    int readcnt; /* Initially 0 */
    sem_t mutex, r, w; /* Initially 1 */

}CachedItem;

CachedItem cacheList[CACHE_LENGTH]; /* Maximum cache length is 10 since maximum cache size is 1MB and maximum object size is 100kB */

/* Utility function for proxy server */
void *thread(void *vargp);
void doit(int fd); 
void parse_uri(char *uri, char *host, char *path, int *port);

/* Cache function */
void cache_init();
int cache_find(char *uri);
int cache_evict();
void update_LRU(int i);
void cache_insert(char *uri, char *obj);

/* Lock & Unlock function for concurrent threading */
void reader_lock(int i);
void reader_unlock(int i);
void writer_lock(int i);
void writer_unlock(int i);

int main(int argc, char **argv)
{   
    cache_init(); /* Initialize cache list */
    int listenfd, connfd; /* Proxy server's listenfd(connfd): client file descriptor */
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid; 

    /* Invalid argument form check */
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    if(listenfd < 0){
        printf("Failed to open listener\n");
        return 0;
    }

    /* Concurrent proxy using thread */
    while(1){
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA*) &clientaddr, (void *)&clientlen); /* Make connfd for each thread */
        if(connfd < 0){
            printf("Failed to make connection\n");
            return 0;
        }
        Pthread_create(&tid, NULL, thread, &connfd);
    }

    return 0;
}

/* Thread routine */
void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    doit(connfd); 
    Close(connfd);
    return NULL;
} 

/* Handle client request - variation from tiny.c doit function */
void doit(int connfd){
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    
    /* uri is composed of host, path, port. We will parse uri to get this information */
    char host[MAXLINE], path[MAXLINE];
    int port;

    int serverfd; /* server file descriptor */

    rio_t client_rio, server_rio; /* rio for client and server */
    Rio_readinitb(&client_rio, connfd);
    Rio_readlineb(&client_rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version); /* For example, method is GET, uri is www.cmu,edu, version is HTTP/1.0 */

    if(strcasecmp(method, "GET")){
        printf("Proxy does not implement this method");
        return;
    }

    /* Check if given uri is within cache. If it is, we don't have to connect to the server directly */
    int found_index;
    if((found_index = cache_find(uri)) >= 0){
         reader_lock(found_index);

         Rio_writen(connfd, cacheList[found_index].obj, strlen(cacheList[found_index].obj));

         reader_unlock(found_index);
         update_LRU(found_index);
         return;
    }

    /* If uri is not within cache, go to server directly */
    parse_uri(uri, host, path, &port); /* Parse uri and get host, path, port */

    char request_hdr[MAXLINE], host_hdr[MAXLINE];
    char http_header[MAXLINE]; /* Entire http header which is sent to the server */

    /* Send http header to the server*/
    sprintf(request_hdr, request_hdr_format, path);
    sprintf(host_hdr, host_hdr_format, host);
    sprintf(http_header,"%s%s%s%s%s%s", request_hdr, user_agent_hdr, host_hdr, connection_hdr, proxy_connection_hdr, "\r\n");

    /* Connet to server */
    char port_str[8 * sizeof(int)];
    sprintf(port_str, "%d", port); /* convert to string */
    serverfd = Open_clientfd(host, port_str);
    if(serverfd < 0){
        printf("Failed to make connedtion\n");
        return;
    }

    /* Write http header to the server */
    Rio_readinitb(&server_rio, serverfd);
    Rio_writen(serverfd, http_header, strlen(http_header));

    /* Receive response from server and direct it to the client */
    char object_cache[MAX_OBJECT_SIZE];
    int object_size = 0;
    int read_size;

    while((read_size = Rio_readlineb(&server_rio, buf, MAXLINE)) ){ /* read line by line */
        object_size += read_size;
        if(object_size <= MAX_OBJECT_SIZE) strcat(object_cache, buf);
        Rio_writen(connfd, buf, read_size);
    }

    Close(serverfd);

    /* Store uri and object to the cache */
    if(object_size < MAX_OBJECT_SIZE) cache_insert(uri, object_cache);

    return;
}

void parse_uri(char *uri, char *host, char *path, int *port){

    /* There can be two port number cases */
    /* First, uri is like http://www.cmu.edu/hub/index.html. In this case, port number is 80 (default value) */
    /* Second, uri is like http://www.cmu.edu:8080/hub/index.html. In this case, port number is 8080 */

    char uri_temp[MAXLINE];
    strcpy(uri_temp, uri);

    char *divide1 = (strstr(uri_temp, "//") + 2); /* divide string by // */
    char *divide2 = strstr(divide1, ":"); /* divide string by : */

    if(divide2 == NULL){ /* Use default port number (80) */
        divide2 = strstr(divide1, "/");
        *divide2 = '\0'; /* To divide host and path */
        sscanf(divide1, "%s", host); 

        *divide2 = '/';
        sscanf(divide2, "%s", path);

        *port = DEFAULT_PORT;
    }

    else{ /* Use specific port number */
        *divide2 = '\0'; /* To divide host and (port + path) */
        sscanf(divide1, "%s", host);
        sscanf(divide2 + 1, "%d%s", port, path);
    }

    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Cache functions implemented */

void cache_init(){

    int i;
    for(i = 0; i < CACHE_LENGTH; i++){
        Sem_init(&cacheList[i].r,0,1);
        Sem_init(&cacheList[i].w,0,1);
        Sem_init(&cacheList[i].mutex,0,1); /* Initialize all mutex as 1 */

        cacheList[i].readcnt = 0; /* Initialize readcnt as 0 */
        cacheList[i].age = 0;
    }
}

/* Check given uri is within cache */
int cache_find(char *uri){
    int i;

    for(i = 0; i< CACHE_LENGTH; i++){
        reader_lock(i);

        if(strcmp(cacheList[i].uri, uri) == 0) {
            reader_unlock(i);
            break;
        }

        reader_unlock(i);
    }
    if(i < CACHE_LENGTH) return i; 
    else return -1; /* Failed to find uri */
}

/* Cache eviction using LRU policy */
int cache_evict(){
    int min = cacheList[0].age;
    int evict_index = 0;
    int i;

    for(i = 0; i < CACHE_LENGTH; i++){ 
        reader_lock(i);

        if(cacheList[i].age < min){ /* Find minimum LRU ( Least recently used! ) */
            min = cacheList[i].age;
            evict_index = i;
        }

        reader_unlock(i);
    }

    return evict_index;
}

/* Update LRU value using global variable age */
void update_LRU(int index){
    writer_lock(index);

    cacheList[index].age = time(NULL); /* updage age using current time */

    writer_unlock(index);
}

/* Insert new cache block */
void cache_insert(char *uri,char *obj){


    int i = cache_evict();

    writer_lock(i);

    strcpy(cacheList[i].uri, uri);
    strcpy(cacheList[i].obj, obj);

    writer_unlock(i);

    update_LRU(i);
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/* Reader, writer lock function  */
/* We used 3rd Readers-Writers problem solution from class material */
void reader_lock(int i){
    P(&cacheList[i].r);
    P(&cacheList[i].mutex);
    cacheList[i].readcnt++;
    if(cacheList[i].readcnt==1) {
        P(&cacheList[i].w);
    }
    V(&cacheList[i].mutex);
    V(&cacheList[i].r);
}

void reader_unlock(int i){
    P(&cacheList[i].mutex);
    cacheList[i].readcnt--;
    if(cacheList[i].readcnt==0) {
        V(&cacheList[i].w);
    }
    V(&cacheList[i].mutex);
}

void writer_lock(int i){
    P(&cacheList[i].r);
    P(&cacheList[i].w);
}

void writer_unlock(int i){
    V(&cacheList[i].w);
    V(&cacheList[i].r);
}
