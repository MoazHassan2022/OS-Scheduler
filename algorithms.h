#include "headers.h"
int HPF(struct priorityQueue Q)
{
    if(isPriorityQueueEmpty(&Q))
        return 0;
    struct processEntry currentProcess = getMax(&Q);

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
        //the output task
        int time  = getClk();
        int arr = currentProcess.arrivalTime;
        int total = currentProcess.runningTime;
        int remain = currentProcess.remainingTime;
        int wait = time - arr - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(remain==total)
        {
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,currentProcess.id,arr,total,remain,wait);
        }
        else{
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,currentProcess.id,arr,total,remain,wait);
        }
        waitTillProcessFinishes(currentProcess.remainingTime);
        time = getClk();
        printf("Process %d finishes at time %d \n", currentProcess.id,getClk());
        fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d \n",time,currentProcess.id,arr,total,0,wait);
        fclose(output);
    }
    return 1;
}


int runRoundRobin(struct queue * currentQueue, int quantum){ // returns if process terminated or not
    /*
     * if process terminated ---> will return 1
     * if process didn't terminated   -->  will return 0
    */
    if(isEmpty(currentQueue))
        return 0;
    struct processEntry* pr = front(currentQueue);
    printf("the process id %d has started and the remaining time %d \n",pr->id,pr->remainingTime);
    int processRemainingTime = pr->remainingTime;
    int remainingTime;
    if(processRemainingTime <= 0){
        //dequeue(currentQueue); // finished
        //this case is theoritcally impossible but actually we will have to dequeue this process from queue so will return 1
        //printf("process %d finishes at time %d \n",pr->id,getClk());
        return 1;
    }
    if(processRemainingTime <= quantum)
        remainingTime = processRemainingTime;
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
        //the output task
        int time  = getClk();
        int arr = pr->arrivalTime;
        int total = pr->runningTime;
        int remain = pr->remainingTime;
        int wait = time - arr - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(remain==total)
        {
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,pr->id,arr,total,remain,wait);
        }
        else{
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,pr->id,arr,total,remain,wait);
        }
        //dequeue(currentQueue);
        waitTillProcessFinishes(remainingTime);
        time = getClk();
        processRemainingTime -= remainingTime;
        //enqueue(currentQueue, pr);
        if(processRemainingTime > 0) {

            fprintf(output,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",time,pr->id,arr,total,processRemainingTime,wait);
            fclose(output);
            return 0;
        }
        else
        {
            fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d \n",time,pr->id,arr,total,processRemainingTime,wait);
            //printf("Process %d finishes at time %d \n", pr->id,getClk());
            fclose(output);
            return 1;
        }

    }
}


int SRTN(struct priorityQueue q){ // returns time spent
    if(isPriorityQueueEmpty(&q))
        return 0;
    struct processEntry pr =  getMax(&q);
    int remainingTime= 0 ;


    // setting pr to the process with the shortest time

    // if the processes has finished remove it from the queue and return 0
    if(pr.remainingTime <= 0){
        extractMax(&q);
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
        pr = extractMax(&q);
        waitTillProcessFinishes(1);
        pr.remainingTime -= 1;
        if(pr.remainingTime > 0)
            insert(pr,&q);
        else
        {
            printf("Process %d finishes at time %d \n", pr.id,getClk());
            return 1 ;
        }
    }
    return 0 ;
}
