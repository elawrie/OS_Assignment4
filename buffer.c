#include <semaphore.h>
#include <stdio.h>
#include "buffer.h"

sem_t *empty, *full, *mutex;

int index_buffer = 0;

// initialize the buffer size 
BUFFER_ITEM buffer[BUFFER_SIZE];


// initialize the buffer
int buffer_init() {
    empty = sem_open("empty", O_CREAT, 0644, BUFFER_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    mutex = sem_open("mutex", O_CREAT, 0644, 1);
    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        return -1;
    }
    return 0;
}

// insert item into buffer
int insert_item(BUFFER_ITEM item) {
    // acquire the semaphore
    sem_wait(empty);

    if (sem_trywait(empty) != 0) {
        sem_post(mutex);
        return -1;
    }

    // produce an item 
    buffer[index_buffer] = item; 
    // make the array a circular queue
    index_buffer = (index_buffer + 1) % BUFFER_SIZE; 

    // release the semaphores
    sem_post(full);
    sem_post(mutex);

    // return 0 if insertion successful 
    return 0;
} 

// remove item from buffer
int remove_item(BUFFER_ITEM *item) {
    // acquire the semaphore
    sem_wait(mutex);

    if (sem_trywait(full) != 0) {
        sem_post(mutex);
        return -1;
    }

    // consume an item
    // get item from the buffer 
    *item = buffer[index_buffer];
    // make the array a circular queue
    index_buffer = (index_buffer + 1) % BUFFER_SIZE;

    // release the semaphores
    sem_post(empty);
    sem_post(mutex);

    // return 0 if removal successful
    return 0;
}