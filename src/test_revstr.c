#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ASSERT
// #define TRACE


static char * _alloc(unsigned int len) {
  char *rstr = calloc(len, 1);  // note size of char is 1

  if (rstr == NULL) {
    perror("Could NOT allocate space for the new string");
    exit(1);
  }
 
  return rstr;
}

char *rev_simple(char *pstr) {
  unsigned int len = strlen(pstr);
  char *rstr =  _alloc(len);

  if (len == 0)
    return rstr;
  
  char *pend = pstr + len - 1;
  char *p    = rstr;

  unsigned int l = len;
  do {
    *p++ = *pend--;
  } while (--l > 0); 
  
  *p = '\0';
  return rstr;
}


void test_rev_simple(char *c_str) {
  char *r_str = rev_simple(c_str);
  //
  printf("initial string of len [%3d]: [%s]\n", (unsigned) strlen(c_str), c_str);
  if (r_str != NULL) {
    printf("reverse[1] string of len [%3d]: [%s]\n", (unsigned) strlen(r_str), r_str);
    free(r_str);
  }
  /*
  char *r_str2 = rev_eff(c_str);
  if (r_str2 != NULL) {
    printf("reverse[2] string of len [%3d]: [%s]\n", (unsigned) strlen(r_str2), r_str2);
    free(r_str2);
  }
  */
  return;
}


int main(int argc, char **argv) {
  char *c_str = "A";
  test_rev_simple(c_str);
  return 0;
}
