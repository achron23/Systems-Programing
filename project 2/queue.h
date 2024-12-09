// queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include <string.h>
#include <pthread.h>
#include "stdlib.h"


typedef struct {
    char jobID[20];
    char job[256];
    int client_sock;
    
    
} job_entry;

typedef struct {
    job_entry *jobs;
    int front;
    int flag ;
    int rear;
    int count;
    int threadPoolsize;
    pthread_mutex_t mutex;
    pthread_cond_t cond_nonempty;
    pthread_cond_t cond_nonfull;
} job_queue;

void init_queue(job_queue *q, int threadpoolSize);
int enqueue(job_queue *q, const char *jobID, const char *job, int client_sock);
int dequeue(job_queue *q, job_entry *entry);
int remove_job_by_id(job_queue *q,const char *jobID);
void destroy_queue(job_queue *q);

#endif // QUEUE_H