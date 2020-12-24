#include "headers.h"
#include "priority_queue.h"

struct Process ** queue;
int size = 0;
char selAlgo; 
void insert(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    *pp = p ; 
    enqueue(queue,pp,&size,selAlgo);
}
int main(){
    
    queue = (struct Process **) malloc(sizeof(struct Process*));
    selAlgo = RR; 
    struct Process p; 
    p.arrive = 1; 
    p.id = 1; 
    p.priority =1 ; 
    p.remain = 3; 
    p.runtime = 3; 
    insert(p); 
    p.arrive = 3; 
    insert(p); 
    p.arrive = 2; 
    insert(p); 
    for (int i = 0; i < size; i++)
    {
        printf("process arrive %d \n\n",dequeue(queue,&size,selAlgo)->arrive);
    }
    


    return 0; 
}