
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "queue.h"


extern int MSGSIZE;
extern char* fifo;
extern char* fifo2;
extern char* server_file_name;

void jobExecutorServer();
void errormessage(char *,int );
void jobCommander(char *[],int);
void response(const char *);
void update_running();
void issueJob(queue* , queue *, char *, int );
void handler(int sig);


void parse_args(char *, char** );