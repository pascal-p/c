#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "sllist.h"

/*
void list_iter(sllist_t *lst) {
  printf(" - list: (size: %3d) ", list_size(lst));
  //
  for (sllistItm_t *plitm = list_head(lst); plitm != NULL; plitm = plitm->next) {
    printf("%4d ", *((int *) plitm->data));
  }
  //
  printf("\n");
}
*/

void cb(const void *pdata) {
  printf("%4d ", *((int *) pdata));
}

int i_match(const void *p1, const void *p2) {
  return (*(int *)p1 == *(int *)p2) ? 1 : 0;
}

int main(int argc, char **argv) {
  sllist_t *lst = NULL;

  if (list_new(&lst, &i_match, NULL, &cb) != 0) {
    printf("Error: could not allocate room for a new list\n");
    return(1);
  }
  // int lst
  int i = 2;
  int j = 6;
  int k = 4;    
  //  
  list_ins_next(lst, NULL, &i);
  list_ins_next(lst, list_head(lst), &j);
  list_ins_next(lst, list_tail(lst), &k);
  list_iter(lst);
  //
  sllistItm_t *p = NULL;
  if (list_find(lst, &j, &p)) {
    printf("[+] found itm %3d in the list: [%3d]\n", j, *((int *)(p->data)));
  }
  int z =10;
  if (list_find(lst, &z, &p)) {
    printf("[+] found itm %3d in the list: [%3d]\n", z, *((int *)(p->data)));
  }
  else {
    printf("[+] itm %3d NOT found in the list\n", z);
  }

  // removal middle
  int *pj = &j;
  if (list_rem(lst, (void **) &pj) == 0) {
    printf("[+] removed %3d from the list\n", *pj);
  }
  list_iter(lst);
  list_ins_next(lst, list_head(lst), pj);

  // removal head
  int *pi = &i;
  if (list_rem(lst, (void **) &pi) == 0) {
    printf("[+] removed (head) %3d from the list\n", *pi);
  }
  list_iter(lst);
  list_ins_next(lst, NULL, pi);

  // removal tail
  int *pk = &k;
  if (list_rem(lst, (void **) &pk) == 0) {
    printf("[+] removed (tail) %3d from the list\n", *pk);
  }
  list_iter(lst);
  list_ins_next(lst, list_tail(lst), pk);
  
  printf("list (now): \n");
  list_iter(lst);
  printf(" -- -- -- -- -- -- -- \n");

  // removal any point in the list with list_rem(...)
  pk = &k;
  printf("about to remove item: %d\n", *pk);
  if (list_rem(lst, (void **) &pk) == 0) {
    printf("new final list: \n");
    list_iter(lst);
  }
  pk = &i;
  printf("about to remove item: %d\n", *pk);
  if (list_rem(lst, (void **) &pk) == 0) {
    printf("new final list: \n");
    list_iter(lst);
  }
  pk = &j;
  printf("about to remove item: %d\n", *pk);
  if (list_rem(lst, (void **) &pk) == 0) {
    printf("new final list: \n");
    list_iter(lst);
  }

  list_free(&lst);
  assert(lst == NULL);
  //
  return 0;
}
