#include "headers.h"

void handler(int signum);
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    remainingtime = atoi(argv[0]);
    int lastClk = getClk();
    int currentClk = lastClk;
    while (remainingtime > 0)
    {
        currentClk = getClk();
        if(currentClk != lastClk){
            remainingtime -= (currentClk - lastClk);
            printf("remaining %d\n", remainingtime);
        }
        lastClk = currentClk;
        signal(SIGUSR2,handler);
    }
    destroyClk(false);

    exit(0);
}

void handler(int signum)
{
    if(signum==SIGUSR2)
    {
        exit(0);
    }
}