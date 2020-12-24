#include "headers.h"

int MAX_SIZE = __INT_MAX__; 

// returns the index of the parent node
int parent(int i) {
        return (i - 1) / 2;
}

// return the index of the left child 
int left_child(int i) {
        return 2 * i + 1;
}

// return the index of the right child 
int right_child(int i) {
        return 2 * i + 2;
}

void swap(struct Process ** x, struct Process ** y) {
        struct Process * temp = * x;
        * x = * y;
        * y = temp;
}

// insert the item at the appropriate position
void enqueue(struct Process ** a, struct Process * data, int * n,int algo) {
        if ( * n >= MAX_SIZE) {
                printf("%s\n", "The heap is full. Cannot insert");
                return;
        }
        // first insert the time at the last position of the array 
        // and move it up
        a[ * n] = data;
        * n = * n + 1;

        // move up until the heap property satisfies
        int i = * n - 1;
		switch (algo)
		{
		case RR:
			while (i != 0 && a[parent(i)]->arrive > a[i]->arrive) {
                swap( & a[parent(i)], & a[i]);
                i = parent(i);
        	}
			break;
		case SRTN:
			while (i != 0 && a[parent(i)]->remain > a[i]->remain) {
                swap( & a[parent(i)], & a[i]);
                i = parent(i);
        	}
			break;
		default:
			while (i != 0 && a[parent(i)]->priority > a[i]->priority) {
                swap( & a[parent(i)], & a[i]);
                i = parent(i);
        	}
			break;
		}
        
}

// moves the item at position i of array a
// into its appropriate position
void max_heapify(struct Process ** a, int i, int n,int algo) {
        // find left child node
        int left = left_child(i);

        // find right child node
        int right = right_child(i);

        // find the largest among 3 nodes
        int largest = i;

        // check if the left node is larger than the current node
		
		switch (algo)
			{
			case RR:
				if (left <= n && a[left]->arrive < a[largest]->arrive) {
                	largest = left;
        		}
				if (right <= n && a[right]->arrive < a[largest]->arrive) {
                	largest = right;
        		}	
				break;
			case SRTN:
				if (left <= n && a[left]->remain < a[largest]->remain) {
					largest = left;
				}
				if (right <= n && a[right]->remain < a[largest]->remain) {
               		largest = right;
        		}
				break;
			default:
				if (left <= n && a[left]->priority < a[largest]->priority) {
					largest = left;
				}
				if (right <= n && a[right]->priority < a[largest]->priority) {
               		largest = right;
        		}
				break;
			}
        
        // check if the right node is larger than the current node
        

        // swap the largest node with the current node 
        // and repeat this process until the current node is larger than 
        // the right and the left node
        if (largest != i) {
                struct Process * temp = a[i];
                a[i] = a[largest];
                a[largest] = temp;
                max_heapify(a, largest, n,algo);
        }

}

// returns the  maximum item of the heap
int get_max(int a[]) {
        return a[0];
}

// deletes the max item and return
struct Process * dequeue(struct Process ** a, int * n,int algo) {
        struct Process * max_item = a[0];

        // replace the first item with the last item
        a[0] = a[ * n - 1];
        * n = * n - 1;

        // maintain the heap property by heapifying the 
        // first item
        max_heapify(a, 0, * n,algo);
        return max_item;
}


// void proirity_enqueue(struct Process* data,int p/*remain time*/)//Enqueue function to insert data and its priority in queue
// {	
// 	N++; 
// 	int i;
// 	if((f==0)&&(r==N-1)) //Check if Queue is full
// 		printf("Queue is full");
// 	else
// 	{
// 		if(f==-1)//if Queue is empty
// 		{
// 			f = r = 0;
// 			Q[r] = data;
// 			Pr[r] = p;

// 		}
// 		else if(r == N-1)//if there there is some elemets in Queue
// 		{
// 			for(i=f;i<=r;i++) { 
// 				Q[i-f] = Q[i];
// 				Pr[i-f] = Pr[i];
// 				r = r-f; f = 0; 
// 				for(i = r;i>f;i--)
// 				{
// 					if(p<Pr[i])
// 					{
// 						Q[i+1] = Q[i];
// 						Pr[i+1] = Pr[i];
// 					}
// 					else
// 						break;
// 					Q[i+1] = data;
// 					Pr[i+1] = p;
// 					r++;
// 				}
// 			}
// 		}
// 		else
// 		{
// 			for(i = r;i>=f;i--)
// 			{
// 				if(p<Pr[i])
// 				{
// 					Q[i+1] = Q[i];
// 					Pr[i+1] = Pr[i];	
// 				}
// 				else
// 					break;
// 			}
// 			Q[i+1] = data;
// 			Pr[i+1] = p;
// 			r++;
// 		}	
// 	}

// }


// struct Process* dequeue() //remove the data from front
// {
// 	if(f == -1)
// 	{
// 		return NULL ; 
// 	}	
// 	else
// 	{
// 		//printf("deleted Element = %d\t Its Priority = %d",Q[f],Pr[f]); 
// 		if(f==r)
// 			f = r = -1;
// 		else
// 			f++;
// 		N--; 
// 		return Q[f];	
// 	}
// }
//C implementation of a max priority queue