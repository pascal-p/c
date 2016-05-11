/*****************************************************************************
*
* -------------------------------- queue.c --------------------------------- *
*
*****************************************************************************/
#include <stdlib.h>

#include "sllist.h"
#include "queue.h"

int queue_enqueue(queue_t *q, const void *data) {
  return list_ins_next(q, list_tail(q), data);
}

int queue_dequeue(queue_t *q, void **data) {
  return list_rem_next(q, NULL, data);
}
