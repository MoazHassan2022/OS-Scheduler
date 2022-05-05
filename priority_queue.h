#include "Commons.h"


struct processEntry *H;
int size = -1;
short sorting = 1;


bool isPriorityQueueEmpty()
{
    return (size < 0);  //0 -> 1 element
}

void setSorting(short sortingVal){
    sorting = sortingVal;
}

void initPriorityQueue(int sizeOfQueue)
{
    H= (struct processEntry*)malloc(sizeOfQueue * sizeof(struct processEntry));
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


void shiftUp(int i)
{
    if(sorting == 0){
        while (i > 0 && H[parent(i)].remainingTime > H[i].remainingTime) {
            pswap(&H[parent(i)], &H[i]);

            i = parent(i);
        }
    }
    else{
        while (i > 0 && H[parent(i)].priority > H[i].priority) {
            pswap(&H[parent(i)], &H[i]);

            i = parent(i);
        }
    }
}

void shiftDown(int i)
{
    int maxIndex = i;

    int l = leftChild(i);
    if(sorting == 0){
        if (l <= size && H[l].remainingTime < H[maxIndex].remainingTime) {
            maxIndex = l;
        }

        int r = rightChild(i);

        if (r <= size && H[r].remainingTime < H[maxIndex].remainingTime) {
            maxIndex = r;
        }

        if (i != maxIndex) {
            pswap(&H[i], &H[maxIndex]);
            shiftDown(maxIndex);
        }
    }
    else{
        if (l <= size && H[l].priority < H[maxIndex].priority) {
            maxIndex = l;
        }

        int r = rightChild(i);

        if (r <= size && H[r].priority < H[maxIndex].priority) {
            maxIndex = r;
        }

        if (i != maxIndex) {
            pswap(&H[i], &H[maxIndex]);
            shiftDown(maxIndex);
        }
    }
}

void insert(struct processEntry p)
{
    size = size + 1;
    H[size] = p;

    shiftUp(size);
}

struct processEntry extractMax()
{
    struct processEntry result ;

    if(isPriorityQueueEmpty())
        return result;

    result = H[0];

    H[0] = H[size];
    size = size - 1;

    shiftDown(0);
    return result;
}

struct processEntry getMax()
{
    return H[0];
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
    while (i <= size) {
        printf("%d ", H[i].priority);
        i++;
    }

    printf("\n");

    printf("Node with maximum priority : %d \n" ,extractMax().priority);

    printf( "Priority queue after extracting maximum : ");
    int j = 0;
    while (j <= size) {
        printf("%d ",H[j].priority);
        j++;
    }
    printf("\n");

    return 0;
}
 */