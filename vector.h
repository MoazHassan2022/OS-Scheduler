#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "priority_queue.h"


#define VECTOR_INITIAL_CAPACITY 16


typedef struct {
    int size;
    int capacity;
    process_memory *data;
} Vector;

void init(Vector *vector){
{
    vector->size = 0;
    vector->capacity = VECTOR_INITIAL_CAPACITY;
    vector->data = malloc(sizeof(process_memory) * vector->capacity);
}

};

void resize(Vector *vector){
    if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
    }
}

void push_back(Vector *vector, process_memory value){
    resize(vector);

    vector->data[vector->size++] = value;
}


void set(Vector *vector, int index, process_memory value){
    process_memory p;
        p.Process_start_location = -1;
    p.Process_end_location = -1;
    while(index >= vector->size){
        push_back(vector, p);
    }

    vector->data[index] = value;
}


void push_front(Vector *vector, process_memory value){
    set(vector, 0, value);
    vector->size++;
}


void delete(Vector *vector, int index){
    for(int i = 0; i < index; i++){
        vector->data[index + i] = vector->data[index + i + 1];
    }
    vector->size--;
}


void free_memory(Vector *vector){
    free(vector->data);
}
process_memory get(Vector *vector, int index){
    if(index > vector->capacity || index < 0){
        printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
        exit(1);
    }

    return vector->data[index];
}

int size_of_vec(Vector *vector){
    return vector->size;
}

int capacity(Vector *vector){
    return vector->capacity;
}

bool is_empty(Vector *vector){
    return vector->size == 0;
}
