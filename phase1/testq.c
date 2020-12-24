#include "headers.h"
#include "min_heap.h"

int main(){
    
    struct Process p1, p2, p3; 
    p1.arrive = 1; 
    p1.id = 1; 
    p1.priority = 5; 
    p1.remain = 6; 
    p1.runtime = 2; 
    
    p2.arrive = 2; 
    p2.id = 1; 
    p2.priority =7 ; 
    p2.remain = 2; 
    p2.runtime = 3;

    p3.arrive = 3; 
    p3.id = 1; 
    p3.priority =1 ; 
    p3.remain = 1; 
    p3.runtime = 3;

    Heap* priorityQueue = CreateHeap(SRTN);
    
    enqueue(priorityQueue, p1);
    enqueue(priorityQueue, p2);
    enqueue(priorityQueue, p3);

    for (int i = 0; i < 3; i++) {
        printf("process arrive %d \n\n", dequeue(priorityQueue).arrive);

    }
    


    return 0; 
}