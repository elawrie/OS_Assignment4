#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "buffer.h"

sem_t *empty, *full;
pthread_mutex_t mutex;

int index_p = 0;
int index_c = 0;

// initialize the buffer size 
BUFFER_ITEM buffer[BUFFER_SIZE];

// BUFFER_ITEM *buffer = (BUFFER_ITEM *) malloc(BUFFER_SIZE * sizeof(BUFFER_ITEM));


// initialize the buffer
int buffer_init() {
    empty = sem_open("empty", O_CREAT, 0644, BUFFER_SIZE);
    full = sem_open("full", O_CREAT, 0644, 0);
    pthread_mutex_init(&mutex, NULL); 
    // mutex = sem_open("mutex", O_CREAT, 0644, 1);

    for (int i = 0; i < BUFFER_SIZE; i++) {
        memset(&buffer[i], 0, sizeof(BUFFER_ITEM));
    }

    if (empty == SEM_FAILED || full == SEM_FAILED) {
        return -1;
    }
    return 0;
}

// cleanup function
int buffer_cleanup() {
    // close semaphores
    // sem_close(empty);
    // sem_close(full);
    // sem_close(mutex);
    pthread_mutex_destroy(&mutex);
    // unlink the semaphores
    sem_unlink("empty");
    sem_unlink("full");
    // sem_unlink("mutex");
    return 0;
}

// insert item into buffer
int insert_item(BUFFER_ITEM *item) {
    // acquire the semaphore
    sem_wait(empty);
    // sem_wait(mutex);
    pthread_mutex_lock(&mutex);

    if (sem_trywait(empty) != 0) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // produce the item 
    memcpy(&buffer[index_p++], item, sizeof(BUFFER_ITEM));
    // change index circularly 
    index_p = index_p % BUFFER_SIZE;

    // release the semaphores
    sem_post(full);
    pthread_mutex_unlock(&mutex);

    // return 0 if insertion successful 
    return 0;
} 

// remove item from buffer
int remove_item(BUFFER_ITEM *item) {
    // acquire the semaphore
    pthread_mutex_lock(&mutex);

    if (sem_trywait(full) != 0) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }

    // consume an item
    memcpy(item, &buffer[index_c++], sizeof(BUFFER_ITEM));
    // change index circularly
    index_c = index_c % BUFFER_SIZE;

    // release the semaphores
    sem_post(empty);
    pthread_mutex_unlock(&mutex);

    // return 0 if removal successful
    return 0;
}