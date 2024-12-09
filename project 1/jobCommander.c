#include "jobCommander.h"

void errormessage(char *msg,int exitcode){
    perror(msg);
    exit(exitcode);

}


void jobCommander(char *job[],int pid){

    int fd,fd2;
    char buffer[MSGSIZE+1];

    fd = open(fifo,O_WRONLY);

    if (fd < 0){
        errormessage("commander error :fifo open for write",1);


    }

    strcpy(buffer, job[1]);
    int i = 2;

    while(job[i] != NULL){
        sprintf(buffer ,"%s %s" ,buffer ,job[i]);
        i++;
    }

    if ( write(fd,buffer,MSGSIZE+1) < 0){
        errormessage("commander error : fifo write",2);
    }

    close(fd);

    fd2 = open(fifo2,O_RDWR);

    if (fd2 < 0){
        errormessage("commander error :fifo open for read",3);


    }

    while(1){
        if (read(fd2,buffer,MSGSIZE+1) < 0){
            errormessage("commander error : cant read",5);

        }
        if (strcmp(buffer,"exit") == 0)
            break;
        printf("%s\n",buffer);    
    }

    close(fd2);

















}