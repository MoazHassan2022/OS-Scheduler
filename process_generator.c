#include "headers.h"

void clearResources(int singnum);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);

    //creating connection with scheduler
    key_t key = ftok("keyFile" , 'c') ;
    int Queue = msgget(key , IPC_CREAT | 0666) ;
    if(Queue == -1)
    {
        perror("Couldn't Create Message Queue with the Scheduler!");
        exit(-1) ;
    }
    else
        printf("Queue id is %d\n" , Queue);

    //1. Reading the file
    FILE* file ;
    char line[100] ;
    file = fopen("processes.txt", "a+");

    if(file == NULL)
        printf("Can't Open The File\n");

    long processesCounter = 0 ;
    while (fgets(line, sizeof(line) , file))
    {
        if(line[0] == '#')
            continue;
        processesCounter ++ ;
    }

    printf("process Counter = %li \n" , processesCounter);

    fclose(file);
    file = fopen("processes.txt", "a+");

    struct processEntry*ptable  = malloc(processesCounter * 40) ;  // ana hena ghalebn h2ra el file mrten 34an a3rf el size
    processesCounter = 0 ;
    char c[100] ;
    while (fscanf(file , "%s" ,  c) == 1)
    {
        if(c[0] == '#')
        {
            char ignore[1024];
            fgets(ignore, sizeof(ignore), file);
        }
        else
        {
            struct processEntry* p = malloc(40);  // create new proccess
            p->header = 1 ; // setting the header always equal to 1
            sscanf(c , "%d" , &p->id);
            if(p->id == 0)
                p->id = 1;
            fscanf(file, "%d %d %d",&p->arrivalTime, &p->runningTime, &p->priority); // read it from the file
            p->remainingTime = p->runningTime ;
            ptable[processesCounter] = *p  ;
            processesCounter++ ;
        }
    }

    fclose(file);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int schdulAlgoNo = 0 ;
    int Q = 0; // Quantum
    printf("Please enter number correspond for the required algo\n1- HPF\n2- SRTN\n3-RR \n") ;
    scanf("%d" , &schdulAlgoNo);

    if(schdulAlgoNo == 3){
        printf("Enter the Quantum you need \n");
        scanf("%d", &Q);
    }
    // 3. Initiate and create the scheduler and clock processes.
    int pid ;
    pid = fork();
    if(pid == 0) {
        if(execv("./clk.out", argv))   // run the program
            perror("Couldent execv");
    }
    else{
        int pid2 = fork();
        if(pid2 == 0)
        {
            // argv -> should contain the type of algo,
            // if RR -> send Quantum
            char s1[5] ;
            char s2[5] ;
            char *s3 = malloc(1 * processesCounter) ;
            sprintf(s3 , "%ld" , processesCounter) ;
            sprintf(s1 , "%d" , schdulAlgoNo);  // converting the int to string
            sprintf(s2 , "%d" , Q);
            //sending these data as arguments to the schedular
            char * data[] = {s1, s2 , s3} ;
            char * envp [] = {"some" , "enviroment" , NULL} ;
            printf("iam executing execv of pid2\n") ;
            //free(s3);
            if(execve("./scheduler.out", data, envp) == -1 )
                perror("Couldent execve");
        }
    }


    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // 6. Send the information to the scheduler at the appropriate time.
    //should iterate over the datastructure and by using the clock i should send the parameters to the schedular
    int counter = 0;
    int x =  0 ;
    while(1 && counter < processesCounter)
    {
        // work on the Clock
        x=getClk() ;
        printf("current time is %d\n", x);
        if(x >= ptable[counter].arrivalTime) // if it is the time for it to be sent then send it to the schedular
        {

            struct processEntry p = ptable[counter];
            counter++;
            int send = msgsnd(Queue , &p ,sizeof(p), !IPC_NOWAIT && MSG_NOERROR);
            if(send == -1)
            {
                perror("fail to send\n") ;
                //exit(-1);
            }
        }

        sleep(1);   // sleep the second of the clock
    }
    // 7. Clear clock resources
    //destroyClk(true);
    free (ptable) ;
    raise(SIGINT);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    system("ipcrm --all=msg"); // turn off all message queues 34an da el server nafso fa lw afl khlas
    signal(SIGINT, 0);
    raise(SIGINT) ;
}