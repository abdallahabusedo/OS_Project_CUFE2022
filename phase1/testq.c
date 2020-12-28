#include "headers.h"
#include "struct.h"

void build(Dstruct * h){
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
    enqueue(h, p1);
    enqueue(h, p2);
    enqueue(h, p3);

}

void buildQ(Queue* q){
    struct Process p1, p2, p3; 
    p1.arrive = 5; 
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
    enqueueQ(q, p1);
    enqueueQ(q, p2);
    enqueueQ(q, p3);

}

int main(){
    
    // Heap* priorityQueue = CreateHeap(SRTN);
    // build(priorityQueue);
    
   
    // for (int i = 0; i < 3; i++) {
    //     printf("process arrive %d \n\n", dequeue(priorityQueue).arrive);

    // }

    Dstruct* s = CreateStruct(2);

    //buildQ(q);
    build(s); 

    for (int i = 0; i < 3; i++) {
        printf("process arrive %d \n\n", dequeue(s).arrive);
    }

    return 0; 
}