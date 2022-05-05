#include "headers.h"
void HPF(struct processEntry currentprocess)
{
    int pid = fork(); // schedule the process for the remaining time passed to it
    if (pid == 0)
    {
        char remainingTimePassed[10];
        sprintf(remainingTimePassed, "%d", currentprocess.remainingTime); // converting the int to string
        char *data[] = {"process.out", remainingTimePassed, NULL};
        if (execv("./process.out", data) == -1)
            perror("Coulden't execv");
    }
    else{
        int stat;
        wait(&stat)
    }
}