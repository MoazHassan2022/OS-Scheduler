#include "headers.h"
void HPF(/*struct processEntry currentprocess*/)
{
    struct processEntry currentProcess = getMax();
    printf("process id %d has started and its remaining time is %d \n",currentProcess.id,currentProcess.remainingTime);
    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        char remainingTimePassed[10];
        sprintf(remainingTimePassed, "%d", currentProcess.remainingTime); // converting the int to string
        char *data[] = {"process.out", remainingTimePassed, NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        waitTillProcessFinishes(currentProcess.remainingTime);
        printf("Process %d finishes...\n", currentProcess.id);
    }
}


int runRoundRobin(struct queue * currentQueue, int quantum){ // returns if process terminated or not
    /*
     * if process terminated ---> will return 1
     * if process didn't terminated   -->  will return 0
    */
    if(isEmpty(currentQueue))
        return 0;
    struct processEntry* pr = front(currentQueue);
    int remainingTime;
    if(pr->remainingTime <= 0){
        dequeue(currentQueue); // finished
        //this case is theoritcally impossible but actually we will have to dequeue this process from queue so will return 1
        return 1;
    }
    if(pr->remainingTime <= quantum)
        remainingTime = pr->remainingTime;
    else
        remainingTime = quantum;
    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        char remainingTimePassed[10]; // max int has 10 digits
        sprintf(remainingTimePassed, "%d", remainingTime); // converting the int to string
        char *data[] = {"process.out", remainingTimePassed, NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        dequeue(currentQueue);
        waitTillProcessFinishes(pr->remainingTime);
        pr->remainingTime -= remainingTime;
        if(pr->remainingTime > 0) {
            enqueue(currentQueue, pr);
            return 0;
        }
        else
        {
            //printf("Process %d finishes...", pr->id);
            return 1;
        }
    }
}


int SRTN(){ // returns time spent
    struct processEntry pr =  getMax();
    printf("process id %d has started and its remaining time is %d \n",pr.id,pr.remainingTime);
    int remainingTime= 0 ;

    if(isPriorityQueueEmpty())
        return 0;

    // setting pr to the process with the shortest time

    // if the processes has finished remove it from the queue and return 0
    if(pr.remainingTime <= 0){
        extractMax();
        return 1;
    }

    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        // child work
        char *data[] = {"process.out", "1", NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
        exit(0);
    }
    else{
        extractMax();
        waitTillProcessFinishes(1);
        pr.remainingTime -= 1;
        if(pr.remainingTime > 0)
            insert(pr);
        else
        {
            printf("Process %d finishes...", pr.id);
            return 1 ;
        }
    }
    return 0 ;
}
