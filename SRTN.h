#include "headers.h"


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

/*
int main(int agrc, char * argv[])
{
    initClk();
    //job of the process generator
    struct processEntry *pr[4];
    for(int i = 0; i<4; i++){
        pr[i] = malloc(sizeof(struct processEntry));
        pr[i]->id = i+1;
    }
    pr[0]->remainingTime=7;
    pr[1]->remainingTime=3;
    pr[2]->remainingTime=1;
    pr[3]->remainingTime=0;
    //end of the job
    initPriorityQueue(4);
    setSorting(0);
    int lastClk = getClk();
    int currentClk = lastClk;
    int index = 0;
    insert(*pr[index]);
    index++;
    while(!isPriorityQueueEmpty() || index <= 3){
        currentClk = getClk();
        if(index < 4 && index > 0 && currentClk - 2 >= lastClk){
            insert(*pr[index]);
            index++;
            lastClk = currentClk;
        }
        SRTN();
    }
    destroyClk(false);

    return 0;
}
*/