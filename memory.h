#include <vector>


#define near_block(Process_end_location) ceil(log(Process_end_location) / log(2));
#define mp(x,y) std::make_pair(x,y)

struct process_memory {
    int Process_start_location;
    int Process_end_location;
};
void init_process_memory(struct process_memory* prm, int s, int en) {
      prm->Process_start_location = s;
      prm->Process_end_location = en; 
}

int size = 8;

// the free nodes of various sizes
std::vector<struct process_memory> arr[9];

void Buddy()
{
    for (int i = 0; i < 4; i++){ 
        struct process_memory prm;
        init_process_memory(&prm, i * 256, ( (i+1) * 256 ) - 1));
        arr[8].push_back(prm);
    }
}

struct process_memory allocate(int process_size)
{
    int x = near_block(process_size);

    if (arr[x].size() > 0)
    {
        struct process_memory temp = arr[x][0];
        arr[x].erase(arr[x].begin());
        printf("Meomry from %d to %d allocated\n", temp.Process_start_location, temp.Process_end_location);
        return temp;
    }
    else
    {
        int i;
        for (i = x + 1; i <= size + 1; i++) 
            if(arr[i].size() > 0) break;

        if (i > size)
        {
            printf("Sorry, No enough memory ya moza\n");
            struct process_memory prm;
            init_process_memory(&prm, -1, -1);
            return prm;
        }
        else
        {
            struct process_memory temp =  arr[i][0];
            arr[i].erase(arr[i].begin());
            i--;
            for (; i >= x; i--)
            {
                // divide the big to two small
                struct process_memory pair1;
                struct process_memory pair2;
                init_process_memory(&pair1, temp.Process_start_location, temp.Process_start_location + (temp.Process_end_location - temp.Process_start_location) / 2);
                init_process_memory(&pair1, temp.Process_start_location + (temp.Process_end_location - temp.Process_start_location + 1) / 2, temp.Process_end_location);
                // push them
                arr[i].push_back(pair1); 
                arr[i].push_back(pair2);
                temp = arr[i][0];
                // remove the big one 
                arr[i].erase(arr[i].begin());
            }
            printf("Meomry from %d to %d allocated\n", temp.Process_start_location, temp.Process_end_location);
            return temp;
        }
    }
}



int merge(int start_of_process, int Process_size)
{
    int i, buddyNumber, buddyAddress, new_time;

    int n = near_block(Process_size);

    // Calculate buddy number
    buddyNumber = start_of_process / Process_size;

    if (buddyNumber % 2 != 0)
        buddyAddress = start_of_process - pow(2, n);
    else
        buddyAddress = start_of_process + pow(2, n);

    // Search in free list to find it's buddy
    for (i = 0; i < arr[n].size(); i++)
    {
        // If buddy found and is also free
        if (arr[n][i].Process_start_location == buddyAddress)
        {
            // Now merge the buddies to make
            // them one large free memory block
            if (buddyNumber % 2 == 0)
            {
                struct process_memory prm;
                init_process_memory(&prm, start_of_process, start_of_process + 2 * (pow(2, n) - 1));
                arr[n + 1].push_back(prm);

                printf("merge of blocks starting at %d and %d Done\n", start_of_process, buddyAddress);
                new_time = start_of_process;
            }
            else
            {
                struct process_memory prm;
                init_process_memory(&prm, buddyAddress, buddyAddress + 2 * (pow(2, n)));
                arr[n + 1].push_back(prm);
                printf("merge of blocks starting at %d and %d Done\n", buddyAddress, start_of_process);
                new_time = buddyAddress;
            }
            arr[n].erase(arr[n].begin() + i);
            arr[n].erase(arr[n].begin() + arr[n].size() - 1);
            return new_time;
        }
    }
    return -1;
}

void deallocate(struct process_memory * p)
{
    int start_time = p->Process_start_location, s_size = p->Process_end_location - p->Process_start_location + 1;
    // Size of block to be searched
    int n = near_block(s_size);
    // Add the block in free array
    struct process_memory prm;
    init_process_memory(&prm, start_time, start_time + pow(2, n) - 1);
    arr[n].push_back(prm);
    printf("Memory block from %d to %d deallocated\n", start_time, int(start_time + pow(2, n) - 1));
    while (1) {
        if (s_size >= 256) break;
        start_time = merge(start_time, s_size);
        if (start_time == -1) break;
        s_size *= 2;
    }
}




// Driver code
int main()
{
    Buddy();
    std::vector<struct process_memory > created;
    created.push_back(allocate(2));
    created.push_back(allocate(200));
    created.push_back(allocate(4));
    created.push_back(allocate(256));
    created.push_back(allocate(10));
    created.push_back(allocate(112));


    for (struct process_memory s : created) {
        if(s.Process_start_location != -1) deallocate(&s);
    }


    return 0;
}