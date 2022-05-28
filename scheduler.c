#include "algorithms.h"
//this include contains the headers file

void terminateScheduler(int*finishTime, int numberofProcesses);
FILE *MemoryLog;
int main(int argc, char *argv[])
{
    //sleep(2);
    int algorithm = 0;
    int Q = 0;
    int numberOfProcesses = 0;
    int programCounter = 0;
    int lastProcessId = -1;
    Buddy();
    // initiating the Clk
    initClk();
    printf("Hello From Scheduler\n");

    printf("%s\n", argv[0]); // schaduler algo number
    printf("%s\n", argv[1]); // quantum of RR
    printf("%s\n", argv[2]); // quantum of RR

    algorithm = atoi(argv[0]);
    Q = atoi(argv[1]);
    numberOfProcesses = atoi(argv[2]);

    //open the output file
//    FILE* outputFile = fopen("scheduler.log","w");
//    fprintf(outputFile,"# At time x process y state arr w total z remain j wait k\n");
//    // creating connection between it and the process_generator
    key_t key = ftok("keyFile", 'c');
    int Queue = msgget(key, IPC_CREAT | 0666);
    printf("Queue id is %d\n", Queue);

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

    struct priorityQueue theQueue; //this for HPF and SRTN
    struct queue *processTableForRR;
    struct queue *hardDisk = newQueue(numberOfProcesses);
    if(algorithm != 3) {
       initPriorityQueue(numberOfProcesses,&theQueue);
       (algorithm==1)?setSorting(1,&theQueue): setSorting(0,&theQueue);
   }
   else {
       processTableForRR = newQueue(numberOfProcesses);
   }

    key_t key_id;
    union Semun semun;


    key_id = ftok("keyfile", 65);
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
    semctl(sem1, 0, SETVAL, semun);
    MemoryLog = fopen("memory.log", "w");
    //fprintf(MemoryLog, "#at time x allocated y bytes for Processs z from i to j\n");

    // else do the work
    while (1)
    {
        //we will fork here for algorithm
        int pid = fork();
        if(pid == 0)
        {
            int status = 0;
            //we are in the child
            //we are in the process assigned for algorithm
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
            union Semun semun;
            key_t keyRemSem = ftok("remainingtimeKey", 66);
            int remSem = semget(keyRemSem, 1, 0666 | IPC_CREAT);
            if (remSem == -1)
            {
                perror("Scheduler: Error in creating semaphore");
                exit(-1);
            }
            semun.val = 0; /* initial value of the semaphore, Binary semaphore */
            if (semctl(remSem, 0, SETVAL, semun) == -1)
            {
                perror("Scheduler: Error in semctl");
                exit(-1);
            }
            up(remSem);
            struct processEntry ptable[numberOfProcesses];
            int i=0;
            //we are in the parent process



            /*
            this for test only
            */
            int* finishTime = malloc (sizeof(int)*numberOfProcesses);

            while (1)
            {
                struct processEntry p;
                int rec = msgrcv(Queue, &p, sizeof(p), 0, IPC_NOWAIT);
                if (rec != -1)
                {
                    ///(*NOOfProcesses)++;
                    printf("recevied process id %d , process remaining time %d, process memory size %d \n", p.id, p.remainingTime, p.memSize);
                    // add the recevied process to the queue
                    ptable[i] = p;
                    ptable[i].isAllocated = 0;
                    i=i+1;
                    if(algorithm == 3)
                    {
                        (*NOOfProcesses)++;
                    }
                    if(algorithm == 2) {
                        //down(remSem);
                        int prt1 = *remainingTimeOfTheCurrentProcess;
                        printf("remainingTimeOfTheCurrentProcess in scheduler = %d\n", prt1);
                        //up(remSem);
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
                    struct processEntry pr;
                    if(WEXITSTATUS(chldstate))
                    {
                        if(algorithm == 3)
                            (*NOOfProcesses)--;
                        switch (algorithm) {
                            case 3:
                                pr= *front(processTableForRR);
                                printf("process %d finishes at time %d \n",pr.id,getClk());
                                dequeue(processTableForRR);
                                deallocate(pr.prm);
                                fprintf(MemoryLog, "at time %d deallocated %d bytes for process %d from %d to %d\n", getClk(), pr.memSize, pr.id, pr.prm->Process_start_location, pr.prm->Process_end_location);
                                finishTime[pr.id - 1] = getClk();
                                break;
                            default:
                                pr = extractMax(&theQueue); // this statement was above in case 2 and case 1 but we put in default
                                printf("Process %d finishes at time %d \n", pr.id, getClk());
                                deallocate(pr.prm);
                                fprintf(MemoryLog, "at time %d deallocated %d bytes for process %d from %d to %d\n", getClk(), pr.memSize, pr.id, pr.prm->Process_start_location, pr.prm->Process_end_location);
                                finishTime[pr.id-1] = getClk();
                                break;
                        }
                        while(!isEmpty(hardDisk)) {
                            pr = *front(hardDisk);
                            // insert into memory : bool result = allocate(pr.memSize);
                            struct process_memory result = allocate(pr.memSize);
                            pr.prm = malloc(sizeof(process_memory));
                            pr.prm->Process_start_location = result.Process_start_location;
                            pr.prm->Process_end_location = result.Process_end_location;
                            if(result.Process_start_location != -1){
                                fprintf(MemoryLog, "at time %d allocated %d bytes for process %d from %d to %d\n", getClk(), pr.memSize, pr.id, result.Process_start_location, result.Process_end_location);
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
                            }
                            else
                                break;
                        }
                        //this statement is general for ending process (the process of the algorithm exited by 1)
                        programCounter++;
                        if(programCounter == numberOfProcesses) {
                            terminateScheduler(finishTime, numberOfProcesses);
                        }   //destroyClk(true);
                    }
                    else
                    {
                        struct processEntry temp;
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
                    {
                        if(ptable[j].isAllocated != 1){
                            enqueue(hardDisk,&ptable[j]);
                        }
                    }
                    while(!isEmpty(hardDisk)) {
                        pr = *front(hardDisk);
                        // insert into memory : bool result = allocate(pr.memSize);
                        struct process_memory result = allocate(pr.memSize);
                        pr.prm = malloc(sizeof(process_memory));
                        pr.prm->Process_start_location = result.Process_start_location;
                        pr.prm->Process_end_location = result.Process_end_location;
                        /*printf("process_start_location %d \n",pr.prm->Process_start_location);
                        printf("process_end_location %d \n",pr.prm->Process_end_location);*/
                        if(result.Process_start_location != -1){
                            fprintf(MemoryLog, "at time %d allocated %d bytes for process %d from %d to %d\n", getClk(), pr.memSize, pr.id, result.Process_start_location, result.Process_end_location);
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
                        }
                        else
                            break;
                    }
                    for(int j  = 0; j < i; j++){
                            if(ptable[j].isAllocated == 1){
                                switch (algorithm) {
                                case 3:
                                    enqueue(processTableForRR,&ptable[j]);
                                    break;
                                default:
                                    insert(ptable[j],&theQueue);
                                    break;
                              }
                          }
                    }
                    break; //break the inner loop to call the algorithms again
                }
            }
        }
        // if recevied do something
    }
    printf("I broke the outer loop \n");
    // upon termination release the clock resources.
//    fclose(outputFile);
    destroyClk(true);
}


void terminateScheduler(int finishTime[],int numberofProcesses)
{
    fclose(MemoryLog);
    //before destroy clock will out put the result to file to test it
    FILE * testFile = fopen("test","w");
    for(int i = 0;i<numberofProcesses;i++)
    {
        fprintf(testFile,"%d \n",finishTime[i]);
    }
    fclose(testFile);
    destroyClk(true);
}