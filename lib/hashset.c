#include "hashset.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void hashset_new(hashset_t *h, int elemSize, int numBuckets,
                 hashset_hash_fun_t hashfn, 
                 hashset_cmp_fun_t cmpfn, 
                 hashset_free_fun_t freefn) {
  //
  assert(elemSize > 0 && numBuckets >0);
  assert(hashfn != NULL && cmpfn != NULL);
  //
  h->hash_fun    = hashfn;
  h->cmp_fun     = cmpfn;
  h->free_fun    = freefn;  
  h->count       = 0;
  h->num_buckets = numBuckets;
  h->elem_size   = elemSize;
  h->chunk_size  = 4;        // how many element(s) to store in vector (initially)
  //
  // need to allocate room for h->num_buckets of type vector_t *
  h->bucket_lst  = (vector_t *) calloc(h->num_buckets, sizeof(vector_t *)); 
  if (h->bucket_lst == NULL) {
    perror("could not allocate memory for the hashset_t");
    exit(EXIT_FAILURE);
  }
  for(uint_t ix_bucket = 0; ix_bucket < h->num_buckets; ix_bucket++) {
    vector_t *v = &h->bucket_lst[ix_bucket];
    vector_new(v, h->elem_size, h->free_fun, h->chunk_size);
    assert(v != NULL);
  }
}

void hashset_dispose(hashset_t *h) {
  assert(h != NULL);
  for(uint_t ix_bucket = 0; ix_bucket < h->num_buckets; ix_bucket++) {
    vector_dispose(&h->bucket_lst[ix_bucket]);
  }
  // free the bucket list (which is a dynamic array)
  free(h->bucket_lst);
  h->bucket_lst = NULL;
  // then clear the hashset_t struct
  memset(h, 0, sizeof(hashset_t));
}

uint_t hashset_count(const hashset_t *h) { 
  return h->count; 
}

void hashset_enter(hashset_t *h, const void *elemAddr) {
  // (1) check
  assert(elemAddr != NULL);
  //
  // (2) compute the hash key == bucket_num
  int bucket_num = h->hash_fun(elemAddr, h->num_buckets);
  assert(bucket_num >= 0 && bucket_num < h->num_buckets);
  //
  // (3) 2 cases: h->bucket_lst[bucket_num] == NULL || h->bucket_lst[bucket_num]  defined
  vector_t *v = &h->bucket_lst[bucket_num];  
  /**
  if (v == NULL) {
    // initialize a vector_t
    vector_new(v, h->elem_size, h->free_fun, h->chunk_size);
    assert(v != NULL);
  }
  */
  //
  // (4) lookup elemAddr in this vector
  int ix = vector_search(v, elemAddr, h->cmp_fun, 0, false);
  //
  // 
  if (ix == -1) {
    vector_append(v, elemAddr);
    h->count++;
  }
  else {
    // free that position and ...
    vector_delete(v, ix); 
    // ... do the insert
    vector_insert(v, elemAddr, ix);
    // which means that h->count remains the same
  }
}

void *hashset_lookup(const hashset_t *h, const void *elemAddr) { 
  // (1) check
  assert(elemAddr != NULL);
  //
  // (2) compute the hash key == bucket_num
  int bucket_num = h->hash_fun(elemAddr, h->num_buckets);
  assert(bucket_num >= 0 && bucket_num < h->num_buckets);  
  //
  // (3)
  vector_t *v = &h->bucket_lst[bucket_num];  
  if (v == NULL) 
    return v;
  //
  // now v != NULL
  int ix = vector_search(v, elemAddr, h->cmp_fun, 0, false);
  if (ix == -1) {
    return NULL;
  }
  else {
    return vector_nth(v, ix);
  }
}

void hashset_map(hashset_t *h, hashset_map_fun_t mapfn, void *auxData) {
  // (1) check
  assert(mapfn != NULL);
  //
  // (2)
  for(uint_t ix_bucket = 0; ix_bucket < h->num_buckets; ix_bucket++) {
    vector_map(&h->bucket_lst[ix_bucket], mapfn, auxData); 
  }
}

