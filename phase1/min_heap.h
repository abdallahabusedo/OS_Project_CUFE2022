#include "headers.h"

int HEAP_SIZE = 100000;

struct Heap{
    struct Process *arr;
    int count;
    int algorithm;
};

typedef struct Heap Heap;

Heap *CreateHeap(int algorithm){
    Heap *h = (Heap * ) malloc(sizeof(Heap)); //one is number of heap

    h->count=0;
    h->arr = (struct Process *) malloc(HEAP_SIZE*sizeof(struct Process)); //size in bytes
    h->algorithm = algorithm;
    return h;
}

struct Process front(Heap * h){
    if(h->count >0){
        return h->arr[0]; 
    }
}

void heapify_bottom_top(Heap *h,int index){
    
    struct Process temp;
    int parent_node = (index-1)/2;
    if(h->algorithm == HPF){
        if(h->arr[parent_node].priority > h->arr[index].priority){
            temp = h->arr[parent_node];
            h->arr[parent_node] = h->arr[index];
            h->arr[index] = temp;
            heapify_bottom_top(h,parent_node);
        }
    } else {
        if(h->arr[parent_node].remain > h->arr[index].remain){
            temp = h->arr[parent_node];
            h->arr[parent_node] = h->arr[index];
            h->arr[index] = temp;
            heapify_bottom_top(h,parent_node);
        }
    }
}

void heapify_top_bottom(Heap *h, int parent_node){
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min;
    struct Process temp;

    if(left >= h->count || left <0)
        left = -1;
    if(right >= h->count || right <0)
        right = -1;

    if(h->algorithm == HPF) {
        if(left != -1 && h->arr[left].priority < h->arr[parent_node].priority)
            min=left;
        else
            min =parent_node;
        if(right != -1 && h->arr[right].priority < h->arr[min].priority)
            min = right;
    } else {
        if(left != -1 && h->arr[left].remain < h->arr[parent_node].remain)
            min=left;
        else
            min =parent_node;
        if(right != -1 && h->arr[right].remain < h->arr[min].remain)
            min = right;
    }
    

    if(min != parent_node){
        temp = h->arr[min];
        h->arr[min] = h->arr[parent_node];
        h->arr[parent_node] = temp;
        heapify_top_bottom(h, min);
    }
}

void enqueue(Heap *h, struct Process key){
    if( h->count < HEAP_SIZE){
        h->arr[h->count] = key;
        heapify_bottom_top(h, h->count);
        h->count++;
    }
}

struct Process dequeue(Heap *h){
    struct Process pop;
    
    // replace first node by last and delete last
    pop = h->arr[0];
    h->arr[0] = h->arr[h->count-1];
    h->count--;
    heapify_top_bottom(h, 0);
    return pop;
}
