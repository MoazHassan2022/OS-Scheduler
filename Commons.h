/**
 * @file Commons.h
 * @author our team 
 * @brief this file contains all the common data that we use in all the algorithms and the other files 
 * we decided to separate it from the header file to be clear and more organizable 
 * @version 0.1
 * @date 2022-05-29
 * @copyright Copyright (c) 2022
 * 
 */
#include <stdbool.h>

/*Struct process data for all processes 
loaded from input file and processes in ready queue*/
typedef struct process_memory {
    int Process_start_location;
    int Process_end_location;
} process_memory;

/**
 * @brief function to initiat the memory
 * 
 * @param s the start 
 * @param en the end 
 * @return process_memory 
 */
process_memory init_process_memory(int s, int en) {
    struct process_memory prm;
    prm.Process_start_location = s;
    prm.Process_end_location = en;
    return prm;
}

/*Struct process data for all processes 
loaded from input file and processes in ready queue*/
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
    struct process_memory* prm;
    int memSize;
    bool isAllocated;
} processEntry;

/*Struct message buffer to be used when 
sending a process from process generator to scheduler*/
struct processMsgBuff
{
   long mtype;
   struct processEntry mProcess;
}processMsgBuff;
