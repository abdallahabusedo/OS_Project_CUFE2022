#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

void handler(int signum);

struct msgbuf
{
    long mtype;
    char mtext[1];
};

union Semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;
    
    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}


int mqId;
int m;
int bufferId, bufferDataId;
union Semun semun;
key_t sem_key_id;
int main() {

    //creating semaphore
    sem_key_id = ftok("sem_key", 3);
    m = semget(sem_key_id, 1, 0666 | IPC_CREAT);
    if (m == -1) {
        perror("Error in create sem");
        exit(-1);
    }
    signal (SIGINT, handler);
    
    //creating a shared memory for data
    //and another one for info about data
    key_t buffer_key_id, buffer_data_key_id;
    int consumer;
    buffer_key_id = ftok("buffer_memory_key", 1);
    buffer_data_key_id = ftok("buffer_data_key", 2);
    
    int bufferSize;

    bufferDataId = shmget(buffer_data_key_id, 4 * sizeof(int), 0666);
    
    /*
        bufferData[0] ---> buffer size
        bufferData[1] ---> number of items
        bufferData[2] ---> address to add
        bufferData[3] ---> address to remove
    */
    int *bufferData;

    if(bufferDataId == -1){
        //if there is no buffer
        shmctl(bufferDataId, IPC_RMID, (struct shmid_ds *)0);
        printf("No buffer available\n");
        exit(-1);
    } else {
        bufferDataId = shmget(buffer_data_key_id, 4 * sizeof(int), IPC_CREAT | 0666);
        bufferData = shmat(bufferDataId, (void *)0, 0);
        if (bufferData == (int *)-1)
        {
            perror("Error in attach in consumer");
            exit(-1);
        }

        bufferSize = bufferData[0];
    }

    bufferId = shmget(buffer_key_id, bufferSize * sizeof(int), IPC_CREAT | 0666);
    if (bufferId == -1) {
        perror("Error in reading buffer");
        exit(-1);
    }

    int *buffer = shmat(bufferId, (void *)0, 0);

    key_t message_key_id;
    message_key_id = ftok("message_queue_key", 4);
    mqId = msgget(message_key_id, 0666 | IPC_CREAT);
    if (mqId == -1) {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuf message;
    int rec_val, send_val;

    while(1){  
        down(m);
        // to empty message buffer
        rec_val = msgrcv(mqId, &message, sizeof(message.mtext), 20, IPC_NOWAIT);

        if (bufferData[3] < 0) /* overflow */
            exit(1);	
        if(bufferData[1] == 0) { //empty
            up(m);
            //waiting for messege from producer
            rec_val = msgrcv(mqId, &message, sizeof(message.mtext), 20, !IPC_NOWAIT);
            if (rec_val == -1)
                perror("Error in receive");

            down(m);
        }
        
        consumer = buffer[bufferData[3]];
        printf("consumed item %d at index %d\n", buffer[bufferData[3]], bufferData[3]);

        bufferData[3] = (bufferData[3]+1) % bufferSize;
        bufferData[1]--;
        if(bufferData[1] == bufferSize-1){
            //awake producer
            message.mtype = 10;
            send_val = msgsnd(mqId, &message, sizeof(message.mtext), !IPC_NOWAIT);
            if (send_val == -1)
                perror("Error in send");
        }
        up(m);
    }
}
void handler(int signum)
{
    msgctl(mqId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(bufferId, IPC_RMID, (struct shmid_ds *)0);
    shmctl(bufferDataId, IPC_RMID, (struct shmid_ds *)0);
    semctl(m, 0, IPC_RMID);
    kill(getpid(), SIGKILL);
}/**/