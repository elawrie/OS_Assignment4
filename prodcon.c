#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "buffer.h"


//DONT FORGET TO CALCULATE CHECKSUM
void *producer(void *param){
    BUFFER_ITEM item;

    while(true){
        /* sleep for a random period of time */
        sleep(1); // Sleep for 1 second
        /* generate a random number */
        for(int i = 0; i < 30; i++) {
            item.data[i] = rand() % 256; // Assign random values to data
        }
        item.cksum = 0; // Initialize checksum
        if(insert_item(item)){
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
        sleep(2); // Sleep for 2 seconds
        if(remove_item(&item)){
            fprintf(stderr, "report error condition");
        }
        else{
            printf("consumer consumed %d\n", item);
        }
    }
}