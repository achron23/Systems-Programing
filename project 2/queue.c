// queue.c
#include "queue.h"


void init_queue(job_queue *q,int threadPoolSize) {

    q->jobs = malloc(sizeof(job_entry) *threadPoolSize);
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->flag = 1;
    q->threadPoolsize = threadPoolSize;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond_nonempty, NULL);
    pthread_cond_init(&q->cond_nonfull, NULL);
}

int enqueue(job_queue *q, const char *jobID, const char *job , int client_sock) {
    pthread_mutex_lock(&q->mutex);
    if (!q->flag )
        return -1;
    while (q->count == q->threadPoolsize ) {
        pthread_cond_wait(&q->cond_nonfull, &q->mutex);
    }
    q->rear = (q->rear + 1) % q->threadPoolsize;
    strcpy(q->jobs[q->rear].jobID, jobID);
    strcpy(q->jobs[q->rear].job, job);
    q->jobs[q->rear].client_sock = client_sock;
    q->count++;
    pthread_cond_signal(&q->cond_nonempty);
    pthread_mutex_unlock(&q->mutex);
    return 0; // Success
}

int dequeue(job_queue *q, job_entry *entry) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0 && q->flag) {
        pthread_cond_wait(&q->cond_nonempty, &q->mutex);
    }
    if (q->count == 0){
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    *entry = q->jobs[q->front];
    q->front = (q->front + 1) % q->threadPoolsize;
    q->count--;
    pthread_cond_signal(&q->cond_nonfull);
    pthread_mutex_unlock(&q->mutex);
    return 1; // Success
}

int remove_job_by_id(job_queue *q,const char *jobID){
    pthread_mutex_lock(&q->mutex);
    for (int i = 0; i < q->count; i++){
        int idx = (q->front + i) % q->threadPoolsize;
        if (strcmp(q->jobs[idx].jobID,jobID) == 0){
            // Shift all subsequent jobs forward
            for (int j = i; j < q->count - 1; j++) {
                int from_idx = (q->front + j + 1) % q->threadPoolsize;
                int to_idx = (q->front + j) % q->threadPoolsize;
                q->jobs[to_idx] = q->jobs[from_idx];
            }
            q->rear = (q->rear - 1 + q->threadPoolsize) % q->threadPoolsize;
            q->count--;
            pthread_cond_signal(&q->cond_nonfull);
            pthread_mutex_unlock(&q->mutex);
            return 1; // Success
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return -1; // Job not found
}



void destroy_queue(job_queue *q){
    free(q->jobs);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond_nonempty);
    pthread_cond_destroy(&q->cond_nonfull);

}