#include "headers.h"


void clearResources(int);

// input file name that holds processes 
const char* PROCESSES_FILE_NAME = "../processes.txt";
struct Process** processes; // array of processes 
int P_N=0; // number of processs

// get processes count in input file
int getProcessesCount(){
     FILE * file = fopen(PROCESSES_FILE_NAME,"r"); 
    if(!file){
        printf("not able to read processes"); 
        exit(-1);
    }
    int n = 0; 
    size_t buf_size = 0; 
    char * line; 
    int i = 0; 
    while(getline(&line,&buf_size,file) > 0){
        if(line[0]=='#') continue;
        i++;
        // loop through the string to extract all other token
    }
    fclose(file); 
    return i; 
}
// read processes from input file 
void readProcesses(int * count){
    FILE * file = fopen(PROCESSES_FILE_NAME,"r"); 
    if(!file){
        printf("not able to read processes"); 
        exit(-1);
    }
    int n = 0; 
    size_t buf_size = 0; 
    char * line; 
    
    P_N = getProcessesCount(); 
    processes = malloc(P_N * sizeof(struct Process* ));

    fseek(file, 0, SEEK_SET);

    int i = 0; 
    while(getline(&line,&buf_size,file) > 0){
        if(line[0]=='#') continue;
        processes[i] = (struct Process *) malloc(sizeof(struct Process));
        char *id = strtok(line, "\t");
        char *arrive = strtok(NULL, "\t");
        char *runtime = strtok(NULL, "\t");
        char *priority = strtok(NULL,"\t");
        char *memsize = strtok(NULL, "\t");
        if(id == NULL || arrive == NULL || runtime == NULL || priority == NULL || memsize == NULL){
            printf("\ninput file format is not correct \n\n");
            exit(-1); 
        }
        processes[i]->id = atoi(id); 
        processes[i]->arrive = atoi(arrive); 
        processes[i]->runtime = atoi(runtime); 
        processes[i]->priority = atoi(priority);
        processes[i]->remain = processes[i]->runtime; 
        processes[i]->memsize = atoi(memsize);
        // printf("processes memory size = %d \n\n", processes[i]->memsize);
        i++;
        // loop through the string to extract all other tokens
    }
    *count = i; 
    fclose(file); 
}
// RR quantum from user 
int RR_QUANTUM = 2 ; 

// get algorithm number from user 
char * readAlgoNum()
{
    printf("Please select one of these algorithms: \n");
    printf("1) Non-preemptive Highest Priority First (HPF).\n");
    printf("2) Shortest Remaining time Next (SRTN).\n");
    printf("3) Round Robin (RR).\n");
    char c; 
    scanf("%c",&c); 
    if (c != '1' && c != '2' && c != '3')
    {
        printf("Please select one of the following algorithms numbers 1 , 2 or 3\n");
        readAlgoNum();
    }
    else
    {
        if(c == '3'){
            printf("enter RR quantum\n"); 
            scanf("%d",&RR_QUANTUM); 
        }
        char * selected_algorithm_Number  = malloc(sizeof(char)); 
        *selected_algorithm_Number = c; 
        return selected_algorithm_Number;
    }
}

// index of process to send 
int Next = 0; 

// start sending proceeses to schedular
void generateProcesses(int N,int msgq_id){
    int clk;
    // messages params
    //loop over processes
    while(Next < N){
        clk = getClk();

        // while process's arrival time is equal to clock send it to scheduler 
        while(Next < N && processes[Next] != NULL && processes[Next]->arrive == clk){
            //send message
            struct msgbuff message;
            message.mtype = G_MSG_TYPE; 
            message.p = *processes[Next]; 

            // check if there's a process in input file with same arrival time 
            // set is last to true if there isn't 
            message.isLast = !(Next+1 < N && processes[Next+1] != NULL && processes[Next+1]->arrive == clk);
            int send_val = msgsnd(msgq_id,&message,sizeof(struct Process)+ sizeof(bool),IPC_NOWAIT);
            if(send_val == -1){
                printf("failed to send process");
                break; 
            }
            Next++; 
        }
    }
}
// get message queue 
int createMsgChannel(){
    key_t key_id;
    int msgq_id, send_val;
    key_id = ftok("keyfile", G_MSG_KEY);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    if (msgq_id == -1){
        perror("Error in create");
        exit(-1); 
    }
    return msgq_id; 
}
int msgq_id ; 
int main(int argc, char *argv[])
{
    // handle interrupt signal 
    signal(SIGINT, clearResources);

    // 1. Read the input files.
    readProcesses(&P_N); 
   
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char * selAlgo = readAlgoNum();

    // get message queue
    msgq_id = createMsgChannel(); 
    
    // 3. Initiate and create the scheduler and clock processes.
    int clk_pid, clk_stat_loc;
    clk_pid = fork();
    if (clk_pid == -1)
        perror("error in fork");
    else if (clk_pid == 0)
    { 
       // printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        execl("clk.out", "clk.out", NULL);
    }
    // fork schedular 
    int sch_pid = fork(),sch_stat_loc;
    if (sch_pid == -1)
        perror("error in fork");
    else if (sch_pid == 0)
    { 
        //printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        char str[10];
        sprintf(str, "%d", P_N);
        char per[10];
        sprintf(per, "%d", RR_QUANTUM);
        execl("scheduler.out", "scheduler.out",selAlgo, str,per, NULL);
    }
  
    // initialize clock
    initClk();
    
    // start sending processes to schedular on time
    generateProcesses(P_N,msgq_id); 

    // wait for schedular to terminate
    waitpid(sch_pid,&sch_stat_loc,0); 
    // destroyClk(true);
}

// clear resources 
void clearResources(int signum)
{
    msgctl(msgq_id, IPC_RMID, NULL);
    for (int i = 0; i < P_N; i++) {
        free(processes[i]); 
    }
    free(processes);
    printf("process_generator is terminating\n");
    exit(0);
}
