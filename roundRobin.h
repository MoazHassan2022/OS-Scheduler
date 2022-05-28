#include "headers.h"


/* Modify this file as needed*/



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

/*
 *
 * this for test
int main(int agrc, char * argv[])
{
    initClk();
    struct processEntry* pr[4];
    for(int i = 0; i<4; i++){
        pr[i] = malloc(sizeof(struct processEntry));
        pr[i]->remainingTime = (i + 1) * 2;
        pr[i]->id = i+1;
    }
    struct queue* currentQueue = newQueue(4);
    int lastClk = getClk();
    int currentClk = lastClk;
    int index = 0;
    enqueue(currentQueue, pr[index]);
    index++;
    while(!isEmpty(currentQueue) || index <= 3){
        currentClk = getClk();
        if(index < 4 && index > 0 && currentClk - 2 >= lastClk){
            enqueue(currentQueue, pr[index]);
            index++;
            lastClk = currentClk;
        }
        runRoundRobin(currentQueue, 3);
    }
    destroyClk(false);

    return 0;
}
*/