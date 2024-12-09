#include "jobCommander.h"



queue* running = NULL;
queue* queued = NULL;

// void response_arr(char** args){
//     char buffer[MSGSIZE+1];



//     int fd2 = open(fifo2, O_WRONLY);
//     if (fd2 < 0) {
//         perror("Open response FIFO");
//         return;
//     }
//     int i=0;
//     while(args[i] != NULL){
//         strcpy(buffer,args[i]);
//         if (write(fd2, buffer, MSGSIZE+ 1) < 0) {
//             errormessage("server error in writing",2);
//         }
//         i++;

//     }

//     buffer[0] = '\0';
//     strcpy(buffer,"exit");
//     if (write(fd2, buffer, MSGSIZE+ 1) < 0) {
//         errormessage("server error in writing",2);
//     }
    
//     close(fd2);

// }


int stop(queue* qu,int id){
    qnode* temp = qu->node ;
    int flag = 0;

    while (temp != NULL){
        
        if (temp->jobID == id){
            qu = queue_remove_id(qu,id);
            flag = 1;
            
            break;
        }    
    
        temp = temp->next;    
    }    

    return flag;
    
}


void jobExecutorServer(){

    
    running = queue_create(1);
    queued = queue_create(1);

    
    char response_str[100];
    int jobID = 1 ;
    char *args = malloc(500);


    char buffer[MSGSIZE+1],*command,*job;
    //make fifo to send response to commander
    if (mkfifo (fifo2 , 0666) == -1){
		if (errno != EEXIST ) {
			errormessage("mkfifo failed", 6);
		}

	}
    for(;;){
        int fd = open(fifo,O_RDWR);

        if (fd < 0){
            errormessage("server error :fifo open for read",3);

        }   

        
        if (read(fd,buffer,MSGSIZE + 1) < 0){
            errormessage("server error : can't read ",5);
        }
        close(fd);

        //split the string in first space to get the first argument as command
        //the second argument will be the job to run
        command = strtok(buffer," ");

        
        

        if (strcmp(command,"issueJob") == 0) {
            job = strtok(NULL,"");
            

            if (job == NULL){
                printf("issuejob error missing arguments");
            
            }else{
                //function to run the job
                issueJob(running,queued,job,jobID);
                jobID++;
            }
        }

        if (strcmp(command,"setConcurrency") == 0){
            job = strtok(NULL," ");
            running->concurrency =  atoi(job);
            sprintf(response_str,"%s %d" , "Concurrency changed to :" ,running->concurrency);
            response(response_str);
            update_running();



        }

        if (strcmp(command,"stop") == 0) {
            printf("FROm stop\n");
            job = strtok(NULL," ");
            int id = atoi(job);
            

            if (stop(running,id) == 0 ){

                if(stop(queued,id) == 0 ){
                    sprintf(response_str,"%s","No job found to stop");
                    response(response_str);
                }else{
                    sprintf(response_str,"%s %d","Job removed:" ,id);
                    response(response_str);
                }

            }else{
                sprintf(response_str,"%s %d","Job terminated:" ,id);
                response(response_str);
            }
        }

        if (strcmp(command,"poll") == 0){
            printf("FROm poll\n");
            job = strtok(NULL," ");
            if (strcmp(job,"running")){
                printf("running\n");
                args =  queue_print(running);
                response(args);
            }else
            if (strcmp(job,"queued")){
                printf("queued\n");
                args = queue_print(queued);
                response(args);
            }


        }

        if (strcmp(command,"exit") == 0){
            unlink(server_file_name);
            sprintf(response_str,"%s" ,"Server exiting");
            response(response_str);
            queue_destroy(running);
            queue_destroy(queued);
            exit(0);
        }

        fflush(stdout);


    }

}



//function to return a response to commander

void response(const char *message) {

    char buffer[MSGSIZE+1];


    int fd2 = open(fifo2, O_WRONLY);
    if (fd2 < 0) {
        perror("Open response FIFO");
        return;
    }
    strcpy(buffer,message);
    if (write(fd2, buffer, MSGSIZE+ 1) < 0) {
        errormessage("server error in writing",2);
    }

    buffer[0] = '\0';
    strcpy(buffer,"exit");
    if (write(fd2, buffer, MSGSIZE+ 1) < 0) {
        errormessage("server error in writing",2);
    }
    
    close(fd2);
}