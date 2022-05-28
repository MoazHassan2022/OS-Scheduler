#include<stdio.h>
#include <stdlib.h>

struct processEntry {
    long header;
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int remainingTime;
    int criteria;
    int PID;
    int runningID; //order of running
    int memSize;
} processEntry;


int main() {

    FILE* file ;
    char line[50] ;
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

    struct processEntry*ptable  = malloc(processesCounter * sizeof(processEntry)) ;  // ana hena ghalebn h2ra el file mrten 34an a3rf el size
    processesCounter = 0 ;
    char* c = malloc(sizeof(processEntry));
    while (fscanf(file , "%s" ,  c) == 1)
    {
        if(c[0] == '#')
        {
            char ignore[1024];
            fgets(ignore, sizeof(ignore), file);
        }
        else
        {
            printf("C= %s \n" , c);
            struct processEntry* p = malloc(sizeof(processEntry)/*da el size bta3 el bytes elly procces block btst5dmo*/);  // create new proccess
            p->header = 1 ; // setting the header always equal to 1

            sscanf(c , "%d" , &p->id);
            if(p->id == 0)
                p->id = 1;
            printf("Pid= %d \n" , p->id);
            fscanf(file, "%d %d %d %d",&p->arrivalTime, &p->runningTime, &p->priority, &p->memSize); // read it from the file
            p->remainingTime = p->runningTime ;
            ptable[processesCounter] = *p  ;
            printf("arrival = %d , id=%d \n", ptable[processesCounter].arrivalTime , ptable[processesCounter].id);
            processesCounter++ ;
        }
    }

    fclose(file);

    /////////////////
    FILE *fp;
    char name[50];
    int roll_no,  i, n;
    float marks;

    fp = fopen("marks.txt", "w");

    if(fp == NULL) {
        printf("file can't be opened\n");
        exit(1);
    }


    //printing arrival
    fprintf(fp , "arrival:") ;
    for(int i = 0 ; i < processesCounter ; i ++)
        fprintf(fp, " %d " , ptable[i].arrivalTime );


    //printing runTimee
    fprintf(fp , "\nrunTime: ") ;
    for(int i = 0 ; i < processesCounter ; i ++)
        fprintf(fp, " %d " , ptable[i].runningTime );


    fprintf(fp , "\npriority: ") ;
    for(int i = 0 ; i < processesCounter ; i ++)
        fprintf(fp, " %d " , ptable[i].priority );


    printf("\n Details successfully written to the file\n\n");

    fclose(fp);

    return 0;
}