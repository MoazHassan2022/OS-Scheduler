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

    pr[0]->priority=3;
    pr[1]->priority=1;
    pr[2]->priority=2;
    pr[3]->priority=4;
    //end of the job
    initPriorityQueue(4);
    setSorting(1);
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

        if(!isPriorityQueueEmpty())
        {
            HPF();
            extractMax();
        }
    }
    destroyClk(false);

    return 0;
}
*/