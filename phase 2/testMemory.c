#include "memory.h"


int main () {
    InitializeMemory();
    pair p1 = allocate(16);
    printMemory(); 
    pair p2 = allocate(16);  
    printMemory(); 
    pair p3 = allocate(16);  
    printMemory(); 
    pair p4 = allocate(16);
    printMemory(); 
    pair p5 = allocate(16);
    printMemory(); 
    // deallocate(0);  
    // deallocate(9);  
    // deallocate(32);  
    // deallocate(16);

    // pair p6 = allocate(255);
    // pair p7 = allocate(255);
    // pair p8 = allocate(255);
    // printf("%d, %d\n", p1.start, p1.end);
    // printf("%d, %d\n", p2.start, p2.end);
    // printf("%d, %d\n", p3.start, p3.end);
    // printf("%d, %d\n", p4.start, p4.end);
    // printf("%d, %d\n", p5.start, p5.end);
    // printf("%d, %d\n", p6.start, p6.end);
    // printf("%d, %d\n", p7.start, p7.end);
    // printf("%d, %d\n", p8.start, p8.end);

    // pair p = allocate(8);
    // printf("%d, %d\n", p.start, p.end);

    // printMemory();

    // deallocate(p);

    // printMemory();

    // pair p2 = allocate(256);
    // printf("%d, %d\n", p2.start, p2.end);
    // pair p3 = allocate(256);
    // printf("%d, %d\n", p3.start, p3.end);
    // pair p4 = allocate(256);
    // printf("%d, %d\n", p4.start, p4.end);
    // pair p5 = allocate(256);
    // printf("%d, %d\n", p5.start, p5.end);
    // pair p6 = allocate(256);
    // printf("%d, %d\n", p6.start, p6.end);

    // printMemory();
    return 0;
}