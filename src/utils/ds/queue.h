#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

typedef struct queue_t queue_t;

queue_t* create_queue();
void destroy_queue(queue_t** const queue);
bool queue_is_empty(const queue_t* const queue);
int dequeue(queue_t* const queue, char** const dest);
void enqueue(queue_t* const queue, const char* const value);

#endif  // __QUEUE_H__
