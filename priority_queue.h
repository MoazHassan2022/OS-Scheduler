/**
 * @file priority_queue.h
 * @author our Team
 * @brief this file contains the data structure (priority queue which is used by the SRTN and HPF)
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
