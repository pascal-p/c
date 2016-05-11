#include "my_malloc.h"


void *xmalloc(size_t sz) {
  void *ptr = malloc(sz);

  if (!ptr) {
    perror("xmalloc0");
    exit(EXIT_FAILURE);
  }
  else {
    return ptr;
  }

}


void *xmalloc0(size_t sz) {
  void *ptr = calloc(1, sz);

  if (!ptr) {
    perror("xmalloc0");
    exit(EXIT_FAILURE);
  }
  else {
    return ptr;
  }
}
