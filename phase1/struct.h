#include "min_heap.h"
#include "queue.h"

struct data_struct{
    Heap *h; 
    Queue *q; 
    int algo; 
};
typedef struct data_struct Dstruct; 

Dstruct* CreateStruct(int algorithm){
    Dstruct * s = (Dstruct *) malloc(sizeof(Dstruct)); 
    s->algo = algorithm; 
    if(algorithm == RR){
         s->q = CreateQueue();
    }else
         s->h = CreateHeap(algorithm); 
    return s;
}

void enqueue(Dstruct * s, struct Process p){
    if(s->algo == RR) enqueueQ(s->q,p); 
    else enqueueH(s->h,p); 
}

struct Process dequeue(Dstruct * s){
    if(s->algo == RR) return dequeueQ(s->q); 
    return dequeueH(s->h); 
}
int getcout(Dstruct * s){
        if(s->algo ==RR){
           return s->q->count; 
        }
        return s->h->count; 
}

struct Process front(Dstruct * s){
    if(s->algo == RR){
        return getFront(s->q);
    }
    return frontH(s->h); 
}