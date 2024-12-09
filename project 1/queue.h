#include <stdlib.h>



typedef struct qnode{
    char job[100];
    int jobID;
    int pid;
    int pos;
    struct qnode *next;
} qnode;

typedef struct queue{
    int concurrency  ;
    int size;
    qnode *node;
} queue;

extern queue* running;
extern queue* queued ;

queue* queue_create(int);

char* queue_print(queue*);

void queue_insert(queue*,char *,int,int);

qnode* queue_remove(queue*);

queue* queue_remove_id(queue* , int);

void queue_destroy(queue*);