#pragma once
#include<stdio.h>
int N = 0; 

struct Process{
    int id; 
    int arrive; 
    int runtime; 
    int priority; 
    int remain;
}; 
struct Process** Q;
int * Pr;
int r = -1,f = -1;
struct Process** get_Q(){
	// N = 0; 
	// r = -1;f = -1;
	// for (int i = 0; i < count; i++)
	// {
	// 	proirity_enqueue(processes[i],processes[i]->runtime); 
	// }
	Q = (struct Process**) malloc(__SIZEOF_POINTER__);
	Pr = (int * ) malloc(__SIZEOF_POINTER__); 
	return  Q; 
}
int getQueueSize(){
	return N; 
}

// struct Process** getRR_Q(struct Process** processes, int count){
// 	N = 0; 
// 	r = -1;f = -1;
// 	for (int i = 0; i < count; i++)
// 	{
// 		proirity_enqueue(processes[i],processes[i]->arrive); 
// 	}
// 	return Q; 
// }
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

int dequeue() //remove the data from front
{
	if(f == -1)
	{
		printf("Queue is Empty");
	}	
	else
	{
		//printf("deleted Element = %d\t Its Priority = %d",Q[f],Pr[f]);
		if(f==r)
			f = r = -1;
		else
			f++;
	}
}