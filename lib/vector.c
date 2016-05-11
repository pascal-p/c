#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void vector_new(vector_t *v, uint_t elemSize, vector_free_fun_t free_fun, uint_t initAlloc) {
  //
  assert(initAlloc > 0 && elemSize > 0);
  //
  v->size       = 0;
  v->free_fun   = free_fun;
  v->chunk_size = initAlloc;
  v->chunk_num  = 0;
  v->elem_size  = elemSize; 
  v->headptr    = NULL;
}
 
void vector_dispose(vector_t *v) {
  //
  // dispose of each element by calling the supplied free_fun (if it is not NULL)
  //  
  if (v->free_fun != NULL) {
    for (uint_t pos = vector_len(v) - 1; pos > 0; pos--) {
      void *elem_addr = vector_nth(v, pos);
      if (elem_addr != NULL) {
        printf(">>> call free fun\n");
        v->free_fun(elem_addr);
      }
    }
    v->size = 0;
  }
  else {
    printf(">>> nothing to do as free function was NULL\n");
  } 
  //  
  // then de-allocate the chunks
  free(v->headptr);
  v->headptr = NULL;
  //
  // then clear the vector_t struct
  memset(v, 0, sizeof(vector_t));
}

uint_t vector_len(const vector_t *v) { 
  return v->size; 
}

void *vector_nth(const vector_t *v, uint_t position) {
  assert(position < v->size);
  return (char *) v->headptr + position * v->elem_size; 
}

void vector_replace(vector_t *v, const void *elemAddr, uint_t position) {
  assert(position < vector_len(v));
  void *p_curr = vector_nth(v, position);
  if (v->free_fun != NULL)
    if (p_curr != NULL)
      v->free_fun(p_curr);
  // now replace
  memcpy(p_curr, elemAddr, v->elem_size); 
  return;
}

void vector_delete(vector_t *v, uint_t position) {
  assert(position < vector_len(v));
  // delete pos
  void *p_pos = vector_nth(v, position);
  if (v->free_fun != NULL && p_pos != NULL)
    v->free_fun(p_pos);
  // now do the left shift
  for (uint_t cpos = position + 1; cpos < vector_len(v); cpos++) {
    void *p_curr = vector_nth(v, cpos);
    memmove(p_pos,
            p_curr,
            v->elem_size);
    p_pos = p_curr;
  }
  v->size--;
  return;
}

static void vector_realloc(vector_t *v) {
  // re-allocation
  v->chunk_num++;
  v->headptr = realloc(v->headptr, v->chunk_num * v->chunk_size * v->elem_size);
  //                                    2       *    4          *    x byte(s)   (e.g.: x == 1 for char) 
  if (v->headptr == NULL) {
    perror("could not allocate space for the vector chunk");
    // Free the initial memory block.
    vector_dispose(v);
    exit(EXIT_FAILURE);
  }
  return;
}

void vector_insert(vector_t *v, const void *elemAddr, uint_t position) {
  printf("[!] vector_insert at position: %d // logical size: %d\n",
         position,
         v->size);
  // check
  assert(position <= v->size);
  //
  if (v->size == 0 || position ==  vector_len(v)) {
    // insert into an empty vector == vector_append
    vector_append(v, elemAddr);
  }
  else {
    assert(position >= 0 && position < vector_len(v));
    //
    // 2 cases - realloc or not
    if (vector_len(v) == v->chunk_num * v->chunk_size)
      vector_realloc(v);
    //
    // incr logical size
    uint_t laspos_t = vector_len(v) - 1; // before incr. the size to allow for the right shift
    v->size++;
    for(uint_t cpos = laspos_t; cpos > position; cpos--) { 
      void *p_curr = vector_nth(v, cpos);
      void *p_dst  = vector_nth(v, cpos + 1);
      memmove(p_dst,
              p_curr,
              v->elem_size);
    }
    // cpos == position, but it did not memmove the item at position 0
    // special case at position == 0
    if (position == 0) {
      void *p_curr = vector_nth(v, position);
      void *p_dst  = vector_nth(v, position + 1);
      memmove(p_dst,
              p_curr,
              v->elem_size);
    }
    // do the insert - get the position for insertion
    void *p_pos  = vector_nth(v, position);
    memcpy(p_pos, elemAddr, v->elem_size);    
  }  
  return;
}

void vector_append(vector_t *v, const void *elemAddr) {
  
  if (vector_len(v) == v->chunk_num * v->chunk_size) { 
    vector_realloc(v);
  }  
  // incr size
  uint_t pos = vector_len(v);
  v->size++;  
  // find the position (last one)
  void *ptr = vector_nth(v, pos);
  if (ptr == NULL) {
    perror("could not find the position");
    vector_dispose(v);
    exit(EXIT_FAILURE);
  }  
  // do the copy
  if (memcpy(ptr, elemAddr, v->elem_size) != ptr) {
    perror("memcpy failed to make the copy\n");
    vector_dispose(v);
    exit(EXIT_FAILURE);
  }
  return;
}

static const int kNotFound = -1;

int vector_search(const vector_t *v, const void *key, vector_cmp_fun_t search_fun, uint_t startIndex, bool isSorted) { 

  /**
     void * bsearch(const void *key, const void *base, size_t nmemb, size_t size, 
                    int (*compar) (const void *, const void *));

     The bsearch() function searches an array of nmemb objects, the initial
     member of which is pointed to by base, for a member that matches the
     object pointed to by key.  The size of each member of the array is speci‐
     fied by size.

     The contents of the array should be in ascending sorted order according
     to the comparison function referenced by compar.  The compar routine is
     expected to have two arguments which point to the key object and to an
     array member, in that order, and should return an integer less than,
     equal to, or greater than zero if the key object is found, respectively,
     to be less than, to match, or be greater than the array member.  See the
     incompare_t sample function in qsort(3) for a comparison function that is
     also compatible with bsearch().

     RETURN VALUES
     The bsearch() function returns a pointer to a matching member of the
     array, or a null pointer if no match is found.  If two members compare as
     equal, which member is matched is unspecified.
  */

  if (v == NULL || startIndex == vector_len(v))
    return kNotFound;
  
  assert(startIndex < vector_len(v)); // always true then...
  assert(search_fun != NULL);
  
  if (isSorted) {
    // binary search by calling bsearch
    void *ptr = bsearch(key, v->headptr, vector_len(v), v->elem_size, search_fun);
    if (ptr == NULL)
       return kNotFound;
    else
      return (uint_t) ((char *)ptr - (char *) v->headptr) / v->elem_size;                
  }
  else {
    // linear search
    printf(">>>=== linear search\n");
    for(uint_t pos = startIndex; pos < vector_len(v); pos++) {
      void *p_curr = (char *) v->headptr + pos * v->elem_size;      
      if (search_fun(p_curr, key) == 0)
        return pos;
    }
    return kNotFound;
  }
} 

void vector_sort(vector_t *v, vector_cmp_fun_t cmp_fun) {
  assert(cmp_fun != NULL);
  /**
   * void qsort(void *base, size_t nmemb, size_t size,
   *             int (*compar)(const void *, const void *));
   *
     The qsort() and heapsort() functions sort an array of nmemb objects, the
     initial member of which is pointed to by base.  The size of each object
     is specified by size.  The mergesort() function behaves similarly, but
     requires that size be greater than “sizeof(void *) / 2”.

     The contents of the array base are sorted in ascending order according to
     a comparison function pointed to by compar, which requires two arguments
     pointing to the objects being compared.

     The comparison function must return an integer less than, equal to, or
     greater than zero if the first argument is considered to be respectively
     less than, equal to, or greater than the second.
   *
   */
  qsort(v->headptr, vector_len(v), v->elem_size, cmp_fun); 
  return;
}

void vector_map(vector_t *v, vector_map_fun_t map_fun, void *auxData) {
  assert(map_fun != NULL);
  //
  for (uint_t pos = 0; pos < vector_len(v); pos++) {
    void *p = vector_nth(v, pos);
    map_fun(p, auxData);
  }
  //
  return;
}



