#include "headers.h"

int pid;
void handler(int signum);
struct processEntry pr;

struct priorityQueue Q;
int *remainingTimeOfTheCurrentProcess;
int *NOOfProcesses;
int remSem;
FILE *output;

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
        output = fopen("scheduler.log","a");
        if(remain==total)
        {
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,currentProcess.id,arr,total,remain,wait);
        }
        else{
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,currentProcess.id,arr,total,remain,wait);
        }
        waitTillProcessFinishes(currentProcess.remainingTime);
        time = getClk();
        int TA  = currentProcess.runningTime+wait;
        float WTA =((float)TA)/currentProcess.runningTime;
        fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",time,currentProcess.id,arr,total,0,wait,TA,WTA);
        fclose(output);
    }
    return 1;
}


int runRoundRobin(struct queue * currentQueue, int quantum){ // returns if process terminated or not
    /*
     * if process terminated ---> will return 1
     * if process didn't terminated   -->  will return 0
    */

    key_t key_id = ftok("remainingtimeKey", 65);
    int shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    remainingTimeOfTheCurrentProcess = shmat(shmid, (int *)0, 0);
    key_id = ftok("keyFile", 65);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    NOOfProcesses = shmat(shmid, (int *) 0, 0);
    if(isEmpty(currentQueue)) {
        return 0;
    }
    struct processEntry* pr = front(currentQueue);
    printf("the process id %d has started and the remaining time %d \n",pr->id,pr->remainingTime);
    int processRemainingTime = pr->remainingTime;
    *remainingTimeOfTheCurrentProcess = processRemainingTime;
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
        while(1){
            if(processRemainingTime <= quantum)
                remainingTime = processRemainingTime;
            else
                remainingTime = quantum;
            waitTillProcessFinishes(remainingTime);
            time = getClk();
            processRemainingTime -= remainingTime;
            *remainingTimeOfTheCurrentProcess = processRemainingTime;
            if((*NOOfProcesses) > 1 || processRemainingTime <= 0){
                break;
            }
        }
        //enqueue(currentQueue, pr);
        if(processRemainingTime > 0) {

            fprintf(output,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",time,pr->id,arr,total,processRemainingTime,wait);
            fclose(output);
            return 0;
        }
        else
        {
            int TA  = pr->runningTime+wait;
            float WTA =((float)TA)/pr->runningTime;
            fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",getClk(),pr->id,arr,total,processRemainingTime,wait,TA,WTA);
            //printf("Process %d finishes at time %d \n", pr->id,getClk());
            fclose(output);
            return 1;
        }

    }
}


int SRTN(struct priorityQueue q){ // returns time spent
    union Semun semun;

    //a7na leh bnb3t el queue kolha msh el max bs

    key_t key_id;


    key_id = ftok("remainingtimeKey", 65);
    key_t keyRemSem = ftok("remainingtimeKey", 66);
    int shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    remSem = semget(keyRemSem, 1, 0666 | IPC_CREAT);
    if (remSem == -1)
    {
        perror("SRTN: Error in creating semaphores");
        exit(-1);
    }
    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(remSem, 0, SETVAL, semun) == -1)
    {
        perror("SRTN: Error in semctl");
        exit(-1);
    }
    remainingTimeOfTheCurrentProcess = shmat(shmid, (int *)0, 0);

    if(isPriorityQueueEmpty(&q)) {
        return 0;
    }Q = q;
    pr =  getMax(&q);
    int remainingTime= 0 ;



    //signal(SIGUSR2,handler2);
    // setting pr to the process with the shortest time

    // if the processes has finished remove it from the queue and return 0
    if(pr.remainingTime <= 0){
        extractMax(&q);
        FILE *output = fopen("scheduler.log","a");
        fprintf(output, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %s\n", getClk(),
                pr.id, pr.arrivalTime, pr.runningTime, pr.remainingTime, 0, 0,"undefined");
        fclose(output);

        return 1;
    }

    pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        // child work
        char text[20];
        sprintf(text, "%d", pr.remainingTime);
        char *data[] = {"process.out", text, NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
        exit(0);
    }
    else{

        signal(SIGUSR1, handler);
        //the output task13.0
        int time  = getClk();
        int arr = pr.arrivalTime;
        int total = pr.runningTime;
        int remain = pr.remainingTime;
        int wait = time - arr - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(remain==total)
        {
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,pr.id,arr,total,remain,wait);
        }
        else{
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,pr.id,arr,total,remain,wait);
        }
        pr = extractMax(&q);
        up(remSem);
        while(1) {
            //down(remSem);
            *remainingTimeOfTheCurrentProcess = pr.remainingTime;
            printf("remainingTimeOfTheCurrentProcess in SRTN = %d\n", *remainingTimeOfTheCurrentProcess);
            //up(remSem);
            //up(remSem);
            waitTillProcessFinishes(1);
            pr.remainingTime -= 1;
            //------------------------------------------------------------------------
            time = getClk();

            if (pr.remainingTime == 0) {
                int TA = pr.runningTime + wait;
                float WTA = ((float) TA) / pr.runningTime;
                fprintf(output, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n", time,
                        pr.id, arr, total, pr.remainingTime, wait, TA, WTA);
                //printf("Process %d finishes at time %d \n", pr->id,getClk());
                fclose(output);
                return 1;
            }
        }
    }
    return 0 ;
}


void handler(int signum)
{
    if(signum==SIGUSR1)
    {
        semctl(remSem, 0, IPC_RMID);
        //we should terminate the process.o   --> theoritical done

        int time  = getClk();
        int arr = pr.arrivalTime;
        int total = pr.runningTime;
        int remain = pr.remainingTime;
        int wait = time - arr - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(pr.remainingTime > 0) {
    //        fprintf(output,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",time,pr.id,arr,total,pr.remainingTime ,wait);
            insert(pr, &Q);
            exit(0);
        }
        else
        {
            int TA  = pr.runningTime+wait;
            float WTA =((float)TA)/pr.runningTime;
            fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",time,pr.id,arr,total,pr.remainingTime ,wait,TA,WTA);
            //printf("Process %d finishes at time %d \n", pr->id,getClk());
            fclose(output);
            printf("Process %d finishes at time %d \n", pr.id,getClk());
            //signal(SIGUSR2, SIG_IGN);
            //kill(0,SIGUSR2);

            //should be revised


            exit(1);
        }
        signal(SIGUSR1,handler);
    }

}
