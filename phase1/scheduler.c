#include "headers.h"
#include "priority_queue.h"
#define RR_PERIOD 2
void insert_HPF(struct Process p){
    struct Process *pp = (struct Process *) malloc(sizeof(struct Process)); 
    *pp = p ; 
    proirity_enqueue(pp,p.priority);
}
void forkProcess();
void switchProcess();
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
int size;
char selAlgo; 
struct Process *curr_process;
int main(int argc, char * argv[])
{
    signal(SIGCHLD,switchProcess);
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
    int shmid, pid;
    shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    bool first = true; 
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
        size = getQueueSize();
        if(size>0&& first){
            forkProcess();
            first = false; 
        }

    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    destroyClk(true);
}
int getRemainOfLast()
{
    int shmid, pid;
    shmid = shmget(IPC_PRIVATE, 4096, IPC_CREAT | 0644);
    if (shmid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    int *shmaddr = (int *) shmat(shmid, (void *)0, 0);
    if (*shmaddr == -1)
    {
        perror("Error in attach in reader");
        exit(-1);
    }
    int remain = (*shmaddr); 
    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
    return remain   ; 
   
}
void switchProcess(){
    int remainOfLast = getRemainOfLast(); 
    if(selAlgo == RR) {
        if(remainOfLast != 0){
            curr_process->remain = remainOfLast; 
            proirity_enqueue(curr_process,curr_process->remain);
        }
    }
    if(getQueueSize()>0){
        forkProcess(); 
    }
}

void forkProcess(){
    int sch_pid = fork(),sch_stat_loc;
    if (sch_pid == -1)
        perror("error in fork");
    else if (sch_pid == 0)
    { 
        int remaining_time ; 
        curr_process  = dequeue(); 
        switch (selAlgo)
        {
            case RR: 
                remaining_time = RR_PERIOD;  
                break;
            default:
                remaining_time = curr_process->runtime; 
                break;
        }
        //printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        printf("start process with arrival %d at time %d \n\n",curr_process->arrive,getClk());
        if(RR == selAlgo)
            execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/p.o", "p.o",remaining_time,curr_process->remain, NULL);
        else{
            execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/p.o", "p.o",remaining_time, NULL);
        }
    }
}
   

