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

    // 1. Read the input files.
    FILE * myFile = fopen ("processes.txt" , "r") ;
    if(myFile == NULL) // can't open the file
    {
        perror("Unable to open the file\n");
        exit(-1);
    }

    //determining the number of lines in the file
    int processesCounter = 0 ;
    char line [30] ;
    while(fgets(line , sizeof(line), myFile))
    {
        if(line[0] == '#') // neglect comments
            continue;
        processesCounter ++;
    }

    //vector of proccesses
    struct processEntry*ptable  = malloc(processesCounter * 40) ;  // ana hena ghalebn h2ra el file mrten 34an a3rf el size
    while(fgets(line , sizeof(line), myFile))
    {
        if(line[0] == '#') // neglect comments
            continue;
        struct processEntry* p = malloc(40 /*da el size bta3 el bytes elly procces block btst5dmo*/);  // create new proccess
        p->header = 1 ; // setting the header always equal to 1
        fscanf(myFile, "%d %d %d %d" , &p->id , &p->arrivalTime, &p->runningTime, &p->priority); // read it from the file
        ptable[processesCounter++] = *p  ;
    }

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
            char s1[10] ;
            char s2[10] ;
            sprintf(s1 , "%d" , schdulAlgoNo);  // converting the int to string
            sprintf(s2 , "%d" , Q);
            //sending these data as arguments to the schedular
            char * data[] = {s1, s2} ;
            char * envp [] = {"some" , "enviroment" , NULL} ;
            printf("iam executing execv of pid2\n") ;
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
            struct processEntry p = ptable[counter++];
            int send = msgsnd(Queue , &p , sizeof(struct processEntry) , !IPC_NOWAIT );
            if(send == -1)
            {
                perror("fail to send\n") ;
                exit(-1);
            }
        }
        sleep(1);   // sleep the second of the clock
    }
    // 7. Clear clock resources
    destroyClk(true);
    free (ptable) ;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    system("ipcrm --all=msg"); // turn off all message queues 34an da el server nafso fa lw afl khlas
    signal(SIGINT, 0);
    raise(SIGINT) ;
}