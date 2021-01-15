#include "min_heap.h"
#include "queue.h"

struct data_struct{
    Heap *h; 
    Queue *q; 
    int algo; 
};

typedef struct data_struct Dstruct; 

// create data structure based on algorithm given 
Dstruct* CreateStruct(int algorithm){
    Dstruct * s = (Dstruct *) malloc(sizeof(Dstruct)); 
    s->algo = algorithm; 
    if(algorithm == RR){
         s->q = CreateQueue();
    }else
         s->h = CreateHeap(algorithm); 
    return s;
}

// insert porcess in ready queue
void enqueue(Dstruct * s, struct Process p){
    if(s->algo == RR) enqueueQ(s->q,p); 
    else enqueueH(s->h,p); 
}

// get first processes  in ready queue 
struct Process dequeue(Dstruct * s){
    if(s->algo == RR) return dequeueQ(s->q); 
    return dequeueH(s->h); 
}
// get count of ready processes
int getcount(Dstruct * s){
        if(s->algo ==RR){
           return s->q->count; 
        }
        return s->h->count; 
}

//check up front of ready queue
struct Process front(Dstruct * s){
    if(s->algo == RR){
        return getFront(s->q);
    }
    return frontH(s->h); 
}

void displayS(Dstruct * s){
    if(s->algo == RR) display(s->q);
}