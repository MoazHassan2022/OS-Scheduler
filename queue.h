//
// Created by mohamed on 30/04/22.
//



struct queue
{
    struct processEntry *items;     // array to store queue elements
    int maxqueueSize;    // maximum capacity of the queue
    int front;      // front points to the front element in the queue (if any)
    int rear;       // rear points to the last element in the queue
    int queueSize;       // current capacity of the queue
};

struct queue* newQueue(int qs)
{
    struct queue *pt = (struct queue*)malloc(sizeof(struct queue));

    pt->items = (struct processEntry*)malloc( qs * sizeof(struct processEntry));
    pt->maxqueueSize = qs;
    pt->front = 0;
    pt->rear = -1;
    pt->queueSize = 0;

    return pt;
}
int size(struct queue *pt) {
    return pt->queueSize;
}

int isEmpty(struct queue *pt) {
    printf("HardDisk Used size = %d", size(pt));
    return !size(pt);
}

struct processEntry * front(struct queue *pt)
{
    if (isEmpty(pt))
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    return &pt->items[pt->front];
}

void enqueue(struct queue *pt, struct processEntry* x)
{
    if (size(pt) == pt->maxqueueSize)
    {
        printf("Overflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->rear = (pt->rear + 1) % pt->maxqueueSize;    // circular queue
    pt->items[pt->rear] = *x;
    pt->queueSize++;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

void dequeue(struct queue *pt)
{
    if (isEmpty(pt))    // front == rear
    {
        printf("Underflow\nProgram Terminated\n");
        exit(EXIT_FAILURE);
    }

    pt->front = (pt->front + 1) % pt->maxqueueSize;  // circular queue
    pt->queueSize--;

    printf("front = %d, rear = %d\n", pt->front, pt->rear);
}
