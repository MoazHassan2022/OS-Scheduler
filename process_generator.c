/**
 * @file process_generator.c
 * @author Abdelaziz Salah (Abdelaziz132001@gmail.com)
 * @brief this file is the process generator which generates the processes and initiate the schedular and the clock and 
 * reads the data from the files and start the program
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "headers.h"
/**
 * @brief handler used for terminating house keeping by clearing all the resources.
 * 
 * @param singnum which is the number of the sent signal
 */
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


    //1. Reading the file to get the number of the processes
    FILE* file ;
    char line[100] ;
    file = fopen("processes.txt", "r");

    if(file == NULL)
        printf("Can't Open The File\n");

    long processesCounter = 0 ;

    while (fgets(line, sizeof(line) , file))
    {
        if(line[0] == '#')
            continue;
        processesCounter ++ ;
    }
    fclose(file);

    // reading the file again to store the data
    file = fopen("processes.txt", "r");
    struct processEntry*ptable  = malloc(processesCounter * sizeof(processEntry)) ;  
    processesCounter = 0 ;
    char c[100] ;
    while (fscanf(file , "%s" ,  c) == 1)
    {
        //ignore comments 
        if(c[0] == '#')
        {
            char ignore[1024];
            fgets(ignore, sizeof(ignore), file); // ignore the whole line
        }
        else
        {
            struct processEntry* p = malloc(sizeof(processEntry));  // create new proccess
            p->header = 1 ; // setting the header always equal to 1
            sscanf(c , "%d" , &p->id);
            if(p->id == 0)
                p->id = 1;
            fscanf(file, "%d %d %d %d",&p->arrivalTime, &p->runningTime, &p->priority, &p->memSize); // read it from the file
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

    // let the user insert the quatum he needs 
    if(schdulAlgoNo == 3){
        printf("Enter the Quantum you need \n");
        scanf("%d", &Q);
    }

    // 3. Initiate and create the scheduler and clock processes.
    int pid ;
    pid = fork();
    char *s3 ;
    if(pid == 0) {
        if(execv("./clk.out", argv))   // run the clock
            perror("Couldent execv");
    }
    else{
        int pid2 = fork();
        if(pid2 == 0)
        {
            char s1[5] ;
            char s2[5] ;
            s3 = malloc(processesCounter) ;
            sprintf(s3 , "%ld" , processesCounter) ;
            sprintf(s1 , "%d" , schdulAlgoNo);  // converting the int to string
            sprintf(s2 , "%d" , Q);
            //sending these data as arguments to the schedular
            char * data[] = {s1, s2 , s3} ;
            char * envp [] = {"some" , "enviroment" , NULL} ;
            if(execve("./scheduler.out", data, envp) == -1 )
                perror("Couldent execve");
        }
    }


    // 4. Use this function after creating the clock process to initialize clock
    initClk();

    // 6. Send the information to the scheduler at the appropriate time.
    //should iterate over the datastructure and by using the clock i should send the parameters to the schedular
    int counter = 0;
    int time =  0 ;
    while(1 && counter < processesCounter)
    {
        // work on the Clock
        time = getClk() ;
        
        // if it is the time for it to be sent then send it to the schedular
        if(time >= ptable[counter].arrivalTime)
        {
            struct processEntry p = ptable[counter];
            counter++;
            int send = msgsnd(Queue , &p ,sizeof(p), !IPC_NOWAIT && MSG_NOERROR);
            if(send == -1)
                perror("fail to send\n") ;
                //exit(-1);
        }
        //sleep(1);   // sleep the second of the clock
    }

    // 7. Clear clock resources
    //destroyClk(true);
    /*free(s3);
    for(int i = 0 ; i< processesCounter ; i ++) // freeing all the allocated processes
        free(&ptable[i]) ;
    free (ptable) ;*/
    //raise(SIGINT);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    system("ipcrm --all=msg"); // turn off all message queues 34an da el server nafso fa lw afl khlas
    signal(SIGINT, 0);
    raise(SIGINT) ;
}
