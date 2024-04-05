#include <stdint.h>

/* buffer.h */
typedef struct buffer_item {
    uint8_t data[30];
    uint16_t cksum;
} BUFFER_ITEM;

#define BUFFER_SIZE 10

// function definitions
int buffer_init();
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
