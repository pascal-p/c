#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "queue.h"

/*
void queue_iter(queue_t *q) {
  printf(" - list: (size: %3d) ", queue_size(q));
  //
  for (t_SLListItm *qitm = list_head(q); qitm != NULL; qitm = qitm->next) {
    printf("%4d ", *((int *) qitm->data));
  }
  //
  printf("\n");
}
*/

void cb(const void *pdata) {
  printf("%4d ", *((int *) pdata));
}

int i_match(const void *p1, const void *p2) {
  return (*(int *)p1 == *(int *)p2) ? 0 : 1;
}

int main(int argc, char **argv) {
  queue_t *q = NULL; // (queue_t *) malloc(sizeof(queue_t));
  
  if (queue_new(&q, &i_match, NULL, &cb) != 0) {
    printf("Error: could not allocate room for a new queue\n");
    return(1);
  }
  //
  // 
  int i = 2;   
  int j = 6;
  int k = 4;    
  //
  queue_enqueue(q, &i);
  queue_enqueue(q, &j);
  queue_enqueue(q, &k);
  queue_iter(q);
  
  int *pi;
  queue_dequeue(q, (void **)&pi);
  printf("[+] dequeue element: %3d\n", *pi);
  queue_dequeue(q, (void **)&pi);
  printf("[+] dequeue element: %3d\n", *pi);
  queue_iter(q);
  //
  queue_free(&q);
  assert(q == NULL);
  return 0;
}
