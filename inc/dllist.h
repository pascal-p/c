/* *********************************************************************
 *                                                                     *
 * --------------- Double Linked List Abstract Datatype -------------- *
 *                                                                     *
 * ********************************************************************/

#ifndef DLLIST_H
#define DLLIST_H

#include <stdbool.h>

// Using Opaque pointers

typedef struct dllitm_ dllitm_t;

typedef struct dllist_ dllist_t;


/*
 * returns true, if allocated and init successfully, <>0 otherwise
 */
bool dllist_new(dllist_t **lst, 
             int (*match)(const void *k1, const void *k2), 
             void (*destroy)(void *data),
             void (*cb)(const void *data));


void dllist_free(dllist_t **lst);

/*
 * insert data into list after element pointed to by itm 
 * if itm == NULL, then insert at the head of the list
 *
 * to insert at the tail just set itm to tail
 *
 * returns true if insert successful, any <>0 in case of failure
 */
bool dllist_ins_next(dllist_t *lst, dllitm_t *itm, const void *data);

/*
 * remove item pointed to by data and next to itm, set it to data (returns true if success)
 * Complexity is O(1)
 */
bool dllist_rem_next(dllist_t *lst, dllitm_t *itm, void **data);


/*
 * remove item pointed to by data (returns true if success)
 * imply walking the list to locate the element
 * Complexity is O(n)
 */
bool dllist_rem(dllist_t *lst, void **data);


/*
 * find/lookup for data into the list
 * if found returns true and set *itm as a pointer to the cell containing 
 * data, otherwise false 
 * fins and lookup are aliases 
 *
 * Complexity is O(n)
 */
bool dllist_find(const dllist_t *lst, const void *data, dllitm_t **itm);

#define dllist_lookup(lst, data, itm) dllist_find((lst), (data), (itm))


/*
 * iterate through each item (calling the callback function (cb)) of the given list 
 */
void dllist_iter(const dllist_t *lst);


// convenience functions

unsigned int dllist_size(const dllist_t *lst);

dllitm_t *dllist_head(const dllist_t *lst);

dllitm_t *dllist_tail(const dllist_t *lst);

void *dllist_data(const dllitm_t *itm);

dllitm_t *dllist_next(const dllitm_t *itm);

dllitm_t *dllist_prev(const dllitm_t *itm);

#endif
