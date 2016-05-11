#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dllist.h"


typedef struct person_ {
  char *firstname;
  char *lastname;
  unsigned int height_cm;
  double weight_kg;
} person_t;

void person_print(const person_t *p) {
  char *m1  = "- firstname is [%s]\n";
  char *m2  = "- lastname is [%s]\n";
  char *m3  = "- height is [%3d]\n";
  char *m4  = "- weight is [%3.2f]\n\n";
  printf(m1, p->firstname);
  printf(m2, p->lastname);
  printf(m3, p->height_cm);
  printf(m4, p->weight_kg);
  return; 
}

// call back
void cb(const void *pdata) {
  person_t *_p =  (person_t *) pdata;
  printf("[%s:%s] entry \n", __FILE__, __FUNCTION__);
  printf("\t- first name:%s, last name:%s, height:%3d(cm), weight:%3.2f(kg)\n", 
         _p->firstname,
         _p->lastname,
         _p->height_cm,
         _p->weight_kg);
}

// cmp
int person_match(const void *p1, const void *p2) {
  person_t *_p1 = (person_t *) p1;
  person_t *_p2 = (person_t *) p2;
  /*
  printf("--- cmp ---\n");
  person_print(_p1);
  person_print(_p2);
  */
  int res1 = (strncmp(_p1->firstname, _p2->firstname, strlen(_p1->firstname)) == 0) ? 0 : 1;
  int res2 = (strncmp(_p1->lastname, _p2->lastname, strlen(_p1->lastname)) == 0) ? 0 : 1;
  // printf("--- cmp end res: [%d // %d]---\n\n", res1, res2);    
  return res1 + res2; 
}

void test_insert_person(dllist_t *lst, const person_t *p) {
  // insert a person
  printf("[%s:%s] entry \n", __FILE__, __FUNCTION__);
  unsigned int len = dllist_size(lst);
  person_print(p);

  if (! dllist_ins_next(lst, NULL, p)) {
    printf("[!] Error inserting item in the list..\n");
    dllist_free(&lst);
    assert(lst == NULL);
  }
  assert(dllist_size(lst) == len + 1);
  printf("[+] OK list contains one more item - list size is %3d...\n", dllist_size(lst));
  //
  dllitm_t *_itm = NULL;
  assert(dllist_find(lst, p, &_itm));
  assert(person_match(p, dllist_data(_itm)) == 0);
  person_t *_p = (person_t *) dllist_data(_itm);
  //person_print(dllist_data(_itm));
  printf("[+] OK list header is as expected: firstname:[%s] // lastname:[%s] ...\n", 
         _p->firstname,
         _p->lastname);
  return;
}

void test_delete_person(dllist_t *lst, const person_t *p) {
  printf("[%s:%s] entry \n", __FILE__, __FUNCTION__);
  unsigned int len = dllist_size(lst);
  
  if (! dllist_rem(lst, (void **) &p)) {
    printf("[!] Error deleting item from the list..\n");
    dllist_free(&lst);
    assert(lst == NULL);
  }
  assert(dllist_size(lst) == len - 1);
  printf("[+] OK list contains one item less - list size is %3d...\n", dllist_size(lst));
  dllitm_t *_itm = NULL;
  assert(! dllist_find(lst, p, &_itm));
  assert(_itm == NULL);
  printf("[+] OK deleted node contains: firstname:[%s] // lastname:[%s] ...\n", 
         p->firstname,
         p->lastname);
  return;
}

int main(int argc, char **argv) {
  dllist_t *lst = NULL;

  // destroy is NULL
  if (! dllist_new(&lst, &person_match, NULL, &cb)) {
    printf("Error: could not allocate room for a new list\n");
    return(1);
  }
  assert(lst != NULL);

  printf("[+] OK list created...\n");

  assert(dllist_size(lst) == 0);
  printf("[+] OK list created - and size is 0...\n");

  person_t p1 = { .firstname="Pascal", 
                  .lastname="Picard", 
                  .height_cm=173, 
                  .weight_kg=64.5 };
  printf("[+] OK first person created...\n");

  person_t p2 = { .firstname="Masako", 
                  .lastname="Moriyama", 
                  .height_cm=165, 
                  .weight_kg=55.0 };
  printf("[+] OK second person created...\n");
  
  // insert first person
  test_insert_person(lst, &p1);
  test_insert_person(lst, &p2);

  person_t p3 = { .firstname="Ayumi", 
                  .lastname="Moriyama-Picard", 
                  .height_cm=100, 
                  .weight_kg=16.5 };
  printf("[+] OK third person created...\n");
  test_insert_person(lst, &p3);
  //
  dllist_iter(lst);
  //
  test_delete_person(lst, &p1);
  //
  dllist_iter(lst);
  // free
  dllist_free(&lst);
  assert(lst == NULL);
  printf("[+] OK list destroyed...\n");

  return 0;
}

