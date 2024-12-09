
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

void errormessage(char *msg,int exitcode){
    perror(msg);
    exit(exitcode);
}



void send_command(const char *server,int port,const char *command){
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;
    struct hostent *he;
    struct in_addr **addr_list;

    //Resolve the server name
    if ((he = gethostbyname(server)) == NULL){
        errormessage("gethostbyname faile", 1);
    }

    addr_list = (struct in_addr **)he->h_addr_list;


    //Create socket
    if ((sock = socket(AF_INET,SOCK_STREAM,0)) < 0){
        errormessage("Socket create error",1);

    }

    //Setup address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *addr_list[0];

    //connect to server
    if (connect(sock,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0 ){
        errormessage("Connection failed",1);
    }

    //Send command
    snprintf(buffer,sizeof(buffer),"%s",command);

    if (send(sock,buffer,strlen(buffer),0) < 0){
        errormessage("Send failed",1);
    }

    //wait for server response
//
    while ((bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0 ){
        buffer[bytes_read] = '\0';
        printf("Server response: %s\n" , buffer);
    }
    if (bytes_read < 0 )
        errormessage("Receive failed" , 1);
   
    close(sock);


}


int main(int argc,char *argv[]){
    if (argc < 4){
        fprintf(stderr,"Usage : ./jobCommander <serverName> <portNum> \"<jobCommanderInputCommand>\"");
    }

    const char *serverName = argv[1];
    int portNum = atoi(argv[2]);
    // Combine all arguments after argv[2] into a single command string
    char command[BUFFER_SIZE];
    memset(command, 0, BUFFER_SIZE);
    for (int i = 3; i < argc; i++) {
        strcat(command, argv[i]);
        if (i < argc - 1) {
            strcat(command, " ");
        }
    }

    send_command(serverName,portNum,command);


    
}