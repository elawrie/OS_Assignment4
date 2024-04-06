#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
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
        // sleep for a random period of time
        int sleep_time = rand() % 5 + 1; 
        // generate a random number 
        for (int i = 0; i < 30; i++) {
            item.data[i] = (uint8_t) rand() % 256; // assign random values to data
        }

        // calculate checksum
        item.cksum = checksum((char*)item.data, sizeof(item.data)); 
        sleep(sleep_time); // sleep for a random period of time

        if (insert_item(&item) == 0) {
            printf("Producer produced with checksum: %d \n", item.cksum);
        }
        else {
            fprintf(stderr, "Error: unable to insert into full buffer. Retrying now\n"); 
        }
    }
}

// consumer function 
void *consumer (void *param) {
    BUFFER_ITEM item;
    uint16_t check;

    while (true) {
        // sleep for a random period of time 
        int sleep_time = rand() % 5 + 1; 
        sleep(sleep_time);

        // consume the item
        if (remove_item(&item) == 0) {
            // calculate checksum
            check = checksum((char*)item.data, sizeof(item.data));
            if (check == item.cksum) {
                printf("Consumer consumed data with checksum: %d \n", item.cksum);
            }
            else {
                printf("Error: there is a checksum mismatch. Received checksum: %d and expected checksum: %d\n", check, item.cksum);
                exit(0);
            }
        }
        else {
            fprintf(stderr, "Error: unable to remove from empty buffer. Retrying now\n");
        }
    }
}


int main (int argc, char *argv[]) {
    int sleep_time;
    int num_producers;
    int num_consumers;

    // error handling for incorrect number of arguments
    if (argc != 4) {
        fprintf(stderr, "Proper format: %s sleep_time num_producers num_consumers\n", argv[0]);
        return -1;
    }

    sleep_time = atoi(argv[1]);
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[3]);

    // error handling for negative values
    if (sleep_time < 0 || num_producers < 0 || num_consumers < 0) {
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    // clean and initialize buffer
    buffer_cleanup();
    buffer_init();

    // create producer threads
    pthread_t threads;
    for (int i = 0; i < num_producers; ++i) {
        pthread_create(&threads, NULL, producer, NULL);
    }

    // create consumer threads
    for (int i = 0; i < num_consumers; ++i) {
        pthread_create(&threads, NULL, consumer, NULL);
    }

    // sleep
    sleep(sleep_time);

    return 0;
}