#include "jobCommander.h"





void issueJob(queue* running, queue *queued, char *job, int jobID ) {
    pid_t pid;
    char response_str[100];
    char *args[64];
    char todo[strlen(job)];

    //todo is a temp to store the job 
    //and be parsed into arguments
    strcpy(todo , job);
    
    parse_args(todo,args);

    signal(SIGCHLD,handler);
    //if there is space to running queue
    if (running->concurrency > running->size) {
        
       
            pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            }
            if (pid == 0) {
                //child proccess
                //execute given job
                printf("%s\n",*args);
                execvp(*args, args);
                perror("execvp failed");
                exit(1);
            } else {
                //parent proccess
                //insert to running queue

                sprintf(response_str, "job_%d\t%s\t%s", jobID, job, "RUNNING");
                response(response_str);
                queue_insert(running, job, jobID, pid);
                
            }
        }

    else {
        //insert to queued queue
        sprintf(response_str, "job_%d\t%s\t%s", jobID, job, "QUEUED");
        response(response_str);
        queue_insert(queued, job, jobID, -1);
    }
}

//handlres for terminated jobbs
void handler(int sig){
    pid_t pid;
    int status;

    //find terminated children 
    while ((pid = waitpid((-1),&status, WNOHANG)) > 0){
        qnode* to_free = queue_remove(running);
        free(to_free);
    }    
    //if running jobs have space and there are queued jobs
    while(running->size < running->concurrency && queued->size > 0 ){
        //remove from queued

        qnode* jobNode = queue_remove(queued);
        //insert to running
        issueJob(running,queued,jobNode->job,jobNode->jobID);
        free(jobNode);
        
    }    

     
}

//This function is called when concurrency is changed and works 
//similar to the handler
void update_running(){
   
    pid_t pid;
    int status;

    while(running->size < running->concurrency && queued->size > 0 ){
        qnode* jobNode = queue_remove(queued);
        
        issueJob(running,queued,jobNode->job,jobNode->jobID);
        free(jobNode);
        
    }    

}


//split a command into arguments
void parse_args(char *buf, char **args) {
    while (*buf != '\0') {
        while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n'))
            *buf++ = '\0';  // Null terminate previous arg and skip whitespace

        *args++ = buf;  // Start a new argument

        while ((*buf != '\0') && (*buf != ' ') && (*buf != '\t') && (*buf != '\n'))
            buf++;  // Find the end of the argument

        *args = NULL;  // Null terminate the args array
    
    }

}