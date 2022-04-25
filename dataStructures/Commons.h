#pragma once

#include <stdbool.h> 

typedef struct ProcessEntry
{
    int entryId;
    int arrival;
    int runTime;
    int priority;
} ProcessEntry;

typedef struct ProcessMsgBuffer
{
    long type;
    struct ProcessEntry p;
} ProcessMsgBuffer;


typedef struct PCB
{
    int entryId;
    int pid;
    int priority;
    int lastStartedTime; 
    int runTime;
    int arrivalTime;
    bool isRunning; // false when reading
    int startingTime;
    int remainingTime;
} PCB;

typedef struct Event
{
    int time;
    int entryId;
    short state;
    int arrival;
    int total;
    int remaining;
    int wait;
    int turnaroundTime;
    float weightedTurnaroundTime;
} Event;
