#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>

typedef struct queue_t queue_t;

/**
 * Creates a queue.
 *
 * @return A pointer to the created queue.
 */
queue_t* create_queue();

/**
 * Frees all associated memory with the queue.
 *
 * @param queue A pointer to the pointer of the queue to be destroyed.
 */
void destroy_queue(queue_t** const queue);

/**
 * Checks if the queue is empty.
 *
 * @param queue A pointer to the queue.
 * @return True if the queue is empty, false otherwise.
 */
bool queue_is_empty(const queue_t* const queue);

/**
 * Removes an element from the front of the queue.
 *
 * @param queue A pointer to the queue.
 * @param dest A pointer to the destination variable to store the dequeued value.
 * @return 0 if successful, or a non-zero value (-1) if an error occurred.
 *
 * @note The dequeued value MUST be freed
 */
int dequeue(queue_t* const queue, char** const dest);

/**
 * Adds an element to the back of the queue.
 *
 * @param queue A pointer to the queue.
 * @param value A pointer to the value to be enqueued.
 */
void enqueue(queue_t* const queue, const char* const value);

#endif  // __QUEUE_H__
