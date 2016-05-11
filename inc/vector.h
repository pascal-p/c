/**
 * File: vector.h
 * ---------------
 * Defines the interface for the vector_t.
 *
 * The vector_t allows the client to store any number of elements of any desired
 * primitive type and is appropriate for a wide variety of storage problems.  It 
 * supports efficient element access and appending/inserting/deleting elements
 * as well as optional sorting and searching.  In all cases, the vector_t imposes 
 * no upper bound on the number of elements and deals with all its own memory 
 * management. The client specifies the size (in bytes) of the elements that 
 * will be stored in the vector_t when it is created.  Thereafter the client and 
 * the vector_t can refer to elements via (void *) ptrs.
 */

#ifndef _vector_
#define _vector_

#include "bool.h"

typedef unsigned int uint_t;
typedef unsigned short int usint_t;

/**
 * Type: vector_cmp_fun_t
 * ---------------------------
 * vector_cmp_fun_t is a pointer to a client-supplied function which the
 * vector_t uses to sort or search for elements.  The comparator takes two 
 * (const void *) pointers (these will point to elements) and returns an int.
 * The comparator should indicate the ordering of the two elements
 * using the same convention as the strcmp library function:
 * 
 *   If elemAddr1 is less than elemAddr2, return a negative number.
 *   If elemAddr1 is greater than elemAddr2, return a positive number.
 *   If the two elements are equal, return 0.
 */

typedef int (*vector_cmp_fun_t)(const void *elemAddr1, const void *elemAddr2);

/**
 * Type: vector_map_fun_t
 * ---------------------------
 * vector_map_fun_t defines the space of functions that can be used to map over
 * the elements in a vector_t.  A map function is called with a pointer to
 * the element and a client data pointer passed in from the original
 * caller.
 *
 */

typedef void (*vector_map_fun_t)(void *elemAddr, void *auxData);

/**
 * Type: vector_free_fun_t
 * ---------------------------------
 * vector_free_fun_t defines the space of functions that can be used as the
 * clean-up function for each element as it is deleted from the vector_t
 * or when the entire vector_t is destroyed.  The cleanup function is 
 * called with a pointer to an element about to be deleted.
 */

typedef void (*vector_free_fun_t)(void *elemAddr);

/**
 * Type: vector_t
 * ------------
 * Defines the concrete representation of
 * the vector_t.  Even though everything is
 * exposed, the client should respect the 
 * the privacy of the representation and initialize,
 * dispose of, and otherwise interact with a
 * vector_t using those functions defined in this file.
 */

typedef struct vector_ {
  vector_free_fun_t free_fun;
  //
  uint_t size;          // how many item/element are in the current vector
  uint_t chunk_size;    // size of chunk to allocate/de-allocate  - example 4 units
  usint_t chunk_num;    // number of chunk(s) currently allocated - example 1 at the beginning
  usint_t elem_size;    // size of an element                     - example size of a fraction, if we store fractions  
  //
  void *headptr;        // pointer to first chunk of data
} vector_t;

/**
 * Function: vector_new
 * Usage: vector_t myFriends;
 *        vector_new(&myFriends, sizeof(char *), StringFree, 10);
 * -------------------
 * Constructs a raw or previously destroyed vector_t to be the
 * empty vector_t.
 * 
 * The elemSize parameter specifies the number of bytes that a single 
 * element of the vector_t should take up.  For example, if you want to store 
 * elements of type char *, you would pass sizeof(char *) as this parameter.
 * An assert is raised if the size is not greater than zero.
 *
 * The free_fun is the function that will be called on an element that
 * is about to be deleted (using vector_delete) or on each element in the
 * vector_t when the entire vector_t is being freed (using vector_dispose).  This function
 * is your chance to do any deallocation/cleanup required for the element
 * (such as freeing/deleting any pointers contained in the element). The client can pass 
 * NULL for the free_fun if the elements don't require any special handling.
 *
 * The initAlloc parameter specifies the initial allocated length 
 * of the vector_t, as well as the dynamic reallocation increment for those times when the 
 * vector_t needs to grow.  Rather than growing the vector_t one element at a time as 
 * elements are added (inefficient), you will grow the vector_t 
 * in chunks of initAlloc size.  The allocated length is the number
 * of elements for which space has been allocated: the logical length 
 * is the number of those slots currently being used.
 * 
 * A new vector_t pre-allocates space for initAlloc elements, but the
 * logical length is zero.  As elements are added, those allocated slots fill
 * up, and when the initial allocation is all used, grow the vector_t by another 
 * initAlloc elements.  You will continue growing the vector_t in chunks
 * like this as needed.  Thus the allocated length will always be a multiple
 * of initAlloc.  Don't worry about using realloc to shrink the vector_t's 
 * allocation if a bunch of elements get deleted.  It turns out that 
 * many implementations of realloc don't even pay attention to such a request, 
 * so there is little point in asking.  Just leave the vector_t over-allocated and no
 * one will care.
 *
 * The initAlloc is the client's opportunity to tune the resizing
 * behavior for his/her particular needs.  Clients who expect their vectors to
 * become large should probably choose a large initial allocation size, whereas
 * clients who expect the vector_t to be relatively small should choose a smaller
 * initAlloc size.  You want to minimize the number of reallocations, but
 * you don't want to pre-allocate all that much memory if you don't expect to use very
 * much of it.  If the client passes 0 for initAlloc, the implementation
 * will use the default value of its own choosing.  As assert is raised is 
 * the initAlloc value is less than 0.
 */

void vector_new(vector_t *v, uint_t elemSize, vector_free_fun_t free_fun, uint_t initAlloc);

/**
 * Function: vector_dispose
 *           vector_dispose(&studentsDroppingTheCourse);
 * -----------------------
 * Frees up all the memory of the specified vector_t and its elements.  It does *not* 
 * automatically free memory owned by pointers embedded in the elements. 
 * This would require knowledge of the structure of the elements, which the 
 * vector_t does not have.  However, it *will* iterate over the elements calling
 * the vector_free_fun_t previously supplied to vector_new.
 */

void vector_dispose(vector_t *v);

/**
 * Function: vector_len
 * ----------------------
 * Returns the logical length of the vector_t, i.e. the number of elements
 * currently in the vector_t.  Must run in constant time.
 */

uint_t vector_len(const vector_t *v);
	   
/**
 * Method: vector_nth
 * -----------------
 * Returns a pointer to the element numbered position in the vector_t.  
 * Numbering begins with 0.  An assert is raised if n is less than 0 or greater 
 * than the logical length minus 1.  Note this function returns a pointer into 
 * the vector's storage, so the pointer should be used with care.
 * This method must operate in constant time.
 *
 * We could have written the vector_t without this sort of access, but it
 * is useful and efficient to offer it, although the client needs to be 
 * careful when using it.  In particular, a pointer returned by vector_nth 
 * becomes invalid after any calls which involve insertion into, deletion from or 
 * sorting of the vector_t, as all of these may rearrange the elements to some extent.
 */ 

void *vector_nth(const vector_t *v, uint_t position);
					  
/**
 * Function: vector_insert
 * ----------------------
 * Inserts a new element into the specified vector_t, placing it at the specified position.
 * An assert is raised if position is less than 0 or greater than the logical length.
 * The vector_t elements after the supplied position will be shifted over to make room. 
 * The element is passed by address: The new element's contents are copied from 
 * the memory pointed to by elemAddr.  This method runs in linear time.
 */

void vector_insert(vector_t *v, const void *elemAddr, uint_t position);

/**
 * Function: vector_append
 * ----------------------
 * Appends a new element to the end of the specified vector_t.  The element is 
 * passed by address, and the element contents are copied from the memory pointed 
 * to by elemAddr.  Note that right after this call, the new element will be 
 * the last in the vector_t; i.e. its element number will be the logical length 
 * minus 1.  This method must run in constant time (neglecting the memory reallocation 
 * time which may be required occasionally).
 */

void vector_append(vector_t *v, const void *elemAddr);
  
/**
 * Function: vector_replace
 * -----------------------
 * Overwrites the element at the specified position with a new value.  Before 
 * being overwritten, the vector_free_fun_t that was supplied to vector_new is levied
 * against the old element.  Then that position in the vector_t will get a new value by
 * copying the new element's contents from the memory pointed to by elemAddr.
 * An assert is raised if n is less than 0 or greater than the logical length 
 * minus one.  None of the other elements are affected or rearranged by this
 * operation, and the size of the vector_t remains constant. This method must
 * operate in constant time.
 */

void vector_replace(vector_t *v, const void *elemAddr, uint_t position);

/**
 * Function: vector_delete
 * ----------------------
 * Deletes the element at the specified position from the vector_t. Before the 
 * element is removed,  the ArrayFreeFunction that was supplied to vector_new
 * will be called on the element.
 *
 * An assert is raised if position is less than 0 or greater than the logical length 
 * minus one.  All the elements after the specified position will be shifted over to fill 
 * the gap.  This method runs in linear time.  It does not shrink the 
 * allocated size of the vector_t when an element is deleted; the vector_t just 
 * stays over-allocated.
 */

void vector_delete(vector_t *v, uint_t position);
  
/**
 * Function: vector_search
 * ----------------------
 * Searches the specified vector_t for an element whose contents match
 * the element passed as the key.  Uses the comparator argument to test
 * for equality.  The startIndex parameter controls where the search
 * starts.  If the client desires to search the entire vector_t,
 * they should pass 0 as the startIndex.  The method will search from
 * there to the end of the vector_t.  The isSorted parameter allows the client 
 * to specify that the vector_t is already in sorted order, in which case vector_search
 * uses a faster binary search.  If isSorted is false, a simple linear search is 
 * used.  If a match is found, the position of the matching element is returned;
 * else the function returns -1.  Calling this function does not 
 * re-arrange or change contents of the vector_t or modify the key in any way.
 * 
 * An assert is raised if startIndex is less than 0 or greater than
 * the logical length (although searching from logical length will never
 * find anything, allowing this case means you can search an entirely empty
 * vector_t from 0 without getting an assert).  An assert is raised if the
 * comparator or the key is NULL.
 */  

int vector_search(const vector_t *v, const void *key, vector_cmp_fun_t search_fun, uint_t startIndex, bool isSorted);

/**
 * Function: vector_sort
 * --------------------
 * Sorts the vector_t into ascending order according to the supplied
 * comparator.  The numbering of the elements will change to reflect the 
 * new ordering.  An assert is raised if the comparator is NULL.
 */

void vector_sort(vector_t *v, vector_cmp_fun_t cmp_fun);

/**
 * Method: vector_map
 * -----------------
 * Iterates over the elements of the vector_t in order (from element 0 to
 * element n-1, inclusive) and calls the function map_fun on each element.  The function is 
 * called with the address of the vector_t element and the auxData pointer.  
 * The auxData value allows the client to pass extra state information to 
 * the client-supplied function, if necessary.  If no client data is required, 
 * this argument should be NULL.  
 * An assert is raised if the map_fun function is NULL.
 */

void vector_map(vector_t *v, vector_map_fun_t map_fun, void *auxData);

#endif
