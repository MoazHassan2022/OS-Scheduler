#include "headers.h"
void handler(int signum);

// some global variables
int pid;
struct processEntry pr;
struct priorityQueue Q;
int *remainingTimeOfTheCurrentProcess;
int *NOOfProcesses;
int remSem;
FILE *output;

/**
 * @brief Highest priority first algorithm in which we run the process with the highest priority till it finishes its running time then after that we choose another one
 * 
 * @param Q  which is a priority queue containing all the processes from the schedular 
 * @return int which indecates whethere the process has finished or the queue is empty
 */
int HPF(struct priorityQueue Q)
{
    if(isPriorityQueueEmpty(&Q))
        return 0;
    
    // getting the process with the highest priority from the queue
    struct processEntry currentProcess = getMax(&Q);

    int pid = fork(); 
    if (pid == 0)
    {
        // child logic
        char remainingTimePassed[10];

        // converting the int to string
        sprintf(remainingTimePassed, "%d", currentProcess.remainingTime); 
        char *data[] = {"process.out", remainingTimePassed, NULL};
        
        // executing the process file
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        // the parent logic 

        //the output task
        int time  = getClk();
        int arrival = currentProcess.arrivalTime;
        int total = currentProcess.runningTime;
        int remain = currentProcess.remainingTime;
        int wait = time - arrival - (total - remain);

        //openning the log file and start writing into it
        output = fopen("scheduler.log","a");
        if(remain==total)
        {
              
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,currentProcess.id,arrival,total,remain,wait);
        }
        else
        {
              
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,currentProcess.id,arrival,total,remain,wait);
        }

        //wait on the time of the process    
        waitTillProcessFinishes(currentProcess.remainingTime);
        time = getClk();
        int TA  = currentProcess.runningTime+wait;
        float WTA =((float)TA)/currentProcess.runningTime;
          
        fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",time,currentProcess.id,arrival,total,0,wait,TA,WTA);
        fclose(output);
        int *WTAarr;
        int *waitingarr;
        key_t wtaKey = ftok("WTA", 65);
        int shmWtaId = shmget(wtaKey, 4096, IPC_CREAT | 0644);
        WTAarr = shmat(shmWtaId, (int *) 0, 0);
        key_t waitingKey = ftok("waiting", 65);
        int shmWaitingId = shmget(waitingKey, 4096, IPC_CREAT | 0644);
        waitingarr = shmat(shmWaitingId, (int *) 0, 0);
        WTAarr[currentProcess.id]=WTA;
        waitingarr[currentProcess.id]=wait;
    }
    return 1;
}


/**
 * @brief this is the round robin algorithm in which we give each process in the ready queue a certain quantum then we do a context switching to let another process run
 * 
 * @param currentQueue which is a circular queue containing the ready processes
 * @param quantum which is the time for each process to run then we apply the context switching 
 * @return int which indecates whethere we have finished or not
     * if process terminated ---> will return 1
     * if process didn't terminated   -->  will return 0
 */
int runRoundRobin(struct queue * currentQueue, int quantum){ 

    // attaching to the shared memory
    key_t key_id = ftok("remainingtimeKey", 65);
    int shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    remainingTimeOfTheCurrentProcess = shmat(shmid, (int *)0, 0);

    key_id = ftok("keyFile", 65);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    NOOfProcesses = shmat(shmid, (int *) 0, 0);

    if(isEmpty(currentQueue)) {
        return 0;
    }

    // getting the process which run
    struct processEntry* pr = front(currentQueue);
    printf("the process id %d has started and the remaining time %d \n",pr->id,pr->remainingTime);
    int processRemainingTime = pr->remainingTime;
    *remainingTimeOfTheCurrentProcess = processRemainingTime;
    int remainingTime;

    // this should be an impossible case but to avoid any errors 
    if(processRemainingTime <= 0)
        return 1;
    
    // to check if the quantum is less than the remaining time
    if(processRemainingTime <= quantum)
        remainingTime = processRemainingTime;
    else
        remainingTime = quantum;


    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        // child logic 
        char remainingTimePassed[10]; // max int has 10 digits
        sprintf(remainingTimePassed, "%d", remainingTime); // converting the int to string
        char *data[] = {"process.out", remainingTimePassed, NULL};
        
        // executing the process file 
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        // parent logic 

        //the output task
        int time  = getClk();
        int arrival = pr->arrivalTime;
        int total = pr->runningTime;
        int remain = pr->remainingTime;
        int wait = time - arrival - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(remain==total)
        {
              
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,pr->id,arrival,total,remain,wait);
        }
        else
        {
              
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,pr->id,arrival,total,remain,wait);
        }
            
        
        // this loop is made to to ensure that if there is only 1 process in the queue it should keep taking the remaining quantums till it finishes
        while(1){

            if(processRemainingTime <= quantum)
                remainingTime = processRemainingTime;
            else
                remainingTime = quantum;

            waitTillProcessFinishes(remainingTime);
            time = getClk();
            processRemainingTime -= remainingTime;
            *remainingTimeOfTheCurrentProcess = pr->remainingTime=processRemainingTime;
            if((*NOOfProcesses) > 1 || processRemainingTime <= 0){
                break;
            }
        }
        time  = getClk();
        arrival = pr->arrivalTime;
        total = pr->runningTime;
        remain = pr->remainingTime;
        wait = time - arrival - (total - remain);

        // if the process has finished it should be printed in the output file 
        if(processRemainingTime > 0) {
              
            fprintf(output,"At time %d process %d stopped arrival %d total %d remain %d wait %d \n",time,pr->id,arrival,total,processRemainingTime,wait);
            fclose(output);
            return 0;
        }
        else
        {
            int TA  = pr->runningTime+wait;
            float WTA =((float)TA)/pr->runningTime;
            fprintf(output,"At time %d process %d finished arrival %d total %d remain %d wait %d TA %d WTA %f\n",getClk(),pr->id,arrival,total,processRemainingTime,wait,TA,WTA);
            fclose(output);
            int *WTAarr;
            int *waitingarr;
            key_t wtaKey = ftok("WTA", 65);
            int shmWtaId = shmget(wtaKey, 4096, IPC_CREAT | 0644);
            WTAarr = shmat(shmWtaId, (int *) 0, 0);
            key_t waitingKey = ftok("waiting", 65);
            int shmWaitingId = shmget(waitingKey, 4096, IPC_CREAT | 0644);
            waitingarr = shmat(shmWaitingId, (int *) 0, 0);
            WTAarr[pr->id]=WTA;
            waitingarr[pr->id]=wait;
            return 1;
        }

    }
}


/**
 * @brief Shortest remaining time next algorithm in which we should run the process whose remaining time is the least one 
 * 
 * @param q which is the priority queue which act as the ready queue for the processes 
 * @return int which indecates whethere the process has finished its running time or not 
 * 1 if yes 
 * 0 if no
 */
int SRTN(struct priorityQueue q){ // returns time spent

    // attaching to the shared memory
    key_t key_id;
    key_id = ftok("remainingtimeKey", 65);
    int shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    remainingTimeOfTheCurrentProcess = shmat(shmid, (int *)0, 0);

    // creating semaphore
    union Semun semun;
    key_t keyRemSem = ftok("keyfile", 10);
    remSem = semget(keyRemSem, 1, 0666 | IPC_CREAT);
    if (remSem == -1)
    {
        perror("SRTN: Error in creating semaphores");
        exit(-1);
    }
    
     /* initial value of the semaphore, Binary semaphore */
    semun.val = 0;
    if (semctl(remSem, 0, SETVAL, semun) == -1)
    {
        perror("SRTN: Error in semctl");
        exit(-1);
    }

    if(isPriorityQueueEmpty(&q)) 
        return 0;
    
    
    // defining the queue and getting the process which should run
    Q = q;

    // setting pr to the process with the shortest time
    pr =  getMax(&q);
    int remainingTime= 0 ;



    //signal(SIGUSR2,handler2);

    // if the processes has finished remove it from the queue and return 0
    if(pr.remainingTime <= 0){
        extractMax(&q);
        FILE *output = fopen("scheduler.log","a");
          
        fprintf(output, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %s\n", getClk(),
                pr.id, pr.arrivalTime, pr.runningTime, pr.remainingTime, 0, 0,"undefined");
        fclose(output);
        int *WTAarr;
        int *waitingarr;
        key_t wtaKey = ftok("WTA", 65);
        int shmWtaId = shmget(wtaKey, 4096, IPC_CREAT | 0644);
        WTAarr = shmat(shmWtaId, (int *) 0, 0);
        key_t waitingKey = ftok("waiting", 65);
        int shmWaitingId = shmget(waitingKey, 4096, IPC_CREAT | 0644);
        waitingarr = shmat(shmWaitingId, (int *) 0, 0);
        WTAarr[pr.id]=0;
        waitingarr[pr.id]=wait;
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
        // parent work 
        // define an handler for the child to be handled when a new process with running time less than that which is been running 
        //so it should be extracted and the other should enter instead
        signal(SIGUSR1, handler);
        
        //the output task
        int time  = getClk();
        int arrival = pr.arrivalTime;
        int total = pr.runningTime;
        int remain = pr.remainingTime;
        int wait = time - arrival - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(remain==total)
        {
              
            fprintf(output,"At time %d process %d started arr %d total %d remain %d wait %d \n",time,pr.id,arrival,total,remain,wait);
        }
        else
        {
              
            fprintf(output,"At time %d process %d resumed arr %d total %d remain %d wait %d \n",time,pr.id,arrival,total,remain,wait);
        }
        // extracting the process which should be in the CPU
        pr = extractMax(&q);
        //up(remSem);
        while(1) {
            *remainingTimeOfTheCurrentProcess = pr.remainingTime;
            pr.remainingTime -= 1;
            waitTillProcessFinishes(1);
            //------------------------------------------------------------------------
            time = getClk();
            
            // if the process has finished we should print its details in the output file and return 
            if (pr.remainingTime == 0) {
                int TA = pr.runningTime + wait;
                float WTA = ((float) TA) / pr.runningTime;
                  
                fprintf(output, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n", time,
                        pr.id, arrival, total, pr.remainingTime, wait, TA, WTA);
                int *WTAarr;
                int *waitingarr;
                key_t wtaKey = ftok("WTA", 65);
                int shmWtaId = shmget(wtaKey, 4096, IPC_CREAT | 0644);
                WTAarr = shmat(shmWtaId, (int *) 0, 0);
                key_t waitingKey = ftok("waiting", 65);
                int shmWaitingId = shmget(waitingKey, 4096, IPC_CREAT | 0644);
                waitingarr = shmat(shmWaitingId, (int *) 0, 0);
                WTAarr[pr.id]=WTA;
                waitingarr[pr.id]=wait;

                fclose(output);
                return 1;
            }
        }
    }
    return 0 ;
}


/**
 * @brief handler for the signal which is sent from the schedular to the SRTN to inform it that there is new process with
 *  running time less than that of the current running process
 * 
 * @param signum indecates the number of the signal 
 */
void handler(int signum)
{
    if(signum==SIGUSR1) /// eh el condition da ?
    {
        // deleting the semaphore
        // semctl(remSem, 0, IPC_RMID);

        //we should terminate the process.o   --> theoritical done
        int time  = getClk();
        int arrival = pr.arrivalTime;
        int total = pr.runningTime;
        int remain = pr.remainingTime;
        int wait = time - arrival - (total - remain);
        FILE *output = fopen("scheduler.log","a");
        if(pr.remainingTime > 0) {
            fprintf(output,"At time %d process %d stopped arr %d total %d remain %d wait %d \n",time,pr.id,arrival,total,pr.remainingTime ,wait);
            insert(pr, &Q);
            exit(0);
        }
        else
        {
            int TA  = pr.runningTime+wait;
            float WTA =((float)TA)/pr.runningTime;
              
            fprintf(output,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %f\n",time,pr.id,arrival,total,pr.remainingTime ,wait,TA,WTA);
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
