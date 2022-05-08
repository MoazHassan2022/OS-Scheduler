#include "algorithms.h"
//this include contains the headers file


int main(int argc, char *argv[])
{
    //sleep(2);
    int algorithm = 0;
    int Q = 0;
    int numberOfProcesses = 0;
    int programCounter = 0;
    int lastProcessId = -1;
    // initiating the Clk
    initClk();
    printf("Hello From Scheduler\n");

    printf("%s\n", argv[0]); // schaduler algo number
    printf("%s\n", argv[1]); // quantum of RR
    printf("%s\n", argv[2]); // quantum of RR

    algorithm = atoi(argv[0]);
    Q = atoi(argv[1]);
    numberOfProcesses = atoi(argv[2])+1;

    //open the output file
    FILE* outputFile = fopen("scheduler.log","w");
    fprintf(outputFile,"# At time x process y state arr w total z remain j wait k\n");
    // creating connection between it and the process_generator
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
   if(algorithm != 3) {
       initPriorityQueue(numberOfProcesses,&theQueue);
       (algorithm==1)?setSorting(1,&theQueue): setSorting(0,&theQueue);
   }
   else {
       processTableForRR = newQueue(numberOfProcesses);
   }

    // else do the work
    while (1)
    {
        //we will fork here for algorithm
        int pid = fork();
        if(pid == 0)
        {
            int status;
            //we are in the child
            //we are in the process assigned for algorithm
            switch (algorithm) {
                case 1:
                    status = HPF(theQueue);
                    exit(status);
                    break;
                case 2:
                    status = SRTN(theQueue);
                    exit(status);
                    break;
                case 3:
                    status = runRoundRobin(processTableForRR,Q);
                    exit(status);
                    break;
                default:
                    break;
            }
        }
        else
        {
            struct processEntry ptable[numberOfProcesses];
            int i=0;
            //we are in the parent process
            while (1) {
                struct processEntry p;
                int rec = msgrcv(Queue, &p, sizeof(p), 0, IPC_NOWAIT);
                if (rec != -1) {
                    printf("recevied process id %d , process remaining time %d \n", p.id, p.remainingTime);
                    // add the recevied process to the queue
                    ptable[i] = p;
                    i++;
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
                    if(WEXITSTATUS(chldstate)) {
                        switch (algorithm) {
                            case 3:
                                pr= *front(processTableForRR);
                                printf("process %d finishes at time %d \n",pr.id,getClk());
                                dequeue(processTableForRR);
                                break;
                            default:
                                extractMax(&theQueue); // this statement was above in case 2 and case 1 but we put in default
                                break;
                        }
                        //this statement is general for ending process (the process of the algorithm exited by 1)
                        programCounter++;
                        if(programCounter == numberOfProcesses)
                            destroyClk(true);
                    }
                    else
                    {
                        struct processEntry temp;
                        switch(algorithm)
                        {
                            case 2:
                                if(!isPriorityQueueEmpty(&theQueue)) {
                                    temp = extractMax(&theQueue);
                                    temp.remainingTime -= 1;
                                    insert(temp, &theQueue);
                                }
                                break;
                            case 3:
                                if(!isEmpty(processTableForRR)) {
                                    temp = *front(processTableForRR);
                                    dequeue(processTableForRR);
                                    temp.remainingTime -= Q;
                                    enqueue(processTableForRR,&temp);
                                }
                                break;
                        }
                    }
                    if(algorithm!=3){
                        for(int j=0;j<i;j++)
                        {
                            insert(ptable[j],&theQueue);
                        }
                    }
                    else
                    {
                        for(int j=0;j<i;j++)
                        {
                            enqueue(processTableForRR,&ptable[j]);
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
    fclose(outputFile);
    destroyClk(true);
}
