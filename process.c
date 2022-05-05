#include "headers.h"

/* Modify this file as needed*/
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
    }
    destroyClk(false);

    exit(0);
}