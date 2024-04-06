#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "buffer.h"
#include "Checksum.c"


// producer function 
void *producer (void *param) {
    BUFFER_ITEM item;

    while (true) {
        /* sleep for a random period of time */
        int sleep_time = rand() % 5 + 1; 
        sleep(sleep_time);
        /* generate a random number */
        for (int i = 0; i < 30; i++) {
            item.data[i] = (uint8_t) rand() % 256; // Assign random values to data
        }
        item.cksum = checksum((char*)item.data, sizeof(item.data)); 
        if (insert_item(&item)) {
            fprintf(stderr, "Error: Unable to insert item into buffer\n"); 
        }
        else {
            // printf("producer produced %p\n", item);
            printf("producer produced cksum: %d \n", item.cksum);
        }
    }
}

// consumer function 
void *consumer (void *param) {
    BUFFER_ITEM item;
    uint16_t check;

    while (true) {
        /* sleep for a random period of time */
        int sleep_time = rand() % 5 + 1; 
        sleep(sleep_time);
        if (remove_item(&item)) {
            fprintf(stderr, "Error: Unable to remove item from buffer\n");
        }
        else {
            check = checksum((char*)item.data, sizeof(item.data));
            if (check != item.cksum) {
                printf("Checksum invalid (did not match)\n");
            }
            else {
                printf("consumer consumed cksum: %d \n", item.cksum);
            }
        }
    }
}

/* 1. get command line arguments argv[1] argv[2] and argv[3]*/
/* 2. initialize buffer */
/* 3. create producer thread(s) */
/* 4. create consumer thread(s) */
/* 5. sleep */
/* 6. exit */
int main (int argc, char *argv[]) {
    int sleep_time;
    int num_producers;
    int num_consumers;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <sleep time> <num producers> <num consumers>\n", argv[0]);
        return -1;
    }

    sleep_time = atoi(argv[1]);
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[3]);

    if (sleep_time < 0 || num_producers < 0 || num_consumers < 0) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    // initialize buffer
    buffer_init();

    // create producer threads
    pthread_t threads;
    for (int i = 0; i < num_producers; ++i) {
        pthread_create(&threads, NULL, producer, NULL);
    }
    // pthread_t con_threads;
    for (int i = 0; i < num_consumers; ++i) {
        pthread_create(&threads, NULL, consumer, NULL);
    }

    // // join threads
    // for (int i = 0; i < num_producers; i++) {
    //     pthread_join(prod_threads, NULL);
    // }
    // for (int i = 0; i < num_consumers; i++) {
    //     pthread_join(con_threads, NULL);
    // }

    // cleanup memory allocation 
    buffer_cleanup();

    // sleep
    sleep(sleep_time);

    return 0;
}