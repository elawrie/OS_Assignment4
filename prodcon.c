#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"
#include "Checksum.c"


//DONT FORGET TO CALCULATE CHECKSUM
void *producer(void *param){
    BUFFER_ITEM item;

    while(true){
        /* sleep for a random period of time */
        int sleep_time = rand() % 5 + 1; 
        sleep(sleep_time);
        /* generate a random number */
        for(int i = 0; i < 30; i++) {
            item.data[i] = (uint8_t) rand() % 256; // Assign random values to data
        }
        item.cksum = checksum((char*)item.data, 16); 
        if(insert_item(item) == -1){
            fprintf(stderr, "report error condition");
        }
        else{
            printf("producer produced %d\n", item);
        }
    }
}

//DONT FORGET TO CALCULATE CHECKSUM
void *consumer(void *param){
    BUFFER_ITEM item;

    while(true){
        /* sleep for a random period of time */
        int sleep_time = rand() % 5 + 1; 
        sleep(sleep_time);
        if(remove_item(&item) == -1){
            fprintf(stderr, "report error condition");
        }
        else{
            uint16_t check = checksum((char*)item.data, 16);
            if(check == item.cksum){
                printf("Consumed successfully\n");
            }
            else{
                printf("checksum FLOPPED!\n");
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
int main(int argc, char *argv[]){
    int sleep_time;
    int num_producers;
    int num_consumers;

    if(argc != 4){
        fprintf(stderr, "Usage: %s <sleep time> <num producers> <num consumers>\n", argv[0]);
        return -1;
    }

    sleep_time = atoi(argv[1]);
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[3]);

    if(sleep_time < 0 || num_producers < 0 || num_consumers < 0){
        fprintf(stderr, "Invalid arguments\n");
        return -1;
    }

    // Initialize buffer
    buffer_init();

    // Create producer threads
    pthread_t prod_threads[num_producers];
    for (int i = 0; i < num_producers; i++) {
        pthread_create(&prod_threads[i], NULL, producer, NULL);
    }
    pthread_t con_threads[num_consumers];
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&con_threads[i], NULL, consumer, NULL);
    }

    // join threads
    for (int i = 0; i < num_producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(con_threads[i], NULL);
    }

    // Sleep
    sleep(sleep_time);

    return 0;
}