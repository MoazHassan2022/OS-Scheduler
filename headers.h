/**
 * @file headers.h
 * @author our Team
 * @brief this file contains all headers that should be used in the project
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include "Bud.h"
#include <sys/time.h>

#define SHKEY 300
typedef unsigned short ushort;

/* arg for semctl system calls. */
union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

// for the semaphore
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

// for the semaphore
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

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}


/**
 * @brief used to wait till any process in the cpu finishes its time 
 * 
 * @param remainingtime 
 */
void waitTillProcessFinishes(int remainingtime){
    //sleep(remainingtime);
    int lastClk = getClk();
    int currentClk = lastClk;
    while (remainingtime > 0)
    {
        currentClk = getClk();
        if(currentClk != lastClk){
            remainingtime -= (currentClk - lastClk);
        }
        lastClk = currentClk;
    }
}

// queue used as a ready queue
struct queue
{
    struct processEntry *items;     // array to store queue elements
    int maxqueueSize;    // maximum capacity of the queue
    int front;      // front points to the front element in the queue (if any)
    int rear;       // rear points to the last element in the queue
    int queueSize;       // current capacity of the queue
};

// queue functions
struct queue* newQueue(int qs)
{
    struct queue *pt = NULL;
    pt = (struct queue*)malloc(sizeof(struct queue));

    pt->items = (struct processEntry*)malloc( qs * sizeof(struct processEntry));
    pt->maxqueueSize = qs;
    pt->front = 0;
    pt->rear = -1;
    pt->queueSize = 0;

    return pt;
}

int sizeOfQueue(struct queue *pt) {
    return pt->queueSize;
}

int isEmpty(struct queue *pt) {
    return !sizeOfQueue(pt);
}

struct processEntry * front(struct queue *pt)
{
    return &pt->items[pt->front];
}

void enqueue(struct queue *pt, struct processEntry* x)
{
    if (sizeOfQueue(pt) == pt->maxqueueSize)
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->rear = (pt->rear + 1) % pt->maxqueueSize;    // circular queue
    pt->items[pt->rear] = *x;
    pt->queueSize++;
}

void dequeue(struct queue *pt)
{
    if (isEmpty(pt))    // front == rear
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->front = (pt->front + 1) % pt->maxqueueSize;  // circular queue
    pt->queueSize--;

}