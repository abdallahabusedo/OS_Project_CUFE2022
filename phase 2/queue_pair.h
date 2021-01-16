#include "headers.h"

typedef struct node_pair{
	pair data;
	struct node_pair* next;
}*node_pair_ptr;

typedef struct queue_pair{
	node_pair_ptr front;
	node_pair_ptr rear;
	int count; 
}QueuePair;


QueuePair *CreateQueuePair(){
    QueuePair *q = (QueuePair * ) malloc(sizeof(QueuePair));
	q->front = NULL;
	q->rear = NULL;
	q->count = 0; 
    return q;
}

bool is_empty_queue_pair(QueuePair* q){
	return (q->front == NULL);
}

struct pair getFrontPair(QueuePair* q){
	return q->front->data;
}

struct pair getRearPair(QueuePair* q){	
	return q->rear->data;
}

bool enqueuePair(QueuePair* q, pair value){

	node_pair_ptr item = (node_pair_ptr) malloc(sizeof(struct node_pair));

	if (item == NULL)
		return false;

	item->data = value;
	item->next = NULL;

	if(q->rear == NULL){
		q->front = item;
		q->rear = item;
	} else{
		q->rear->next = item;
		q->rear = item;
	}
	q->count += 1; 
	return true;
}

pair dequeuePair(QueuePair* q){
	node_pair_ptr temp = q->front;
    pair data = q->front->data;
	q->front = q->front->next;
	free(temp);
	q->count -= 1;
	if(q->front == NULL)
		q->rear = NULL;
	return data;
}



void displayPairs(QueuePair* q){

	if(is_empty_queue_pair(q)){

		printf("\nThe queue is empty!\n");
		return;
	}

	node_pair_ptr temp = q->front;

	printf("\n[front -> ");

	while(temp != NULL){
		printf("[%d]", temp->data.start);
		temp = temp->next;
	}

	printf(" <- rear]\n");

}

int findBuddy(QueuePair* list, int buddyAddress){
    node_pair_ptr temp = list->front;
    int i = 0;
    while(temp != NULL) {

        if(temp->data.start == buddyAddress)
            return i;

        temp = temp->next;
        i++;
    }

    return -1;
}

pair getNode(QueuePair* list, int position) {

    pair data;
    if(position == 0) {
        data = dequeuePair(list);
        return data;
    }

    node_pair_ptr prev = list->front;
    
    for (int i = 0; i < position - 1; i++) {
        prev = prev->next;
    }

    node_pair_ptr temp = prev->next;
    prev->next = temp->next;
    
    if(position == list->count - 1) {
        list->rear = prev;
    }

    data = temp->data;
    free(temp);
    list->count -= 1;
    return data;
}

