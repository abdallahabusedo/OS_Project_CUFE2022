#include "headers.h"
#include "priority_queue.h"

void clearResources(int);

const char* PROCESSES_FILE_NAME = "../processes.txt";
struct Process* processes[__INT8_MAX__];

void readProcesses(struct Process ** processes ,int * count){
    FILE * file = fopen(PROCESSES_FILE_NAME,"r"); 
    if(!file){
        printf("not able to read processes"); 
        exit(-1);                                   // what should we do here ? 
    }
    char * line; 
    int i = 0; 
    size_t buf_size = 0; 
    while(getline(&line,&buf_size,file) > 0){
        if(line[0]=='#') continue; 
        processes[i] = (struct Process *) malloc(sizeof(struct Process*));
        char *id = strtok(line, "\t");
        processes[i]->id = atoi(id); 
        char *arrive = strtok(NULL, "\t");
        processes[i]->arrive = atoi(arrive); 
        char *runtime = strtok(NULL, "\t");
        processes[i]->runtime = atoi(runtime); 
        char *priority = strtok(NULL, "\t");
        processes[i]->priority = atoi(priority);
        processes[i]->remain = processes[i]->runtime; 
        i++; 
        // loop through the string to extract all other tokens
       
    }
    *count = i; 
    fclose(file); 
}


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
        char * selected_algorithm_Number  = malloc(sizeof(char*)); 
        *selected_algorithm_Number = c; 
        return selected_algorithm_Number;
    }
}


int Next = 0; 
void generateProcesses(struct Process ** processes,int N,int msgq_id){
    int clk;
    // messages params
    //loop over processes
    while(Next < N){
        clk = getClk();
        //printf("process generator at clock %d \n\n",clk); 
        if(processes[Next] != NULL && processes[Next]->arrive == clk){
            //send message
            struct msgbuff message;
            message.mtype = G_MSG_TYPE; 
            message.p = *processes[Next]; 
            int send_val = msgsnd(msgq_id,&message,sizeof(struct Process),IPC_NOWAIT);
            if(send_val == -1){
                printf("failed to send process");
                break; 
            }
            printf("process with arrival: %d send on time %d \n",processes[Next]->arrive,clk);
            Next++; 
        }
    }
}
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
 int P_N = 0;  int msgq_id ; 
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.
   
    readProcesses(processes,&P_N); 
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char * selAlgo = readAlgoNum();

    msgq_id = createMsgChannel(); 
    
    // 3. Initiate and create the scheduler and clock processes.
    // initialize clock
    int clk_pid, clk_stat_loc;
    clk_pid = fork();
    if (clk_pid == -1)
        perror("error in fork");
    else if (clk_pid == 0)
    { 
       // printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/clk.o", "clk.o", NULL);
    }
    // fork schedular 
    int sch_pid = fork(),sch_stat_loc;
    if (sch_pid == -1)
        perror("error in fork");
    else if (sch_pid == 0)
    { 
        //printf("\nI am the child, my pid = %d and my parent's pid = %d\n\n", getpid(), getppid());
        execl("/home/khalid/OS/OS_Project_CUFE2022/phase1/sch.o", "sch.o",selAlgo, NULL);
    }
    // send processes to schedular on time
    initClk();
    generateProcesses(processes,P_N,msgq_id); 
    destroyClk(true);
}

void clearResources(int signum)
{
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *) 0);
    for (int i = 0; i < P_N; i++)
    {
        free(processes[i]); 
    }
    //delete dynamic memeory 
    //TODO Clears all resources in case of interruption
}