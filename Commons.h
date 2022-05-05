#include <stdbool.h>
/*Struct process data for all processes loaded from input file and processes in ready queue*/
struct processEntry {
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int remainingTime;
    int criteria;
    int PID;
    int runningID; //order of running
    long header;
} processEntry;


/*Struct for process data in process control block*/
struct processBlock {
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int remainingTime;
    int startTime;
    int finishTime;
    int waitTime; //derived from other attributes?
    char* state;
}processBlock;

/*Struct message buffer to be used when sending a process from process generator to scheduler*/
struct processMsgBuff
{
   long mtype;
   struct processEntry mProcess;
};
