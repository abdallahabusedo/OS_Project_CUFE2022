#include "headers.h"

typedef struct node{

	struct Process data;
	struct node* previous;

}*node_ptr;

typedef struct queue{
	node_ptr front;
	node_ptr rear;
}Queue;


Queue *CreateQueue(){
    Queue *q = (Queue * ) malloc(sizeof(Queue));
	q->front = NULL;
	q->rear = NULL;
    return q;
}

bool is_empty(Queue* q){
	return (q->front == NULL);
}

struct Process getFront(Queue* q){
	return q->front->data;
}

struct Process getRear(Queue* q){	
	return q->rear->data;
}

bool enqueueQ(Queue* q, struct Process value){

	node_ptr item = (node_ptr) malloc(sizeof(struct node));

	if (item == NULL)
		return false;

	item->data = value;
	item->previous = NULL;

	if(q->rear == NULL)
		q->front = q->rear = item;
	else{
		q->rear->previous = item;
		q->rear = item;
	}

	return true;
}

struct Process dequeueQ(Queue* q){

	node_ptr temp = q->front;
	struct Process data = q->front->data;
	q->front = q->front->previous;
	free(temp);

	return data;
}

void display(Queue* q){

	if(is_empty(q)){

		printf("\nThe queue is empty!\n");
		return;
	}

	node_ptr temp = q->front;

	printf("\n[front -> ");

	while(temp != NULL){
		printf("[%d]", temp->data.arrive);
		temp = temp->previous;
	}

	printf(" <- rear]\n");

}

// void main(){

// 	int option = 0, value = 0;

// 	do{
		
// 		printf("\nQUEUE Menu\n"
// 			   "\n1 - Is Empty?"
// 			   "\n2 - Enqueue"
// 			   "\n3 - Dequeue"
// 			   "\n4 - Show FRONT"
// 			   "\n5 - Show REAR"
// 			   "\n6 - Display elements"
// 			   "\n7 - Clear queue"
// 			   "\n\n0 - EXIT\n\n");

// 		printf("option: ");
// 		scanf("%d", &option);

// 		switch(option){

// 			case 0:
// 				if(front != NULL)
// 					clear();
// 				break;
				
// 			case 1:
// 				printf(is_empty() ? "\nTrue\n" : "\nFalse\n");
// 				break;

// 			case 2:
// 				printf("\nInsert a integer value to enqueue: ");
// 				scanf("%d", &value);
// 				printf(enqueue(value) ? "\nEnqueued :)\n" : "\nSomething went wrong :(\n");
// 				break;

// 			case 3:
// 				printf(dequeue() ? "\nDequeued :)\n" : "\nSomething went wrong :(\n");
// 				break;

// 			case 4:
// 				display_front();
// 				break;

// 			case 5:
// 				display_rear();
// 				break;

// 			case 6:
// 				display();
// 				break;

// 			case 7:
// 				printf(clear() ? "\nCleared :)\n": "\nSomething went wrong :(\n");
// 				break;

// 			default:
// 				printf("\nINVALID OPTION!!!\n");
// 				break;

// 		}

// 	}while(option != 0);

// 	printf("\n\n** See more at: https://github.com/rafaeltardivo \n\n");
// }