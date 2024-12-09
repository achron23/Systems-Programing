#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/fcntl.h>

#define BUFFER_SIZE 1024
pthread_mutex_t running_jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_running_jobs = PTHREAD_COND_INITIALIZER;


volatile int server_running = 1;
job_queue jobQueue;
int jobID = 0;
int bufferSize;
int runningJobs = 0;
int threadPoolSize;
int concurrency_level = 1;

pthread_t *threads;

//split a command into arguments
void parse_args(char *buf, char **args) {
    while (*buf != '\0') {
        while ((*buf == ' ') || (*buf == '\t') || (*buf == '\n'))
            *buf++ = '\0';  // Null terminate previous arg and skip whitespace

        *args++ = buf;  // Start a new argument

        while ((*buf != '\0') && (*buf != ' ') && (*buf != '\t') && (*buf != '\n'))
            buf++;  // Find the end of the argument

        if (*buf != '\0'){
            *buf++ = '\0';
        }    

    
    }
    *args = NULL;  // Null terminate the args array

}

void *worker_thread(void *arg){
    job_entry job;
    while(server_running){
        if (dequeue(&jobQueue,&job) == -1)
            break;
        pthread_mutex_lock(&running_jobs_mutex);
        while(runningJobs >= concurrency_level){
            pthread_cond_wait(&cond_running_jobs,&running_jobs_mutex);

        }
        runningJobs++;
        pthread_mutex_unlock(&running_jobs_mutex);

        pid_t pid = fork();
        if (pid == 0){
            //child proccess
            char output_filename[BUFFER_SIZE];
            snprintf(output_filename,BUFFER_SIZE,"%d.output",getpid());
            int output_fd = open(output_filename,O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (output_fd < 0){
                perror("Failed to open output file");
                exit(EXIT_FAILURE);

            }
            //redirect stdout to the output file
            if (dup2(output_fd,STDOUT_FILENO) < 0 ){
                perror("Redirection error");
                exit(EXIT_FAILURE);
            }
            close(output_fd);

            //Execute the job command
            char* args[64];
            parse_args(job.job,args);
            execvp(args[0],args);
            perror("execvp failed");
            exit(EXIT_FAILURE);

        }else if (pid < 0){
            perror("Fork failed");
        }else {
           // Parent process
            int status;
            waitpid(pid, &status, 0);

            // Read the output file and send its content back to the client
            char output_filename[BUFFER_SIZE];
            snprintf(output_filename, BUFFER_SIZE, "%d.output", pid);
            FILE *output_file = fopen(output_filename, "r");
            if (output_file) {
                char file_buffer[BUFFER_SIZE];
                size_t n;
                while ((n = fread(file_buffer, 1, sizeof(file_buffer), output_file)) > 0) {
                    send(job.client_sock, file_buffer, n, 0);
                }
                fclose(output_file);
               
                remove(output_filename); // Clean up the output file
            } else {
                perror("Failed to open output file for reading");
            }

            
      
            pthread_mutex_lock(&running_jobs_mutex);
            runningJobs--;
            pthread_cond_signal(&cond_running_jobs);
            pthread_mutex_unlock(&running_jobs_mutex);

            close(job.client_sock);

            
            
        }

           
    }
    return NULL;

}

int issueJob(const char* job, char *job_id_str,int client_sock){
    
    sprintf(job_id_str,"job_%d",++jobID);
    return enqueue(&jobQueue,job_id_str,job,client_sock);
    


}



void *controller_thread(void *arg){
    int client_sock = *((int *)arg);
    char response[BUFFER_SIZE];
    char buffer[bufferSize];
    int bytes_read;
    while ((bytes_read = recv(client_sock, buffer , bufferSize, 0)) > 0) {
        buffer[bytes_read] = '\0';
        //clear the response buffer
        response[0] = '\0';
        char *command = strtok(buffer," ");
        if (strcmp(command,"issueJob") == 0){
            char *job = strtok(NULL,"");
            if (job) {
                char job_id_str[20];
                if ((issueJob(job,job_id_str,client_sock) == 0)){
                    snprintf(response,BUFFER_SIZE,"JOB %s , %s SUBMITTED",job_id_str,job);
                    send(client_sock,response,strlen(response), 0);
                }    
                return NULL; // job will be handled by the worker thread
            }

        }else if(strcmp(command,"setConcurrency") == 0){
            char *param = strtok(NULL," ");
            if (param){
                pthread_mutex_lock(&running_jobs_mutex);
                concurrency_level = atoi(param);
                pthread_cond_broadcast(&cond_running_jobs); // wake up worker threads
                pthread_mutex_unlock(&running_jobs_mutex);
                snprintf(response,BUFFER_SIZE,"CONCURRENCY SET AT %d",concurrency_level);
                send(client_sock,response,strlen(response), 0);
            }   
        }else if (strcmp(command,"stop") == 0) {
            char *jobID = strtok(NULL," ");
            if (jobID){
                if (remove_job_by_id(&jobQueue,jobID) > 0){
                    snprintf(response,BUFFER_SIZE,"JOB %s REMOVED",jobID);
                }else {
                    snprintf(response,BUFFER_SIZE,"JOB %s NOT FOUND",jobID);
                }
                send(client_sock,response,strlen(response), 0);
                
            }

        }else if(strcmp(command,"poll") == 0){
            pthread_mutex_lock(&jobQueue.mutex);
            if (jobQueue.count > 0){
                for(int i=0; i<jobQueue.count; i++){
                    int idx = (jobQueue.front + i) % jobQueue.threadPoolsize;
                    char job_info[BUFFER_SIZE];
                    snprintf(job_info, BUFFER_SIZE, "%s , %s",jobQueue.jobs[idx].jobID,jobQueue.jobs[idx].job);
                    strncat(response,job_info,bufferSize - strlen(response) - 1);
                }    
            }else
                snprintf(response,BUFFER_SIZE,"NO JOBS TO POLL");
            pthread_mutex_unlock(&jobQueue.mutex);
            send(client_sock,response,strlen(response),0);
        }else if(strcmp(command,"exit") == 0){

            pthread_mutex_lock(&jobQueue.mutex);
            server_running = 0;
            jobQueue.flag = 0;
            //wake up all worker threads
            pthread_cond_broadcast(&jobQueue.cond_nonempty);
            pthread_mutex_unlock(&jobQueue.mutex);
            //wake up all running threads to finish running jobs
            pthread_cond_broadcast(&cond_running_jobs);


            snprintf(response,BUFFER_SIZE,"SERVER IS SHUTTING DOWN");
            send(client_sock,response,strlen(response),0);

            

            job_entry entry;
            while(dequeue(&jobQueue,&entry) > 0){
                snprintf(response,BUFFER_SIZE,"SERVER TERMINATED BEFORE EXECUTION");
                printf("%s\n",response);
            }
            

            for (int i = 0; i<threadPoolSize; i++){
                pthread_join(threads[i],NULL);
                printf("Joined thread %d\n",i);
            }
          
           



            close(client_sock);
            return NULL;
       
        }
        close(client_sock);
    }
    close (client_sock);
    return NULL;



}


int main(int argc,char *argv[]){
    if (argc != 4){
        fprintf(stderr,"Usage : ./jobExecutorServer <portNum> <bufferSize> <threadPoolSize>\n");
        exit(1);
    }

    int portNum = atoi (argv[1]);
    bufferSize = atoi (argv[2]);
    int threadPoolSize = atoi (argv[3]);

    threads = malloc(threadPoolSize*sizeof(pthread_t));

    int server_sock,client_sock;
    struct sockaddr_in server_addr , client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    //initialize queue
    init_queue(&jobQueue,threadPoolSize);

    //create worker threads
   
    for(int i = 0; i < threadPoolSize;  i++){
        pthread_create(&threads[i],NULL,worker_thread,NULL);
    }


   
    //Create socket
    if ((server_sock = socket(AF_INET,SOCK_STREAM,0)) == 0){
        perror("Socket create error");
        exit(EXIT_FAILURE);

    }

    //Setup address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //Bind the socket to the port
    if (bind(server_sock,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);


    }


    if (listen(server_sock,threadPoolSize) < 0 ) { 
        perror("Listening failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n" , portNum);

    while(server_running){
        client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_addr_len);
        if (client_sock < 0){
            perror("Accept failed");
            continue;
        }
        pthread_t client_thread;
        pthread_create(&client_thread,NULL,controller_thread,&client_sock);
        pthread_detach(client_thread); 
    }


    // for (int i = 0; i<threadPoolSize; i++){
    //         pthread_join(threads[i],NULL);
    // }



    //cleanup
    pthread_mutex_destroy(&running_jobs_mutex);
    pthread_cond_destroy(&cond_running_jobs);

    destroy_queue(&jobQueue);
    free(threads);
    
   
   
   
    close(server_sock);
    printf("Server shutdown complete\n");






}
