
#include "vector.h" 
#include <math.h>

#define near_block(Process_end_location) ceil(log(Process_end_location) / log(2)); 
#define mp(x,y) std::make_pair(x,y) 
 
int size = 8; 
 
// the free nodes of various sizes 
Vector arr[9];


 
void Buddy() 
{ 
for (int i = 0; i < 9; i++){
            init(&arr[i]);
    }
    for (int i = 0; i < 4; i++){  
        process_memory prm = init_process_memory( i * 256, ( (i+1) * 256 ) - 1); 
        push_back(&arr[8],prm);
    } 
} 
 
struct process_memory allocate(int process_size) 
{ 
    int x = near_block(process_size); 
    
    if (!is_empty(&arr[x])) 
    { 
        struct process_memory temp = get(&arr[x], 0); 
        delete(&arr[x],0);
        printf("Meomry from %d to %d allocated\n", temp.Process_start_location, temp.Process_end_location); 
        return temp; 
    } 
    else 
    { 
        int i; 
        for (i = x + 1; i <= size + 1; i++)  
            if(!is_empty(&arr[i]) > 0) break; 
 
        if (i > size) 
        { 
            printf("Sorry, No enough memory ya moza\n"); 
            struct process_memory prm = init_process_memory(-1, -1); 
            return prm; 
        } 
        else 
        { 
            struct process_memory temp =  get(&arr[i], 0); 
            delete(&arr[i],0);
            i--; 
            for (; i >= x; i--) 
            { 
                // divide the big to two small 
                process_memory pair1 = init_process_memory(temp.Process_start_location, temp.Process_start_location + (temp.Process_end_location - temp.Process_start_location) / 2); 
                process_memory pair2 = init_process_memory(temp.Process_start_location + (temp.Process_end_location - temp.Process_start_location + 1) / 2, temp.Process_end_location); 
                // push them 
                push_back(&arr[i],pair1);
                push_back(&arr[i],pair2);
                
                temp = get(&arr[i], 0); 
                // remove the big one  
                delete(&arr[i],0);
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
    
    for (i = 0; i < size_of_vec(&arr[n]); i++) 
    { 
        // If buddy found and is also free 
        if (get(&arr[n],i).Process_start_location == buddyAddress) 
        { 
            // Now merge the buddies to make 
            // them one large free memory block 
            if (buddyNumber % 2 == 0) 
            { 
                process_memory prm = init_process_memory( start_of_process, start_of_process + 2 * (pow(2, n) - 1)); 
                push_back(&arr[n + 1],prm);

 
                printf("merge of blocks starting at %d and %d Done\n", start_of_process, buddyAddress); 
                new_time = start_of_process; 
            } 
            else 
            { 
                process_memory prm = init_process_memory( buddyAddress, buddyAddress + 2 * (pow(2, n))); 
                push_back(&arr[n + 1],prm);
                printf("merge of blocks starting at %d and %d Done\n", buddyAddress, start_of_process); 
                new_time = buddyAddress; 
            } 
            delete(&arr[n],i);
            delete(&arr[n], arr[n].size - 1);
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
    process_memory prm = init_process_memory(start_time, start_time + pow(2, n) - 1); 
    push_back(&arr[n],prm);

    printf("Memory block from %d to %d deallocated\n", start_time, (int)(start_time + pow(2, n) - 1) ); 
    while (1) { 
        if (s_size >= 256) break; 
        start_time = merge(start_time, s_size); 
        if (start_time == -1) break; 
        s_size *= 2; 
    } 
} 
 
/*// Driver code 
int main() 
{ 
    Buddy(); 
    
    return 0; 
}*/