/*****************************************************************************
*
* -------------------------------- dllist.c -------------------------------- *
*
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "dllist.h"
#include "my_malloc.h"

// the actual data structure

struct dllitm_ {
  void *data;
  struct dllitm_ *next;
  struct dllitm_ *prev;
};

struct dllist_ {
  unsigned int size;
  //
  int  (*match)(const void *k1, const void *k2); // to compare two itmes.
  void (*destroy)(void *data);   // to free the resource taken by one item (client responsibility)
  void (*cb)(const void *data);  // callback - for example: display item list

  dllitm_t *head;
  dllitm_t *tail;
};

/*
 * Convenient macros - WONT WORK !!!! - Need to be in dllist.h - but we use opaque pointer...
 
#define dllist_size(lst) ((lst)->size)

#define dllist_head(lst) ((lst)->head)

#define dllist_tail(lst) ((lst)->tail)

#define dllist_is_head(lst, itm) ((itm) == (lst)->head ? 1 : 0)

#define dllist_is_tail(itm) ((itm)->next == NULL ? 1 : 0)

#define dllist_data(itm) ((itm)->data)

#define dllist_next(itm) ((itm)->next)

#define dllist_prev(itm) ((itm)->prev)

*/

/*
 * Implementation
 */

bool dllist_new(dllist_t **lst, 
             int (*match)(const void *k1, const void *k2), 
             void (*destroy)(void *data),
             void (*cb)(const void *data)) {
  //
  *lst = xmalloc0(sizeof(dllist_t));   // exit with an error if lst is NULL
  //
  (*lst)->size    = 0;
  (*lst)->match   = match;
  (*lst)->destroy = destroy;
  (*lst)->cb      = cb;
  (*lst)->head    = (*lst)->tail = NULL;
  return true;
}

void dllist_free(dllist_t **lst) {
  printf("[%s] entry \n", __FUNCTION__);
  if ((*lst)->destroy != NULL) {
    while ((*lst)->size > 0) {
      printf("[%s] loop over the list item - current size is %d\n", __FILE__, (*lst)->size);
      dllitm_t *p = (*lst)->head;
      (*lst)->head = p->next;
      (*lst)->destroy(p->data); // call the user function
      free(p);
      (*lst)->size--;
    }
  }
  else {  
    while ((*lst)->size > 0) {
      printf("[%s] loop over the list item - current size is %d\n", __FILE__, (*lst)->size);
      dllitm_t *p = (*lst)->head;
      (*lst)->head = p->next;
      free(p);
      (*lst)->size--;
    }
  }
  memset(*lst, 0, sizeof(dllist_t));
  free(*lst);
  *lst = NULL;
  printf("[%s] exit \n", __FUNCTION__);
  return;
}

bool dllist_ins_next(dllist_t *lst, dllitm_t *itm, const void *data) {
  //
  printf("[%s] entry \n", __FUNCTION__);
  dllitm_t *plitm = xmalloc0(sizeof(dllitm_t)); // exit with an error if plitm is NULL
  // OK now, cast to avoid warning: assigning to 'void *' from 'const void *' discards qualifiers
  plitm->data = (void *) data;
  // insertion - 4 cases
  if (lst->size == 0) {
    // (1) empty list, so actually itm is NULL
    plitm->next = NULL; // or  lst->tail;
    plitm->prev = NULL; // lst->head;
    lst->tail = lst->head = plitm;
    printf("[%s] case empty list \n", __FUNCTION__);
  }
  else if (itm == NULL) {
    // (2) dllist_size >0 => list is not empty, but itm == NULL
    //     => insertion at the head
    plitm->next = lst->head;
    plitm->prev = NULL;
    lst->head->prev = plitm; 
    lst->head   = plitm;
    printf("[%s] case insert at head position \n", __FUNCTION__);
  }
  else {
    // (3) and (4)
    plitm->next = itm->next;
    if (itm->next != NULL)  { 
      plitm->prev = itm->next->prev;
      itm->next->prev = plitm;
    }
    else {
      plitm->prev = itm;
    }
    itm->next   = plitm;
    if (plitm->next == NULL) lst->tail = plitm;
  }
  lst->size++;   // insertion OK, increase the size list by 1
  return true;
}

bool dllist_rem_next(dllist_t *lst, dllitm_t *itm, void **data) {
  //
  if (lst->size == 0) {   // empty list
    return false;           
  }
  else if (itm == lst->tail) { // only one item
    assert(lst->size == 1);
    return false;
  }
  else {                  // at least 2 items
    assert(lst->size > 1);
    dllitm_t *del = itm->next;
    itm->next = del->next;    
    if (del->next != NULL) { // del is not the last itm in the list
      del->next->prev = itm;
    }
    else { // del is the last, itm, so tail needs to be changed         
      assert(lst->size == 2);
      lst->tail = itm;
    }
    //
    del->next = del->prev = NULL;
    *data = del->data; // make the data available
    free(del);         // free th space taken by this cell
    lst->size--;
    return true;
  }
 }

bool dllist_rem(dllist_t *lst, void **data) {
  printf("[%s] entry \n", __FUNCTION__);
  if (lst->size == 0) {
    printf("[%s] exit - case empty list\n", __FUNCTION__);
    return false;
  }
  bool found = false;
  dllitm_t *pcur = NULL;
  assert(*data != NULL);
  for(pcur = lst->head; pcur != NULL; pcur = pcur->next) {
    if (lst->match(*data, pcur->data) == 0) {
      found = true;
      break;
    }
  }
  if (found) {
    // 3 cases: head, general, tail
    *data = pcur->data;
    if (pcur == lst->head) {
      lst->head = lst->head->next;
      lst->head->prev = NULL;
    }
    else if (pcur == lst->tail) {
      printf("[%s] exit - general case - tail deletion\n", __FUNCTION__);
      lst->tail = lst->tail->prev;
      lst->tail->next = NULL;
    }
    else {
      pcur->next->prev = pcur->prev;
      pcur->prev->next = pcur->next;
    }
    free(pcur);
    pcur = NULL;
    lst->size--;
    printf("[%s] exit - general case - item located and deleted\n", __FUNCTION__);
    return true;
  }
  else {
    printf("[%s] exit - general case - item not found and therefore not deleted\n", __FUNCTION__);
    return false;
  }
}

bool dllist_find(const dllist_t *lst, const void *data, dllitm_t **itm) {
  *itm = NULL;
  if (lst->size == 0) {
    printf("[%s] exit - case empty list - item NOT found\n", __FUNCTION__);
    return false;
  }
  else if (lst->size == 1) {
    if (lst->match(data, lst->head->data) == 0) {
      *itm = lst->head;
      printf("[%s] exit - case list singleton - item found\n", __FUNCTION__);
      return true;        
    }
    else {
      printf("[%s] exit - case list singleton - item NOT found\n", __FUNCTION__);
      return false;
    }
  } 
  bool found = false;
  for(dllitm_t *pcur = lst->head; pcur != NULL; pcur = pcur->next) {
    if (lst->match(data, pcur->data) == 0) {
      *itm = pcur;
      printf("[%s] exit - general case - item found\n", __FUNCTION__);
      found = true;
      break;        
    }
  }
  return found;
}

void dllist_iter(const dllist_t *lst) {
  if (lst->size == 0) return;
                        
  // for(dllitm_t *pcur = lst->head; pcur->next != NULL; pcur = pcur->next) {
  for(dllitm_t *pcur = lst->head; pcur != NULL; pcur = pcur->next) {
    lst->cb(pcur->data);
  }  
}

unsigned int dllist_size(const dllist_t *lst) {
  return lst->size;
}

dllitm_t *dllist_head(const dllist_t *lst) {
  return lst->head;
}

dllitm_t *dllist_tail(const dllist_t *lst) {
  return lst->tail;
}

void *dllist_data(const dllitm_t *itm) {
  return itm->data;
}

dllitm_t *dllist_next(const dllitm_t *itm) {
  return itm->next;
}

dllitm_t *dllist_prev(const dllitm_t *itm) {
  return itm->prev;
}


