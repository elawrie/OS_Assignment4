#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "Checksum.c"
#include "buffer.h"


/* the buffer */
BUFFER_ITEM buffer[BUFFER_SIZE];
pthread_mutex_t mutex;
sem_t *empty, *full;

int idx_prod = 0;
int idx_cons = 0;

void initialize()
{
    pthread_mutex_init(&mutex, NULL);
    empty = sem_open("empty", O_CREAT | O_EXCL, 0664, BUFFER_SIZE);
    full = sem_open("full", O_CREAT | O_EXCL, 0664, 0);
}

int insert_item(BUFFER_ITEM *item)
{
    /* insert item into buffer */
    sem_wait(empty);
    pthread_mutex_lock(&mutex);

    memcpy(&buffer[idx_prod++], item, sizeof(BUFFER_ITEM));
    idx_prod = idx_prod % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(full);
    return 0;
}

int remove_item(BUFFER_ITEM *item)
{
    /* remove an object from buffer */
    sem_wait(full);
    pthread_mutex_lock(&mutex);

    memcpy(item, &buffer[idx_cons++], sizeof(BUFFER_ITEM));
    idx_cons = idx_cons % BUFFER_SIZE;

    pthread_mutex_unlock(&mutex);
    sem_post(empty);
    return 0;
}

void *producer(void *param)
{
    BUFFER_ITEM item;

    while (1)
    {
        /* sleep for a random period of time between 1-5 */
        sleep(rand() % 5 + 1);
        /* generate a random number */
        for (int i = 0; i < 30; ++i)
        {
            item.data[i] = rand() % 256;
        }
        item.cksum = checksum((char *)item.data, sizeof(item.data));
        if (insert_item(&item))
        {
            fprintf(stderr, "Error: Buffer is Full\n");
        }
        else
        {
            printf("Producer produced data with a checksum: %d\n", item.cksum);
        }
    }
}

void *consumer(void *param)
{
    BUFFER_ITEM item;
    uint16_t cksum;

    while (1)
    {
        /* sleep for a random period of time between 1-5 */
        sleep(rand() % 5 + 1);
        if (remove_item(&item))
        {
            fprintf(stderr, "Error: Buffer is Empty\n");
        }
        else
        {
            cksum = checksum((char *)item.data, sizeof(item.data));
            if (cksum != item.cksum)
            {
                fprintf(stderr, "Error: Invalid Checksum Detected\n");
            }
            else
            {
                printf("Consumer consumed data with checksum %d\n", item.cksum);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    if (argc != 4)
    {
        printf("Format of command line input must be './prodcon <delay> <#producer threads> <#consumer threads>'\n");
        return -1;
    }

    if (!(atoi(argv[1]) && atoi(argv[2]) && atoi(argv[3])))
    {
        printf("atoi: %d(%s)\n", errno, strerror(errno));
        printf("Format of command line input must be './prodcon <delay> <#producer threads> <#consumer threads>'\n");
        return -1;
    }

    if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0 || atoi(argv[3]) < 0)
    {
        printf("Number of points must be positive\n");
        return -1;
    }

    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        memset(&buffer[i], 0, sizeof(BUFFER_ITEM));
    }

    int delay = atoi(argv[1]);
    int nproducer_threads = atoi(argv[2]);
    int nconsumer_threads = atoi(argv[3]);

    /* 2. Initialize buffer */
    initialize();

    /* 3. Create producer thread(s) */
    pthread_t pthread;
    for (int i = 0; i < nproducer_threads; ++i)
    {
        pthread_create(&pthread, NULL, producer, NULL);
    }

    /* 4. Create consumer thread(s) */
    for (int i = 0; i < nconsumer_threads; ++i)
    {
        pthread_create(&pthread, NULL, consumer, NULL);
    }

    /* 5. Sleep */
    sleep(delay);

    // clean up
    sem_unlink("empty");
    sem_unlink("full");
    pthread_mutex_destroy(&mutex);

    /* 6. Exit */
    return 0;
}