/*****************************************************************************
*
* -------------------------------- sllist.c -------------------------------- *
*
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sllist.h"
#include "my_malloc.h"


int list_new(sllist_t **lst, 
             int (*match)(const void *k1, const void *k2), 
             void (*destroy)(void *data),
             void (*cb)(const void *data)) {
  /*
   *lst = (sllist_t *) malloc(sizeof(sllist_t));

   if (*lst == NULL)
     return -1;    // fail to allocate space for the new list
  */
  *lst = xmalloc0(sizeof(sllist_t));

  list_init(*lst, match, destroy, cb);
  return 0;
}

void list_init(sllist_t *lst, 
               int (*match)(const void *k1, const void *k2), 
               void (*destroy)(void *data),
               void (*cb)(const void *data)) {
  lst->size    = 0;
  lst->match   = match;
  lst->destroy = destroy;
  lst->cb      = cb;
  lst->head    = lst->tail = NULL;
}

void list_free(sllist_t **lst) {
  void *data;

  while ((*lst)->size > 0) {
    if (list_rem_next(*lst, NULL, (void **) &data) == 0 && 
        (*lst)->destroy != NULL) {
      (*lst)->destroy(data);
    }
  }
  //
  memset(*lst, 0, sizeof(sllist_t));
  free(*lst);
  *lst = NULL;
  return;
}

int list_ins_next(sllist_t *lst, sllistItm_t *itm, const void *data) {
  //sllistItm_t *plitm = (sllistItm_t *) malloc(sizeof(sllistItm_t));
  sllistItm_t *plitm = xmalloc0(sizeof(sllistItm_t));

  // test that allocation was successful, if not return -1
  if (plitm == NULL) return -1;

  // OK now, cast to avoid warning: assigning to 'void *' from 'const void *' discards qualifiers
  plitm->data = (void *) data;

  // insertion - 4 cases
  if (lst->size == 0) {
    // (1) empty list, so actually itm is NULL
    plitm->next = lst->tail;
    lst->tail = lst->head = plitm;
  } 
  else if (itm == NULL) {
    // (2) list_size >0 => list is not empty, but itm == NULL
    //     => insertion at the head
    plitm->next = lst->head;
    lst->head   = plitm;
  }
  else {
    // (3) and (4)
    plitm->next = itm->next;
    itm->next   = plitm;
    if (plitm->next == NULL) lst->tail = plitm;
  }
  // insertion OK, increase the size list by 1
  lst->size++;
  return 0;
}


int list_rem_next(sllist_t *lst, sllistItm_t *itm, void **data) {
  sllistItm_t          *pitm;

  if (list_size(lst) == 0)
    return -1;

  if (itm == NULL) {
    // Handle removal from the head of the list.
    *data     = lst->head->data;
    pitm      = lst->head;
    lst->head = lst->head->next;
    if (list_size(lst) == 1) lst->tail = NULL;
  }
  else {
    // Handle removal after itm, which can be the last itm in the list
    if (itm->next == NULL) return -1;
    *data     = itm->next->data;
    pitm      = itm->next;
    itm->next = itm->next->next;
    if (itm->next == NULL) lst->tail = itm;
  }

  // Free the storage allocated by the abstract datatype
  memset(pitm, 0, sizeof(sllistItm_t));
  free(pitm);

  // Adjust the size of the list to account for the removed element
  lst->size--;

  return 0;
}

int list_rem(sllist_t *lst, void **data) {
  sllistItm_t *ppitm, *pitm;

  for (ppitm = pitm = list_head(lst); pitm != NULL; ppitm = pitm, pitm = pitm->next) {
    if (lst->match(pitm->data, *data)) break;
  }
  
  if (pitm == NULL) 
    return -1;     // *data is not in the list lst

  if (pitm == list_head(lst))
    lst->head = pitm->next;   // Handle removal from the head of the list.
  else {
    ppitm->next = pitm->next;
    if (pitm == list_tail(lst)) 
      lst->tail = ppitm;     // Handle removal from the tail of the list.
  }
  
  /*
  if (lst->destroy != NULL)
    lst->destroy(data);

  memset(pitm, 0, sizeof(sllistItm_t));
  free(pitm);
  */

  --lst->size;
  return 0;
}

bool list_find(const sllist_t *lst, const void *data, sllistItm_t **itm) {
  for (sllistItm_t *p = list_head(lst); p != NULL; p = p->next) {
    if (lst->match(p->data, data)) {
      *itm = p;
      return true;
    }
  }
  // not found
  *itm = NULL;
  return false;  
}

void list_iter(const sllist_t *lst) {
  printf(" - list: (size: %3d) ", list_size(lst));
  //
  for (sllistItm_t *plitm = list_head(lst); plitm != NULL; plitm = plitm->next) {
    lst->cb(list_data(plitm));
  }
  //
  printf("\n");
}
