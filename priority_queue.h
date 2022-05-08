#include "Commons.h"

struct priorityQueue {
    struct processEntry *H;
    int mySize ;
    short sorting ;
};


bool isPriorityQueueEmpty(struct priorityQueue* q)
{
    return (q->mySize < 0);  //0 -> 1 element
}

void setSorting(short sortingVal,struct priorityQueue* q){
    q->sorting = sortingVal;
}

void initPriorityQueue(int sizeOfQueue,struct priorityQueue* q)
{
    q->H= (struct processEntry*)malloc(sizeOfQueue * sizeof(struct processEntry));
    for(int i = 0 ; i < sizeOfQueue ; i++ )
    {
        q->H[i].priority = 100000000;
        q->H[i].id = -1;
    }
    q->mySize=-1;
    q->sorting=1;
}

int parent(int i)
{
    return (i - 1) / 2;
}

int leftChild(int i)
{
    return ((2 * i) + 1);
}

int rightChild(int i)
{
    return ((2 * i) + 2);
}

void pswap(struct processEntry *A, struct processEntry *B){
    struct processEntry temp = *A;
    *A = *B;
    *B = temp;
}


void shiftUp(int i,struct priorityQueue* q)
{
    if(q->sorting == 0){
        while (i > 0 && q->H[parent(i)].remainingTime > q->H[i].remainingTime) {
            pswap(&q->H[parent(i)], &q->H[i]);

            i = parent(i);
        }
    }
    else{
        while (i > 0 && q->H[parent(i)].priority > q->H[i].priority) {
            pswap(&q->H[parent(i)], &q->H[i]);

            i = parent(i);
        }
    }
}

void shiftDown(int i,struct priorityQueue* q)
{
    int maxIndex = i;

    int l = leftChild(i);
    if(q->sorting == 0){
        if (l <= q->mySize && q->H[l].remainingTime < q->H[maxIndex].remainingTime) {
            maxIndex = l;
        }

        int r = rightChild(i);

        if (r <= q->mySize && q->H[r].remainingTime < q->H[maxIndex].remainingTime) {
            maxIndex = r;
        }

        if (i != maxIndex) {
            pswap(&q->H[i], &q->H[maxIndex]);
            shiftDown(maxIndex,q);
        }
    }
    else{
        if (l <= q->mySize && q->H[l].priority < q->H[maxIndex].priority) {
            maxIndex = l;
        }

        int r = rightChild(i);

        if (r <= q->mySize && q->H[r].priority < q->H[maxIndex].priority) {
            maxIndex = r;
        }

        if (i != maxIndex) {
            pswap(&q->H[i], &q->H[maxIndex]);
            shiftDown(maxIndex,q);
        }
    }
}

void insert(struct processEntry p,struct priorityQueue* q)
{
    q->mySize += 1;
    q->H[q->mySize] = p;

    shiftUp(q->mySize,q);
}

struct processEntry extractMax(struct priorityQueue* q)
{
    struct processEntry result ;

    if(isPriorityQueueEmpty(q)) {
        return result;
    }
    result = q->H[0];

    q->H[0] = q->H[q->mySize];
    q->mySize -= 1;

    shiftDown(0,q);
    return result;
}

struct processEntry getMax(struct priorityQueue* q)
{
    return q->H[0];
}


/*
int main()
{
    struct processEntry p[3];
    for(int i = 0; i<3; i++){
        p[i].id = i;
        p[i].priority = 3 - i;
        insert(p[i]);
    }
    int i = 0;
    printf("Priority Queue : ");
    while (i <= mySize) {
        printf("%d ", H[i].priority);
        i++;
    }

    printf("\n");

    printf("Node with maximum priority : %d \n" ,extractMax().priority);

    printf( "Priority queue after extracting maximum : ");
    int j = 0;
    while (j <= mySize) {
        printf("%d ",H[j].priority);
        j++;
    }
    printf("\n");

    return 0;
}
 */