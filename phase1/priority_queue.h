#include "headers.h"
#pragma once
int N = 0; 
struct Process** Q;
int * Pr;
int r = -1,f = -1;
struct Process** get_Q(){
	Q = (struct Process**) malloc(sizeof(struct Process *));
	Pr = (int * ) malloc(sizeof(int)); 
	return  Q; 
}
int getQueueSize(){
	return N; 
}

void proirity_enqueue(struct Process* data,int p/*remain time*/)//Enqueue function to insert data and its priority in queue
{	
	N++; 
	int i;
	if((f==0)&&(r==N-1)) //Check if Queue is full
		printf("Queue is full");
	else
	{
		if(f==-1)//if Queue is empty
		{
			f = r = 0;
			Q[r] = data;
			Pr[r] = p;

		}
		else if(r == N-1)//if there there is some elemets in Queue
		{
			for(i=f;i<=r;i++) { 
				Q[i-f] = Q[i];
				Pr[i-f] = Pr[i];
				r = r-f; f = 0; 
				for(i = r;i>f;i--)
				{
					if(p<Pr[i])
					{
						Q[i+1] = Q[i];
						Pr[i+1] = Pr[i];
					}
					else
						break;
					Q[i+1] = data;
					Pr[i+1] = p;
					r++;
				}
			}
		}
		else
		{
			for(i = r;i>=f;i--)
			{
				if(p<Pr[i])
				{
					Q[i+1] = Q[i];
					Pr[i+1] = Pr[i];	
				}
				else
					break;
			}
			Q[i+1] = data;
			Pr[i+1] = p;
			r++;
		}	
	}

}


struct Process* dequeue() //remove the data from front
{
	if(f == -1)
	{
		return NULL ; 
	}	
	else
	{
		//printf("deleted Element = %d\t Its Priority = %d",Q[f],Pr[f]); 
		if(f==r)
			f = r = -1;
		else
			f++;
		return Q[f];	
	}
}
// // C implementation of a max priority queue
// #include < stdio.h >

//         #define MAX_SIZE 15

// // returns the index of the parent node
// int parent(int i) {
//         return (i - 1) / 2;
// }

// // return the index of the left child 
// int left_child(int i) {
//         return 2 * i + 1;
// }

// // return the index of the right child 
// int right_child(int i) {
//         return 2 * i + 2;
// }

// void swap(int * x, int * y) {
//         int temp = * x;
//         * x = * y;
//         * y = temp;
// }

// // insert the item at the appropriate position
// void enqueue(int a[], int data, int * n) {
//         if ( * n >= MAX_SIZE) {
//                 printf("%s\n", "The heap is full. Cannot insert");
//                 return;
//         }
//         // first insert the time at the last position of the array 
//         // and move it up
//         a[ * n] = data;
//         * n = * n + 1;

//         // move up until the heap property satisfies
//         int i = * n - 1;
//         while (i != 0 && a[parent(i)] < a[i]) {
//                 swap( & a[parent(i)], & a[i]);
//                 i = parent(i);
//         }
// }

// // moves the item at position i of array a
// // into its appropriate position
// void max_heapify(int a[], int i, int n) {
//         // find left child node
//         int left = left_child(i);

//         // find right child node
//         int right = right_child(i);

//         // find the largest among 3 nodes
//         int largest = i;

//         // check if the left node is larger than the current node
//         if (left <= n && a[left] > a[largest]) {
//                 largest = left;
//         }

//         // check if the right node is larger than the current node
//         if (right <= n && a[right] > a[largest]) {
//                 largest = right;
//         }

//         // swap the largest node with the current node 
//         // and repeat this process until the current node is larger than 
//         // the right and the left node
//         if (largest != i) {
//                 int temp = a[i];
//                 a[i] = a[largest];
//                 a[largest] = temp;
//                 max_heapify(a, largest, n);
//         }

// }

// // returns the  maximum item of the heap
// int get_max(int a[]) {
//         return a[0];
// }

// // deletes the max item and return
// int dequeue(int a[], int * n) {
//         int max_item = a[0];

//         // replace the first item with the last item
//         a[0] = a[ * n - 1];
//         * n = * n - 1;

//         // maintain the heap property by heapifying the 
//         // first item
//         max_heapify(a, 0, * n);
//         return max_item;
// }