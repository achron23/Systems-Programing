#include "queue.h"
#include <string.h>
#include <stdio.h>

queue* queue_create(int concurrency ){

    queue* q = malloc(sizeof(queue));
    q->concurrency = concurrency;
    q->size = 0;
    return q;

}

void queue_insert(queue* q,char* job,int jobID,int pid){

    if (q == NULL) return;
    
    qnode* new_node = malloc(sizeof(qnode));
    strcpy(new_node->job,job);
    new_node->jobID = jobID;
    new_node->next = NULL;
    new_node->pid = pid;
    
    
    if (q->node == NULL){
        q->node = new_node;

    }else{
        qnode* current = q->node;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = new_node;
    }
   
    q->size ++;
    

}

qnode* queue_remove(queue* q){
    if (q == NULL)
        return NULL;
    qnode * temp = q->node;
    q->node = q->node->next;
    q->size--;
    return temp;

}

queue* queue_remove_id(queue* qu , int id){
    if (qu == NULL || qu->node == NULL) return qu;  // Handle empty queue

    qnode *current = qu->node;
    qnode *prev = NULL;

    while (current != NULL) {
        if (current->jobID == id) {
            // Node to remove is found
            if (prev == NULL) {
                // The node to remove is the first node
                qu->node = current->next;
            } else {
                // The node to remove is not the first node
                prev->next = current->next;
            }
            // Free the node
            free(current);
            break;  // Exit the loop after removing the node
        }
        prev = current;
        current = current->next;
    }

    return qu;
}


char* queue_print(queue* q){
    
	int i;
	char buffer[500];
    char *args = malloc(500);
    qnode* temp = q->node;
    args[0] = '\0';
	while (temp != NULL) {
		sprintf(buffer, "job_%d\t%s\t%d\n", temp->jobID, temp->job, temp->pid);
		temp = temp->next;
        strcat(args,buffer);
	}
	return args;
}


void queue_destroy(queue* q){
    while (q->size > 0 ){
        qnode* to_free = queue_remove(q);
        free(to_free);
    }
    free(q);
}