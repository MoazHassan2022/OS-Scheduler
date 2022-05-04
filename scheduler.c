#include "priority_queue.h"
#include "queue.h"
#include "roundRobinTest.c"
//this include contains the headers file


int main(int argc, char *argv[])
{
    int algorithm = 0;
    int Q = 0;
    int numberOfProcesses = 0;
    // initiating the Clk
    sleep(2); // to make sure that the clock has been initiated in the proccess generator
    initClk();
    printf("Hello From Scheduler\n");

    printf("%s\n", argv[0]); // schaduler algo number
    printf("%s\n", argv[1]); // quantum of RR

    algorithm = atoi(argv[0]);
    Q = atoi(argv[1]);
   
    

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

   struct priorityQueue processTableForHPF;
   struct priorityQueue processTableForSRTN;
   struct queue processTableForRR;





  

    // else do the work
    while (1)
    {
        //we will fork here for algorithm 
        int pid = fork();
        if(pid == 0)
        {
            //we are in the child
            //we are in the process assigned for algorithm
            switch (algorithm) {
                case 1:
                    //HPF
                    break;
                case 2:
                    // SRTN
                    break;
                case 3:
                    //RR
                    exit(runRoundRobin(&processTableForRR,Q));
                    break;
                default:
                    break;
            }
            exit(1);
        }
        else
        {
            //we are in the parent process
            while (1) {
                struct process p;
                int rec = msgrcv(Queue, &p, /*4 + 4 + 4 + 4 + 8*/ 100, 0, IPC_NOWAIT);
                if (rec != -1) {

                    printf("recevied process id %d , process art %d \n", p.id, p.arvT);
                    // add the recevied process to the queue
                    switch (algorithm) {
                        case 1:
                            push(p, -p.Prty, &processTableForHPF);
                            break;
                        case 2:
                            push(p, -p.Prty, &processTableForSRTN);
                            break;
                        case 3:
                            enqueue(&processTableForRR, /*should pass process block bas ana msh 3aref :_( */p);
                            break;
                        default:
                            break;
                    }
                    // [nabil] : why sleep
                    sleep(1);
                } else {
                    // do something
                    // [nabil] : if not receive something I will not do any thing
                }
                //check if the child process exited
                int *chldstate;
                int stateOfChld = waitpid(pid, chldstate, WNOHANG);
                if (stateOfChld > 0)
                {
                    if(WEXITSTATUS(chldstate))
                        switch (algorithm) {
                        case 1 :
                            break;
                        case 2:
                            break;
                        case 3:
                            dequeue(&processTableForRR);
                            break;
                        default:
                            break;
                        }
                    break; //break the inner loop to call the algorithms again
                }
            }
        }
        // if recevied do something

    }

    // upon termination release the clock resources.
    destroyClk(true);
}
