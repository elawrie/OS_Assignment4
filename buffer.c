#include <semaphore.h>
#include <stdio.h>
#include "buffer.h"

sem_t *empty, *full, *mutex;

int index_buffer = 0;

// initialize the buffer size 
BUFFER_ITEM buffer[BUFFER_SIZE];

// BUFFER_ITEM *buffer = (BUFFER_ITEM *) malloc(BUFFER_SIZE * sizeof(BUFFER_ITEM));


// initialize the buffer
// int buffer_init() {
//     empty = sem_open("empty", O_CREAT, 0644, BUFFER_SIZE);
//     full = sem_open("full", O_CREAT, 0644, 0);
//     mutex = sem_open("mutex", O_CREAT, 0644, 1);
//     if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
//         return -1;
//     }
//     return 0;
// }
int buffer_init() {
    // Initialize semaphores globally
    empty = sem_open("/empty", O_CREAT, 0666, BUFFER_SIZE);
    if (empty == SEM_FAILED) {
        perror("sem_open empty");
        return 1;
    }

    full = sem_open("/full", O_CREAT, 0666, 0);
    if (full == SEM_FAILED) {
        perror("sem_open full");
        sem_unlink("/empty");
        sem_close(empty);
        return 1;
    }

    mutex = sem_open("/mutex", O_CREAT, 0666, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open mutex");
        sem_unlink("/empty");
        sem_unlink("/full");
        sem_close(empty);
        sem_close(full);
        return 1;
    }
    return 0;
}

// cleanup function
int buffer_cleanup() {
    // close semaphores
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    // unlink the semaphores
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");
    return 0;
}

// insert item into buffer
int insert_item(BUFFER_ITEM item) {
    // acquire the semaphore
    sem_wait(empty);
    sem_wait(mutex);

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

    // update the buffer index
    index_buffer = (index_buffer + 1) % BUFFER_SIZE;

    // release the semaphores
    sem_post(empty);
    sem_post(mutex);

    // return 0 if removal successful
    return 0;
}