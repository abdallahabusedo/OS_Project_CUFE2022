#include "headers.h"
#include "priority_queue.h"
char selAlgo; 
void insert_HPF(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process)); 
    *pp = p ; 
    proirity_enqueue(pp,p.priority);
}
void forkProcess(int Q_size);

void insert_SRTN(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    *pp = p ; 
    proirity_enqueue(pp,p.remain);
}
void insert_RR(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process )); 
    *pp = p ; 
    proirity_enqueue(pp,p.arrive);
}
struct Process ** queue;
int NextProcess = 0; 
int main(int argc, char * argv[])
{
    selAlgo = *argv[1]; 
    key_t key_id;
    int rec_val, msgq_id;
    key_id = ftok("keyfile", G_MSG_KEY);               //create unique key
    msgq_id = msgget(key_id, 0666);
    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff message;
    initClk();
    queue = get_Q(); // initialize queue
    while (true)
    { 
        rec_val = msgrcv(msgq_id, &message, sizeof(message.p), G_MSG_TYPE, IPC_NOWAIT);
        if (rec_val != -1){
            printf("scheduler: process received with arrival: %d   ========== \n\n", message.p.arrive);
            switch (selAlgo)
            {
            case SRTN: 
                insert_SRTN(message.p); 
                break;
            case HPF: 
                insert_HPF(message.p); 
                break;
            default:
                 insert_RR(message.p);  
                break;
            }   
            
        }
        int size = getQueueSize();
        if(size>0)
            forkProcess(size);

    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    destroyClk(true);
}
void forkProcess(int Q_size){
    // if current process is working do nothing 
    //else if it is finished (i guess process will send it int signal)fork the next process 
    switch (selAlgo)
    {
        case SRTN: 
            //
            
            break;
        case HPF: 
            // when finished only
            
            break;
        default:
                
            break;
    }   
}
