//Struct process data for all processes loaded from input file and processes in ready queue
struct processEntry {
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int remainingTime;
    int criteria;
    int PID;
    int runningID; //order of running
    processData(){}
    processData(int i, int a, int r, int p)
    {id=i; arrivalTime=a; runningTime=r; priority=p;}
    /* overload the less-than operator so priority queues know how to compare two processData objects */
    bool operator>(const processData& right) const
    {
        return criteria < right.criteria;
    }
    bool operator<(const processData& right) const
    {
        return criteria > right.criteria;
    }

    void print() {
        printf("\n Process Data: id %d \t arrivaltime %d runningtime %d priority %d", id, arrivalTime, runningTime,
               priority);
    }

} processEntry;


//Struct for process data in process control block
struct processBlock {
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int remainingTime;
    int startTime;
    int finishTime;
    int waitTime; //derived from other attributes?
    string state;

processBlock() {id = -1;}
processBlock(struct processData data) {
 	id= data.id;
	arrivalTime=data.arrivalTime;
	runningTime=data.runningTime;
	priority=data.priority;
	remainingTime=data.remainingTime;
	
}

}processBlock;

//Struct message buffer to be used when sending a process from process generator to scheduler
struct processMsgBuff
{
   long mtype;
   struct processData mProcess;
};

//Enum for Algorithms' choice
enum Algorithm {
	HPF,
	SRTN,
	RoundRobin
};
