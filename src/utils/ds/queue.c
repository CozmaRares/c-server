#include "queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils.h"

struct node {
    char* value;
    struct node* next;
};

struct queue_t {
    struct node* head;
    struct node* tail;
};

struct node* create_node(const char* const value) {
    struct node* node;
    MALLOC(struct node, node, 1);

    node->value = new_string(value);
    node->next  = NULL;

    return node;
}

queue_t* create_queue() {
    queue_t* q;
    CALLOC(queue_t, q, 1);
    return q;
}

void free_node(struct node* n, bool free_list) {
    if (n == NULL)
        return;
    if (free_list) {
        free_node(n->next, free_list);
        free(n->value);
    }
    free(n);
}

void free_queue(queue_t** const queue) {
    queue_t* q = *queue;
    free_node(q->head, true);
    free(q);
    *queue = NULL;
}

bool queue_is_empty(const queue_t* const queue) {
    return queue->head == NULL;
}

int dequeue(queue_t* const queue, char** const dest) {
    if (queue_is_empty(queue))
        return -1;

    struct node* n = queue->head;

    if (n == queue->tail)
        queue->head = queue->tail = NULL;
    else
        queue->head = queue->head->next;

    *dest = n->value;

    free_node(n, false);
    return 0;
}

void enqueue(queue_t* const queue, const char* const value) {
    struct node* n = create_node(value);

    if (queue_is_empty(queue))
        queue->head = queue->tail = n;
    else
        queue->tail = queue->tail->next = n;
}
