/* *********************************************************************
 *                                                                     *
 * --------------- Single Linked List Abstract Datatype -------------- *
 *                                                                     *
 * ********************************************************************/

#ifndef SLLIST_H
#define SLLIST_H

#include <stdbool.h>


typedef struct sllitm_ {   // list item 
  void *data;
  struct sllitm_ *next;
} sllistItm_t;

typedef struct sllist_ {
  unsigned int size;
  //
  int  (*match)(const void *k1, const void *k2); // to compare two itmes.
  void (*destroy)(void *data);   // to free the resource taken by one item (client responsibility)
  void (*cb)(const void *data);  // callback - for example: display item list
  //
  sllistItm_t *head;
  sllistItm_t *tail;
   
} sllist_t;


/*
 * returns 0, if allocated and init successfully, <>0 otherwise
 */
int list_new(sllist_t **lst, 
             int (*match)(const void *k1, const void *k2), 
             void (*destroy)(void *data),
             void (*cb)(const void *data));

/*
 * Already allocated but not yet initialize (which is mainly init: size/head/tail and functions)
 */
void list_init(sllist_t *lst, 
               int (*match)(const void *k1, const void *k2), 
               void (*destroy)(void *data),
               void (*cb)(const void *data));


void list_free(sllist_t **lst);


/*
 * insert data into lst after element pointed to by itm 
 * if itm == NULL, then insert at the head of the list
 *
 * return 0 if insert successful, any <>0 in case of failure
 */
int list_ins_next(sllist_t *lst, sllistItm_t *itm, const void *data);


/*
 * remove item pointed to by data and next to itm, set it to data (return 0 if success)
 * Complexity is O(1)
 */
int list_rem_next(sllist_t *lst, sllistItm_t *itm, void **data);


/*
 * remove item pointed to by data (return 0 if success)
 * Complexity is O(n)
 */
int list_rem(sllist_t *lst, void **data);


/*
 * find/locate data into list lst
 * if found return 0 and set *itm as a pointer to the cell containing 
 * data, <>0 if data is not in the list
 */
bool list_find(const sllist_t *lst, const void *data, sllistItm_t **itm);


/*
 * iterate through each item (calling the callback function (cb)) of the given list 
 */
void list_iter(const sllist_t *lst);


/*
 * Convenient macros 
 */

#define list_size(lst) ((lst)->size)

#define list_head(lst) ((lst)->head)

#define list_tail(lst) ((lst)->tail)

#define list_is_head(lst, itm) ((itm) == (lst)->head ? 1 : 0)

#define list_is_tail(itm) ((itm)->next == NULL ? 1 : 0)

#define list_data(itm) ((itm)->data)

#define list_next(itm) ((itm)->next)

#endif
