/* *********************************************************************
 *                                                                     *
 * --------------------- Queue Abstract Datatype --------------------- *
 *                                                                     *
 * ********************************************************************/

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "sllist.h"

typedef sllist_t queue_t;

#define queue_new list_new

#define queue_init list_init

#define queue_free list_free

/*
 * enqueue an element (pointed to by data) at the tail of the q
 */
int queue_enqueue(queue_t *q, const void *data);

/*
 * dequeue an element at the head of the q and return through data 
 */
int queue_dequeue(queue_t *q, void **data);

#define queue_peek(queue) ((queue)->head == NULL ? NULL : (queue)->head->data)

#define queue_iter list_iter

#define queue_size list_size

#endif
