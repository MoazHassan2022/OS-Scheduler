/**
 * @file scheduler.c
 * @author Mohammed Nabil
 * @brief this is the schedular which should organize and manages the execution process of all the processes and send them to the algorithms
 * at the appropriate time
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#define _POSIX_SOURCE
#include "algorithms.h"


bool printCounter = 0;
int stop, start;
struct processEntry *processTableForEstUtilization;
/**
 * @brief this is the handler which terminate house keeping all the resources 
 * 
 * @param finishTime of the program
 * @param numberofProcesses 
 */
void terminateScheduler(int*finishTime, int numberofProcesses);
int idleTime(int numberOfProcesses);
int main(int argc, char *argv[])
{
    int receivedProcesses = 0;
   // gettimeofday(&start, NULL);
    //sleep(1) ;
    
    // Defining some data that will be used later
    int algorithm = 0;
    int Q = 0;
    int numberOfProcesses = 0;
    int programCounter = 0;
    int lastProcessId = -1;

    if(printCounter == 0){
        Buddy();
        printCounter = 1;
    }

    // initiating the Clk
    initClk();

    start = getClk();
    // reading the data sent by the process generator
    algorithm = atoi(argv[0]);
    Q = atoi(argv[1]);
    numberOfProcesses = atoi(argv[2]);
    processTableForEstUtilization = malloc (sizeof(processEntry) * numberOfProcesses);

    //open the output file
    FILE* outputFile = fopen("scheduler.log","w");
    fprintf(outputFile,"# At time x process y state arr w total z remain j wait k\n");
    fclose(outputFile);

    // creating connection between it and the process_generator
    key_t key = ftok("keyFile", 'c');
    int Queue = msgget(key, IPC_CREAT | 0666);
    //printf("Queue id is %d\n", Queue);




    // TODO implement the scheduler :)

    /*
    * the schedular will treat with each algotithm by different way
    * * RR -> will define a circular queue
    * * HPF -> will define a priority queue
    * * SRTN -> will define a priority queue
    * * RR -> will block quantum
    * * HPF -> will block until current process terminate
    * * SRTN -> will block 1 clock edge
    *  VERY IMPORTANT NOTE : THE SCHEDULAR SHOULD WORK WHILE ALGORITHM IS BLOCKING TO RECEIVE NEW PROCESSES IN QUEUES
    *  TO MAKE THIS I SUGGEST WE WILL DEFINE FORK FOR ALGORITHM AND DEFINE SHARED MEMORY BETWEEN ALGORITHM(CHILD) AND THE SCHEDULAR(PARENT)
    *  WILL CHECK EVERY ITERATION IF THE ALGORITHM EXITED OR NOT TO CALL IT AGAIN WITH THE NEW PROCESS TABLE
    */
    /*
    * will define the queues that will filled by the schedular
    */

    //this for HPF and SRTN
    struct priorityQueue theQueue; 
    
    // this for the RR
    struct queue *processTableForRR;
    
    // for two level scheduling 
    struct queue *hardDisk = newQueue(numberOfProcesses);

    // if the algorithm is not the roundrobin we should initiate the priority queue
    if(algorithm != 3) {
       initPriorityQueue(numberOfProcesses,&theQueue);
       (algorithm==1)?setSorting(1,&theQueue): setSorting(0,&theQueue);
   }
   else // initiate the queue 
       processTableForRR = newQueue(numberOfProcesses);
   

    // creating semaphores
    key_t key_id;
    union Semun semun;
    key_id = ftok("keyfile", 10); // this 10 was 65
    int sem1 = semget(key_id, 1, 0666 | IPC_CREAT);

    int *NOOfProcesses;
    int *remainingTimeOfTheCurrentProcess;
    int shmid;
    if(algorithm !=1) {
        /* this shm to store the remaining time of the current process */
        key_id = ftok("remainingtimeKey", 65);
        shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
        remainingTimeOfTheCurrentProcess = shmat(shmid, (int *) 0, 0);
        *remainingTimeOfTheCurrentProcess = 9999;
        key_id = ftok("keyFile", 65);
        shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
        NOOfProcesses = shmat(shmid, (int *) 0, 0);
        *NOOfProcesses = 0;
    }

    semun.val = 0;
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("SRTN: Error in semctl");
        exit(-1);
    }

    // all the logic is here in this loop
    while (1)
    {
        //we will fork here for algorithm
        int pid = fork();
        if(pid == 0)
        {
            //the child is the process which is responsible to call the algorithms 
            int status = 0;
            switch (algorithm) {
                case 1:
                    status = HPF(theQueue);
                    exit(status);
                    break;
                case 2:
                  //  if((*remainingTimeOfTheCurrentProcess)!=9999) {
                        status = SRTN(theQueue);
                        exit(status);
                    //}
                    break;
                case 3:
                    if((*NOOfProcesses)!=0) {
                        status = runRoundRobin(processTableForRR, Q);
                        exit(status);
                    }
                    exit(status);
                    break;
                default:
                    break;
            }
        }
        else
        {
            // the parent will do the remaining logic

            // all of this is made once in the begining of the schedular why we repeat? 

            // union Semun semun;
            // key_t keyRemSem = ftok("remainingtimeKey", 66);
            // int remSem = semget(keyRemSem, 1, 0666 | IPC_CREAT);
            // if (remSem == -1)
            // {
            //     perror("Scheduler: Error in creating semaphore");
            //     exit(-1);
            // }
            // semun.val = 0; /* initial value of the semaphore, Binary semaphore */
            // if (semctl(remSem, 0, SETVAL, semun) == -1)
            // {
            //     perror("Scheduler: Error in semctl");
            //     exit(-1);
            // }

            // this should be defined dynamicly not statically 
            struct processEntry *ptable = malloc (sizeof(processEntry) * numberOfProcesses);

            int i=0;
            int* finishTime = malloc (sizeof(int)*numberOfProcesses);

            // iterating by the parent 
            while (1)
            {
                // try to receive from the generator any sent process 
                struct processEntry p;
                int rec = msgrcv(Queue, &p, sizeof(p), 0, IPC_NOWAIT); // we shouldn't wait
                
                // if we received a new process 
                if (rec != -1)
                {
                    // add the recevied process to the queue
                    ptable[i] = p;
                    processTableForEstUtilization[receivedProcesses].runningTime = p.runningTime;
                    processTableForEstUtilization[receivedProcesses].arrivalTime = p.arrivalTime;
                    processTableForEstUtilization[receivedProcesses].id = p.id;
                    printf("the received running time%d\n",processTableForEstUtilization[receivedProcesses].runningTime);


                    receivedProcesses++;
                    ptable[i].isAllocated = 0;
                    i=i+1;

                    // for RR
                    if(algorithm == 3)
                        (*NOOfProcesses)++;
                    
                    // for SRTN
                    if(algorithm == 2) {
                        int prt1 = *remainingTimeOfTheCurrentProcess;
                        // if the new process has remaining time less than that in the CPU we should inform the algorithms that it should break 
                        if (p.remainingTime < prt1 && !isPriorityQueueEmpty(&theQueue) && prt1 != 9999) {
                            printf("I raised a SIGUSR1 \n");
                            kill(pid, SIGUSR1);
                        }
                    }
                }

                //check if the child process exited
                int chldstate = 0;

                //Return value of waitpid()
                //    pid of child, if child has exited
                //    0, if using WNOHANG and child hasn’t exited.

                int stateOfChld = waitpid(-1, &chldstate, WNOHANG);
                
                if (stateOfChld > 0)
                {
                    // if the child process has exited
                    struct processEntry pr;
                    if(WEXITSTATUS(chldstate))
                    { 
                        // if it exit with 1 
                        if(algorithm == 3) // decrement the number of processes
                            (*NOOfProcesses)--;

                        // then depending on the chosen algo we should extract the process from the ready queue and terminate it
                        switch (algorithm) {
                            case 3:
                                pr= *front(processTableForRR);
                                printf("process %d finishes at time %d \n",pr.id,getClk());
                                dequeue(processTableForRR);
                                deallocate(pr.prm, pr.id, getClk());
                                finishTime[pr.id - 1] = getClk();
                                break;
                            default:
                                pr = extractMax(&theQueue); // this statement was above in case 2 and case 1 but we put in default
                                printf("Process %d finishes at time %d \n", pr.id, getClk());
                                deallocate(pr.prm, pr.id, getClk());
                                finishTime[pr.id-1] = getClk();
                                break;
                        }

                        // and we should get the processes from the hard disk to the Ram
                        while(!isEmpty(hardDisk)) {
                            pr = *front(hardDisk);
                            // insert into memory : bool result = allocate(pr.memSize);
                            struct process_memory result = allocate(pr.memSize, pr.id, getClk());
                            pr.prm = malloc(sizeof(process_memory));
                            pr.prm->Process_start_location = result.Process_start_location;
                            pr.prm->Process_end_location = result.Process_end_location;
                            if(result.Process_start_location != -1){
                                pr.isAllocated = 1;
                                dequeue(hardDisk);
                                switch (algorithm) {
                                    case 3:
                                        enqueue(processTableForRR,&pr);
                                        break;
                                    default:
                                        insert(pr,&theQueue);
                                        break;
                                }
                               // free(pr.prm); 
                            }
                            else
                            {
                                //free(pr.prm); 
                                break;
                            }
                                
                        }

                        //this statement is general for ending process (the process of the algorithm exited by 1)
                        programCounter++;
                        
                        // if we has finished all the processes we should terminate
                        if(programCounter == numberOfProcesses)
                        {
                            terminateScheduler(finishTime, numberOfProcesses);
                        }
                    }
                    
                    else
                     // then there is a context switching 
                    {
                        struct processEntry temp;
                        // so we should only extract the process and decrement its remaining time only
                        switch(algorithm)
                        {
                            case 2:
                                if(!isPriorityQueueEmpty(&theQueue)) {

                                    temp = extractMax(&theQueue);
                                    if(*remainingTimeOfTheCurrentProcess!=9999)
                                        temp.remainingTime = *remainingTimeOfTheCurrentProcess;
                                    ptable[i] = temp;
                                    i++;
                                }
                                break;
                            case 3:
                                if(!isEmpty(processTableForRR)) {
                                    temp = *front(processTableForRR);
                                    dequeue(processTableForRR);
                                    temp.remainingTime = *remainingTimeOfTheCurrentProcess;
                                    ptable[i] = temp;
                                    i++;
                                }
                                break;
                        }
                    }

                    int continueIndex = 0;
                    for(int j=0;j<i;j++)
                        if(ptable[j].isAllocated != 1)
                            enqueue(hardDisk,&ptable[j]);
                        
                    
                    // inserting processes from the harddisk to the ram 
                    while(!isEmpty(hardDisk)) {
                        pr = *front(hardDisk);
                        // insert into memory : bool result = allocate(pr.memSize);
                        struct process_memory result = allocate(pr.memSize, pr.id, getClk());
                        pr.prm = malloc(sizeof(process_memory));
                        pr.prm->Process_start_location = result.Process_start_location;
                        pr.prm->Process_end_location = result.Process_end_location;
                        /*printf("process_start_location %d \n",pr.prm->Process_start_location);
                        printf("process_end_location %d \n",pr.prm->Process_end_location);*/
                        if(result.Process_start_location != -1){
                            pr.isAllocated = 1;
                            dequeue(hardDisk);
                            switch (algorithm) {
                                case 3:
                                    enqueue(processTableForRR,&pr);
                                    break;
                                default:
                                    insert(pr,&theQueue);
                                    break;
                            }
                           // free(pr.prm) ; 
                        }
                        else
                        {
                           // free(pr.prm) ; 
                            break;
                        }
                    }

                    // here we add all the new processes in their appropriate queue
                    for(int j  = 0; j < i; j++)
                         if(ptable[j].isAllocated == 1)
                            switch (algorithm) 
                            {
                                case 3:
                                    enqueue(processTableForRR,&ptable[j]);
                                    break;
                                default:
                                    insert(ptable[j],&theQueue);
                                    break;
                            }
                          
                    free (ptable); 
                    free (finishTime);
                    break; //break the inner loop to call the algorithms again
                }
         //should be here       
        //    free (ptable); 
        //    free (finishTime);
        //    
         }
        }
    }

    // upon termination release the clock resources.
    destroyClk(true);
}


void terminateScheduler(int finishTime[],int numberofProcesses)
{
    //gettimeofday(&stop, NULL);
    //u_int64_t realTime = stop.tv_sec - start.tv_sec;
    stop = getClk();
    int realTime = stop-start-1;
    int idleTimeOfProcessor = idleTime(numberofProcesses);
    int sumOfRunningTime = 0;
    for (int i = 0; i < numberofProcesses; ++i) {
        printf("runningTime = %d\n",processTableForEstUtilization[i].runningTime);
        sumOfRunningTime += processTableForEstUtilization[i].runningTime;
    }
    printf("real time = %d \nidleTimeOfprocessor %d\n sumofRunningTime = %d\n",realTime,idleTimeOfProcessor,sumOfRunningTime);
    float utilization =(1-( (realTime - idleTimeOfProcessor-sumOfRunningTime)/(float)sumOfRunningTime))*100;
    //the code of utilization
    int *WTAarr;
    int *waitingarr;
    key_t key_id = ftok("WTA", 65);
    int shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    WTAarr = shmat(shmid, (int *) 0, 0);
    key_id = ftok("waiting", 65);
    shmid = shmget(key_id, 4096, IPC_CREAT | 0644);
    waitingarr = shmat(shmid, (int *) 0, 0);
    //code for calculating the average and the std of the WTA
    float sum = 0.0, meanOfWTA, SDOfWTA = 0.0;
    for (int i = 1; i < numberofProcesses+1; ++i) {
        sum += WTAarr[i];
    }
    meanOfWTA = sum / numberofProcesses;
    for (int i = 1; i < numberofProcesses+1; ++i) {
        SDOfWTA += pow(WTAarr[i] - meanOfWTA, 2);
    }
    SDOfWTA = sqrt(SDOfWTA / 10);
    //code for calculating the average of the waiting
    sum = 0.0;
    float meanOfWaiting;

    for (int i = 1; i < numberofProcesses+1; ++i) {
        sum += waitingarr[i];
    }
    meanOfWaiting = sum / numberofProcesses;
    //printing the information
    FILE *utilizationFile = fopen("scheduler.pref","w");
    fprintf(utilizationFile,"CPU utilization = %.6f\nAvg WTA = %.6f\nAvg Waiting = %.6f\nStd WTA = %.6f\n",utilization,meanOfWTA,meanOfWaiting,SDOfWTA);
    fclose(utilizationFile);
    //before destroy clock will out put the result to file to test it
    FILE * testFile = fopen("test","w");
    for(int i = 0;i<numberofProcesses;i++)
        fprintf(testFile,"%d \n",finishTime[i]);
    fclose(testFile);
    destroyClk(true);
}
int idleTime(int numberOfProcesses)
{
    int idleTimeofProcessor = 0;
    int currentRunningTime = 1;
    for(int i=0; i<numberOfProcesses;i++)
    {
        if(processTableForEstUtilization[i].arrivalTime>currentRunningTime)
        {
            idleTimeofProcessor += processTableForEstUtilization[i].arrivalTime - currentRunningTime;
            currentRunningTime = processTableForEstUtilization[i].arrivalTime+processTableForEstUtilization[i].runningTime;
        }
        else
        {
           currentRunningTime += processTableForEstUtilization[i].runningTime;
        }
    }
    return idleTimeofProcessor;
}