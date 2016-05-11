#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "sllist.h"

typedef struct {
  t_SLList *lst;
} t_lfixture;



int i_match(const void *p1, const void *p2) {
  return (*(int *)p1 == *(int *)p2) ? 0 : 1;
}

void list_setup(t_lfixture *lf, gconstpointer test_data) {
  list_new(&lf->lst, &i_match, NULL);

  if (lf->lst == NULL) {
    printf("Error: could not allocate room for a new list\n");
    exit(1);
  }

  int i = 3;
  int j = 6;
  list_ins_next(lf->lst, NULL, &i);
  list_ins_next(lf->lst, list_head(lf->lst), &j);
}

void list_teardown(t_lfixture *lf, gconstpointer test_data) {
  list_free(&lf->lst);
}

void test_list_size(t_lfixture *lf,  gconstpointer ignored) {
  g_assert_cmpint(2, ==, list_size(lf->lst));
}

void test_list_head(t_lfixture *lf,  gconstpointer ignored) {
  g_assert_cmpint(3, ==, *(int *) list_data(list_head(lf->lst)));
}

void test_list_tail(t_lfixture *lf,  gconstpointer ignored) {
  g_assert_cmpint(6, ==, *(int *) list_data(list_tail(lf->lst)));
}

void test_list_add_head(t_lfixture *lf,  gconstpointer ignored) {
  int k = 5;
  int s = list_size(lf->lst);
  list_ins_next(lf->lst, NULL, &k);
  g_assert_cmpint((s+1), ==, list_size(lf->lst));
  g_assert_cmpint(k, ==, *(int *) list_data(list_head(lf->lst)));
}

void test_list_add_tail(t_lfixture *lf,  gconstpointer ignored) {
  int k = -10;
  int s = list_size(lf->lst);
  list_ins_next(lf->lst, list_tail(lf->lst), &k);
  g_assert_cmpint((s+1), ==, list_size(lf->lst));
  g_assert_cmpint(k, ==, *(int *) list_data(list_tail(lf->lst)));
}

void test_list_rem_head1(t_lfixture *lf,  gconstpointer ignored) {
  int k = 3;
  int s = list_size(lf->lst);
  int n = *(int *) list_data(list_tail(lf->lst));
  if (list_rem(lf->lst, &k) == 0) {
    g_assert_cmpint((s-1), ==, list_size(lf->lst));
    g_assert_cmpint(n, ==, *(int *) list_data(list_head(lf->lst)));
  }
}

void test_list_rem_tail1(t_lfixture *lf,  gconstpointer ignored) {
  int k = 6;
  int s = list_size(lf->lst);
  int n = *(int *)list_data(list_head(lf->lst));
  if (list_rem(lf->lst, &k) == 0) {
    g_assert_cmpint((s-1), ==, list_size(lf->lst));
    g_assert_cmpint(n, ==, *(int *) list_data(list_tail(lf->lst)));
  }
}

void test_list_rem_head(t_lfixture *lf,  gconstpointer ignored) {
  int *pk;
  int s = list_size(lf->lst);
  int n = *(int *) list_data(list_tail(lf->lst));
  if (list_rem_next(lf->lst, NULL, (void **) &pk) == 0) {
    g_assert_cmpint((s-1), ==, list_size(lf->lst));
    g_assert_cmpint(n, ==, *(int *) list_data(list_head(lf->lst)));
  }
}

void test_list_rem_tail(t_lfixture *lf,  gconstpointer ignored) {
  int *pk;
  int s = list_size(lf->lst);
  int n = *(int *)list_data(list_head(lf->lst));
  if (list_rem_next(lf->lst, list_head(lf->lst), (void **) &pk) == 0) {
    g_assert_cmpint((s-1), ==, list_size(lf->lst));
    g_assert_cmpint(n, ==, *(int *) list_data(list_tail(lf->lst)));
  }
}
void test_list_empty(t_lfixture *lf,  gconstpointer ignored) {
  int *pk;
  int s = list_size(lf->lst);
  //
  if (list_rem_next(lf->lst, list_head(lf->lst), (void **) &pk) == 0) {
    if (list_rem_next(lf->lst, list_head(lf->lst), (void **) &pk) == 0) {
      g_assert_cmpint((s-2), ==, list_size(lf->lst));
      g_assert( NULL == list_head(lf->lst));
      g_assert( NULL == list_tail(lf->lst));
    }
  }
}




int main(int argc, char **argv) {
  g_test_init(&argc, &argv, NULL);
  //
  g_test_add("/home/pascal/progr/C test list size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_size, 
             list_teardown);
  g_test_add("/home/pascal/progr/C test list head", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_head, 
             list_teardown);
  g_test_add("/home/pascal/progr/C test list tail", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_tail, 
             list_teardown);
  g_test_add("/home/pascal/progr/C test insert head + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_add_head,
             list_teardown);
  g_test_add("/home/pascal/progr/C test insert tail + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_add_tail,
             list_teardown);  
  g_test_add("/home/pascal/progr/C test rem head + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_rem_head1,
             list_teardown); 
   g_test_add("/home/pascal/progr/C test rem tail + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_rem_tail1,
             list_teardown); 
  g_test_add("/home/pascal/progr/C test rem after head + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_rem_head,
             list_teardown); 
   g_test_add("/home/pascal/progr/C test rem before tail + size", 
             t_lfixture, 
             NULL,
             list_setup, 
             test_list_rem_tail,
             list_teardown); 
   g_test_add("/home/pascal/progr/C test empty list", 
              t_lfixture, 
              NULL,
              list_setup, 
              test_list_empty,
              list_teardown);   
   //
  return g_test_run();  
}
