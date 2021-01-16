#include "headers.h"
#include "queue_pair.h"

#define listsSize 9

QueuePair* lists[listsSize];

void InitializeMemory(){
    for (int i = 0; i < listsSize; i++) {
        lists[i] = CreateQueuePair();
    }
    
    pair temp1,temp2,temp3,temp4;
    temp1.start = 0;
    temp1.end = 255;
    temp2.start = 256;
    temp2.end = 511;
    temp3.start = 512;
    temp3.end = 767;
    temp4.start = 768;
    temp4.end = 1023;
    enqueuePair(lists[listsSize - 1],temp1);
    enqueuePair(lists[listsSize - 1],temp2);
    enqueuePair(lists[listsSize - 1],temp3);
    enqueuePair(lists[listsSize - 1],temp4);
}

pair allocate(int size) {
    int nearestPow2 = (int) ceil(log2((double)size));
    // printf("p2:%d\n", nearestPow2);
    
    if(lists[nearestPow2]->count > 0) {
        pair temp = dequeuePair(lists[nearestPow2]);
        return temp;
    } else {
        for (int i = nearestPow2+1; i < listsSize; i++) {
            // printf("count:%d\n",lists[i]->count);
            if(lists[i]->count > 0) {
                
                pair temp = dequeuePair(lists[i]);
                // printf("temp: %d, %d\n", temp.start, temp.end);

                for (int j = i-1; j >= nearestPow2; j--) {
                    pair temp1, temp2;
                    temp1.start = temp.start;
                    temp1.end = temp.start + (temp.end - temp.start)/2;
                    temp2.start = temp.start + (temp.end - temp.start + 1)/2;
                    temp2.end = temp.end; 
                    // printf("temp1: %d, %d\n", temp1.start, temp1.end);
                    // printf("temp2: %d, %d\n", temp2.start, temp2.end);
                    // printf("j=%d\n",j);
                    enqueuePair(lists[j], temp2);
                    temp = temp1;
                }
                return temp;
            }
        }
        
        pair temp;
        temp.start = 0;
        temp.end = 0;
        return temp;
    }
}

void mergeBuddy(int n, int size, pair block){
    if(size == 256){
        enqueuePair(lists[n], block);
        return;
    }
    
    int buddyNumber, buddyAddress;
    buddyNumber = block.start / size;  
  
    if (buddyNumber % 2 != 0) 
        buddyAddress = block.start - size; 
    else
        buddyAddress = block.start + size;

    int position = findBuddy(lists[n], buddyAddress);
    if(position != -1) {
        pair buddy = getNode(lists[n], position);
        
        pair temp;
        if (buddyNumber % 2 == 0) {
            temp.start = block.start;
            temp.end = buddy.end;         
        } else {
            temp.start = buddy.start;
            temp.end = block.end; 
        } 

        // printf("(%d, %d) and (%d, %d) merged to (%d, %d)\n", block.start, block.end, buddy.start, buddy.end, temp.start, temp.end);
        mergeBuddy(n+1, 2*size, temp);
    } else {
        enqueuePair(lists[n], block);
    }

}

void deallocate(pair block) {

    int size = (block.end - block.start + 1);
    int nearestPow2 = (int) ceil(log2((double)size));
    mergeBuddy(nearestPow2, size, block);
}

void printMemory(){
    printf("-------------------------------------------------\n");
    for (int i = 0; i < 9; i++) {
        printf("%d:{", i);
        int n = lists[i]->count;
        for (int j = 0; j < n; j++) {
            pair temp = dequeuePair(lists[i]);
            printf("(%d, %d), ", temp.start, temp.end);
            enqueuePair(lists[i], temp);
        }
        printf("}\n");
    }
    printf("-------------------------------------------------\n");
}

