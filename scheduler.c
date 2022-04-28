#include "headers.h"

int main(int argc, char *argv[])
{
    // initiating the Clk
    sleep(2); // to make sure that the clock has been initiated in the proccess generator
    initClk();
    printf("Hello From Scheduler\n");
    printf("%s\n", argv[0]); // schaduler algo number
    printf("%s\n", argv[1]); // quantum of RR

    // creating connection between it and the process_generator
    key_t key = ftok("keyFile", 'c');
    int Queue = msgget(key, IPC_CREAT | 0666);
    printf("Queue id is %d\n", Queue);

    // TODO implement the scheduler :)

    // else do the work
    while (1)
    {
        // if recevied do something
        struct process p;
        int rec = msgrcv(Queue, &p, /*4 + 4 + 4 + 4 + 8*/ 100, 0, IPC_NOWAIT);
        if (rec != -1)
        {
            // add the recevied process to the queue
            printf("recevied process id %d , process art %d \n", p.id, p.arvT);
            sleep(1);
        }
        else
        {
            // do something
        }
    }

    // upon termination release the clock resources.
    destroyClk(true);
}
