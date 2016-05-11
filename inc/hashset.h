#ifndef _hashset_
#define _hashset_
#include "vector.h"

/* File: hashtable.h
 * ------------------
 * Defines the interface for the hashset.
 */

/**
 * Type: hashset_hash_fun_t
 * -------------------------
 * Class of function designed to map the figure at the specied
 * elemAddr to some number (the hash code) between 0 and numBuckets - 1.
 * The hashing routine must be stable in that the same number must
 * be returned every single time the same element (where same is defined
 * in the hashset_cmp_fun_t sense) is hashed.  Ideally, the
 * hash routine would manage to distribute the spectrum of client elements
 * as uniformly over the [0, numBuckets) range as possible.
 */

typedef int (*hashset_hash_fun_t)(const void *elemAddr, int numBuckets);

/**
 * Type: hashset_cmp_fun_t
 * ----------------------------
 * Class of function designed to compare two elements, each identified
 * by address.  The hashset_cmp_fun_t compares these elements and
 * decides whether or not they are logically equal or or.  The
 * return value identifies relative ordering:
 * 
 *   - A negative return value means that the item addressed by elemAddr1
 *     is less than the item addressed by elemAddr2.  The definition of
 *     'less than' is dictated by the implementation's interpretation of the data.
 *   - A zero return value means that the two items addressed by elemAddr1
 *     and elemAddr2 are equal as far as the comparison routine is concerned.
 *   - A positive return value means that the item addressed by elemAddr2
 *     is less that the item addressed by elemAddr1.
 */

typedef int (*hashset_cmp_fun_t)(const void *elemAddr1, const void *elemAddr2);

/**
 * Type: hashset_map_fun_t
 * ------------------------
 * Class of function that can me mapped over the elements stored in a hashset_t.
 * These map functions accept a pointer to a client element and a pointer
 * to a piece of auxiliary data passed in as the second argument to hashset_map.
 */

typedef void (*hashset_map_fun_t)(void *elemAddr, void *auxData);

/**
 * Type: hashset_free_fun_t
 * -------------------------
 * Class of functions designed to dispose of and/or clean up
 * any resources embedded within the element at the specified
 * address.  Typically, such resources are dynamically allocated
 * memory, open file pointers, and data structures requiring that
 * some specific cleanup routine be called.
 */

typedef void (*hashset_free_fun_t)(void *elemAddr);

/**
 * Type: hashset_t
 * -------------
 * The concrete representation of the hashset_t.
 * In spite of all of the fields being publicly accessible, the
 * client is absolutely required to initialize, dispose of, and
 * otherwise interact with all hashset_t instances via the suite
 * of the six hashset-related functions described below.
 */

typedef struct {
  hashset_hash_fun_t  hash_fun;
  hashset_cmp_fun_t   cmp_fun;
  hashset_free_fun_t  free_fun;  
  //
  uint_t   num_buckets;     // num. of buckets in the hashset_t
  uint_t   count;           // num. of element in a hashset_t
  usint_t  elem_size;       // size of an element
  uint_t   chunk_size;      // how many element(s) to store in vector (initially)
  vector_t *bucket_lst;     // array of num_buckets element
  //
} hashset_t;

/**
 * Function:  hashset_new
 * ---------------------
 * Initializes the identified hashset_t to be empty.  It is assumed that
 * the specified hashset_t is either raw memory or one previously initialized
 * but later destroyed (using HastSetDispose.)
 * 
 * The elemSize  parameter specifies the number of bytes that a single element of
 * the table should take up. For example, if you want to store elements of type 
 * Binky, you would pass sizeof(Binky) as this parameter. An assert is
 * raised if this size is less than or equal to 0.
 *
 * The numBuckets parameter specifies the number of buckets that the elements
 * will be partitioned into.  Once a hashset_t is created, this number does
 * not change.  The numBuckets parameter must be in sync with the behavior of
 * the hashfn, which must return a hash code between 0 and numBuckets - 1.   
 * The hashfn parameter specifies the function that is called to retrieve the
 * hash code for a given element.  See the type declaration of hashset_hash_fun_t
 * above for more information.  An assert is raised if numBuckets is less than or
 * equal to 0.
 *
 * The cmpfn is used for testing equality between elements.  See the
 * type declaration for hashset_cmp_fun_t above for more information.
 *
 * The freefn is the function that will be called on an element that is
 * about to be overwritten (by a new entry in hashset_enter) or on each element 
 * in the table when the entire table is being freed (using hashset_dispose).  This 
 * function is your chance to do any deallocation/cleanup required,
 * (such as freeing any pointers contained in the element). The client can pass 
 * NULL for the freefn if the elements don't require any handling. 
 * An assert is raised if either the hash or compare functions are NULL.
 *
 * An assert is raised unless all of the following conditions are met:
 *    - elemSize is greater than 0.
 *    - numBuckets is greater than 0.
 *    - hashfn is non-NULL
 *    - cmpfn is non-NULL
 */

void hashset_new(hashset_t *h, int elemSize, int numBuckets, 
		hashset_hash_fun_t hashfn, hashset_cmp_fun_t cmpfn, hashset_free_fun_t freefn);

/**
 * Function: hashset_dispose
 * ------------------------
 * Disposes of any resources acquired during the lifetime of the
 * hashset_t.  It does not dispose of client elements properly unless the
 * hashset_free_fun_t specified at construction time does the right
 * thing.  hashset_dispose will apply this cleanup routine to all
 * of the client elements stored within.
 *
 * Once hashset_dispose has been called, the hashset_t is rendered
 * useless.  The diposed of hashset_t should not be passed to any
 * other hashset_t routines (unless for some reason you want to reuse
 * the same hashset_t variable and re-initialize is by passing it to
 * hashset_new... not recommended.)
 */

void hashset_dispose(hashset_t *h);

/**
 * Function: hashset_count
 * ----------------------
 * Returns the number of elements residing in 
 * the specified hashset_t.
 */

uint_t hashset_count(const hashset_t *h);

/**
 * Function: hashset_enter
 * ----------------------
 * Inserts the specified element into the specified hashset_t.  
 # If the specified element matches an element previously 
 * inserted (SAME POS. as far as the hash and compare functions are 
 * concerned), then the old element is replaced by this new element
 *
 * An assert is raised if the specified address is NULL, or
 * if the embedded hash function somehow computes a hash code
 * for the element that is out of the [0, numBuckets) range.
 */

void hashset_enter(hashset_t *h, const void *elemAddr);

/**
 * Function: hashset_lookup
 * -----------------------
 * Examines the specified hashset_t to see if anything matches
 * the item residing at the specified elemAddr.  If a match
 * is found, then the address of the stored item is returned.
 * If no match is found, then NULL is returned as a sentinel.
 * Understand that the key (residing at elemAddr) only needs
 * to match a stored element as far as the hash and compare
 * functions are concerned.
 *
 * An assert is raised if the specified address is NULL, or
 * if the embedded hash function somehow computes a hash code
 * for the element that is out of the [0, numBuckets) range.
 */

void *hashset_lookup(const hashset_t *h, const void *elemAddr);

/**
 * Function: hashset_map
 * --------------------
 * Iterates over all of the stored elements, applying the specified
 * mapfn to the addresses of each.  The auxData parameter can be
 * used to propagate additional data to each of the mapfn calls; in
 * fact, the auxData value is passed in as the second argument to
 * each mapfn application.  It is the responsibility of the mapping
 * function to received that auxData and handle it accordingly.  NULL
 * should be passed in as the auxData if (and only if) the mapping
 * function itself ignores its second parameter.
 *
 * An assert is raised if the mapping routine is NULL.
 */

void hashset_map(hashset_t *h, hashset_map_fun_t mapfn, void *auxData);
     
#endif
