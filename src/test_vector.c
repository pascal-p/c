#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <assert.h>

#define YES_OR_NO(value) (value != 0 ? "Yes" : "No")

/**
 * print_char
 * ---------
 * Mapping function used to print one character element in a vector.  
 * The file pointer is passed as the client data, so that it can be
 * used to print to any FILE *.
 */

static void print_char(void *elem, void *fp)
{
  fprintf((FILE *)fp, "%c ", *(char *)elem);
  fflush((FILE *)fp);
}

/**
 * cmp_char
 * -----------
 * Comparator function used to compare two character elements within a vector.
 * Used for both sorting and searching in the array of characters.  Has the same
 * return value semantics as the strcmp library function (negative if A<B, zero if A==B,
 * positive if A>B).
 */

static int cmp_char(const void *elemA, const void *elemB)
{
  return (*(char *)elemA - *(char *)elemB);
}

/**
 * Function: test_append
 * --------------------
 * Appends letters of alphabet in order, then appends a few digit chars.
 * Uses vector_map to print the vector_t contents before and after.
 */

static void test_append(vector_t *alphabet)
{
  char ch;
  int i;
  
  for (ch = 'A'; ch <= 'Z'; ch++) {  //  Start with letters of alphabet
    fprintf(stdout, "let's insert %c\n", ch);
    vector_append(alphabet, &ch);
    vector_map(alphabet, print_char, stdout);
    fprintf(stdout, "\n");
  }
  fprintf(stdout, "First, here is the alphabet: ");
  vector_map(alphabet, print_char, stdout);
  fprintf(stdout, "\n");
  
  for (i = 0; i < 10; i++) {	    // Append digit characters
    ch = '0' + i;                   // convert int to ASCII digit character
    vector_append(alphabet, &ch);
  }
  fprintf(stdout, "\n_After_ append digits: ");
  vector_map(alphabet, print_char, stdout);
  fprintf(stdout, "\n");
}


/**
 * Function: test_search
 * --------------------
 * Tests the searching capability of the vector_t by looking for specific
 * character.  Calls vector_search twice, once to see if it finds the character
 * using a binary search (given the array is sorted) and once to see if it
 * finds the character using a linear search.  Reports results to stdout.
 */

static void test_search(vector_t *v, char ch)
{
  int found_sorted, found_not;
  
  found_sorted = vector_search(v, &ch, cmp_char, 0, true); // Test sorted 
  found_not = vector_search(v, &ch, cmp_char, 0, false);   // Not sorted 
  fprintf(stdout,"\n_Found_ '%c' in sorted array? %s. How about unsorted? %s.", 
	  ch, YES_OR_NO((found_sorted != -1)), 
	  YES_OR_NO((found_not != -1)));
}

/**
 * Function: test_sort_Search
 * ------------------------
 * Sorts the vector_t into alphabetic order and then tests searching
 * capabilities, both the linear and binary search versions.
 */

static void test_sort_Search(vector_t *alphabet)
{
  vector_sort(alphabet, cmp_char);	 // Sort into order again
  fprintf(stdout, "\n_After_ sorting: ");
  vector_map(alphabet, print_char, stdout);

  test_search(alphabet, 'J');	// Test searching capabilities
  test_search(alphabet, '$');
}

/**
 * Function: test_at
 * ----------------
 * Uses vector_nth to access every other letter and 
 * lowercase it. Prints results using vector_map.
 */

static void test_at(vector_t *alphabet)
{
  int i;
  
  for (i = 0; i < vector_len(alphabet); i += 2) { // Lowercase every other
    char *elem = (char *) vector_nth(alphabet, i);
    *elem = tolower(*elem);
  }
  
  fprintf(stdout, "\n_After_ lowercase every other letter: ");
  vector_map(alphabet, print_char, stdout);
}

/**
 * Function: test_insert_delete
 * --------------------------
 * Inserts dashes at regular intervals, then uses delete to remove
 * them.  Makes sure that insert at allows you to insert at end of
 * array and checks no problem with deleting very last element.  It's
 * always a good idea to directly test the borderline cases to make
 * sure you have handled even the unusual scenarios.
 */

static void test_insert_delete(vector_t *alphabet)
{
  char ch = '-';
  int i;
  
  for (i = 3; i < vector_len(alphabet); i += 4) // Insert dash every 4th char 
    vector_insert(alphabet, &ch, i);
  fprintf(stdout, "\n_After_ insert dashes: ");
  vector_map(alphabet, print_char, stdout);
  fprintf(stdout, "\n");

  
  for (i = 3; i < vector_len(alphabet); i += 3) // Delete every 4th char 
    vector_delete(alphabet, i);
  fprintf(stdout, "\n_After_ deleting dashes: ");
  vector_map(alphabet, print_char, stdout);
  fprintf(stdout, "\n");

  ch = '!';
  vector_insert(alphabet, &ch, vector_len(alphabet));

  vector_delete(alphabet, vector_len(alphabet) - 1);
  fprintf(stdout, "\n_After_ adding and deleting to very end: ");
  vector_map(alphabet, print_char, stdout);
  fprintf(stdout, "\n");
}

/**
 * Function: test_replace
 * ---------------------
 * Uses repeated searches to find all occurrences of a particular
 * character and then uses replace it to overwrite value.
 */

static void repl_hlpr(vector_t *alphabet, char to_find, char to_replace) {
  int found = 0;

  while (found < vector_len(alphabet)) {
    found = vector_search(alphabet, &to_find, cmp_char, 0, false);
    if (found == -1) break;
    vector_replace(alphabet, &to_replace, found);
  }
  fprintf(stdout, "\n- After changing all %c to %c: ", to_find, to_replace);
  vector_map(alphabet, print_char, stdout);
}

static void test_replace(vector_t *alphabet)
{
  char to_find = 'S', to_replace = '*';
  repl_hlpr(alphabet, to_find, to_replace);
  
  to_find = 'A';
  to_replace = '?';
  repl_hlpr(alphabet, to_find, to_replace);
  
  to_find = 'e';
  to_replace = 'z';
  repl_hlpr(alphabet, to_find, to_replace);
    
  to_find = '9';
  to_replace = '&';
  repl_hlpr(alphabet, to_find, to_replace);
}

/** 
 * Function: simple_test
 * --------------------
 * Exercises the vector_t when it stores characters.
 * Because characters are small and don't have any
 * complicated memory requirements, this test is a
 * good starting point to see whether or not your vector
 * even has a prayer of passing more rigorous tests.
 *
 * See the documentation for each of the helper functions
 * to gain a sense as to how simple_test works.  The intent
 * it certainly to try out all of the vector_t operations so
 * that everything gets exercised.
 */

static void simple_test()
{
  fprintf(stdout, " ------------------------- Starting the basic test...\n");
  vector_t alphabet;
  printf(">> Start allocation\n");
  vector_new(&alphabet, sizeof(char), NULL, 4);
  printf(">> Allocation done\n\n");
  //
  printf(">> Start Append\n");
  test_append(&alphabet);
  printf(">> Append done\n\n");
  //

  test_sort_Search(&alphabet);
  test_at(&alphabet);

  printf(">> Start Insert\n");
  test_insert_delete(&alphabet);
  printf(">> Insert done\n\n");

  printf(">> Start Replace\n");
  test_replace(&alphabet);
  printf(">> Replace done\n\n");

  printf(">> Start dispose\n");
  vector_dispose(&alphabet);
  printf(">> Dispose done\n\n");
}

/** 
 * Function: insert_permutation_of_num
 * ------------------------------------
 * Uses a little bit of number theory to populate the
 * presumably empty numbers vector_t with some permutation
 * of the integers between 0 and d - 1, inclusive.  
 * By design, each number introduced to the vector_t should
 * be introduces once and exactly once.  This happens
 * provided n < d and that both n and d are prime numbers.
 */

static void insert_permutation_of_num(vector_t *numbers, long n, long d) {
  long k;
  long residue;
  fprintf(stdout, "Generating all of the numbers between 0 and %ld (using some number theory). ", d - 1);
  fflush(stdout); // force echo to the screen... 

  for (k = 0; k < d; k++) {
    residue = (long) (((long long)k * (long long) n) % d);
    vector_append(numbers, &residue);
  }
  
  assert(vector_len(numbers) == d);
  fprintf(stdout, "[All done]\n");
  fflush(stdout);
}

/**
 * Function: long_cmp
 * ---------------------
 * Called when searching or sorting a vector_t known to
 * be storing long integer data types.
 */

static int long_cmp(const void *vp1, const void *vp2) {
  return (*(const long *)vp1) - (*(const long *)vp2);
}

/**
 * Function: sort_permutation
 * -------------------------
 * Sorts the (very, very large) vector_To_Sort, and confirms
 * that the sort worked.  This is slightly more strenous
 * that the test_sort routine above simply because the vector
 * is much, much bigger.
 */

static void sort_permutation(vector_t *vect_2_sort) {
  long residue, embedded_long;
  vector_t *sorted_vect;
  fprintf(stdout, "Sorting all of those numbers. ");
  fflush(stdout);
  vector_sort(vect_2_sort, long_cmp);
  fprintf(stdout, "[Done]\n");
  fflush(stdout);
  fprintf(stdout, "Confirming everything was properly sorted. ");
  fflush(stdout);
  sorted_vect = vect_2_sort; // need better name now that it's sorted... 
  for (residue = 0; residue < vector_len(sorted_vect); residue++) {
    embedded_long = *(const long *) vector_nth(sorted_vect, residue);
    assert(embedded_long == residue);
  }
  fprintf(stdout, "[Yep, it's sorted]\n");
  fflush(stdout);
}

/**
 * Function: delete_everything_veryslowly
 * ------------------------------------
 * Empties out the vector_t in such a way that vector_delete
 * is exercised to the hilt.  By repeatedly deleting from
 * within the vector, we ensure that the shifting over of
 * bytes is working properly.
 */

static void delete_everything_veryslowly(vector_t *numbers)
{
  long largest_original_num;
  fprintf(stdout, "Erasing everything in the vector_t by repeatedly deleting the 100th-to-last remaining element (be patient).\n");
  fflush(stdout);
  largest_original_num = *(long *)vector_nth(numbers, vector_len(numbers) - 1);
  while (vector_len(numbers) >= 100) {
    vector_delete(numbers, vector_len(numbers) - 100);
    assert(largest_original_num == *(long *)vector_nth(numbers, vector_len(numbers) -1));
  }
  fprintf(stdout, "\t[Okay, almost done... deleting the last 100 elements... ");
  fflush(stdout);
  while (vector_len(numbers) > 0) vector_delete(numbers, 0);
  fprintf(stdout, "and we're all done... whew!]\n");
  fflush(stdout);
}

/**
 * Function: challenging_test
 * -------------------------
 * Uses a little bit of number theory to generate a very large vector
 * of four-byte values.  Some permutation of the numbers [0, 3021367)
 * is generated, and in the process the vector_t grows in such a way that
 * several realloc calls are likely made.  This will catch any errors
 * with the reallocation, particulatly those where the implementation
 * fails to catch realloc's return value.  The test then goes on the
 * sort the array, confirm that the sort succeeded, and then finally
 * delete all of the elements one by one.
 */

static const long klarge_prime = 1398269;
static const long keven_larger_prime = 3021377;
static void challenging_test() {
  vector_t lots_of_numbers;
  fprintf(stdout, "\n\n------------------------- Starting the more advanced tests...\n");  
  vector_new(&lots_of_numbers, sizeof(long), NULL, 512);
  insert_permutation_of_num(&lots_of_numbers, klarge_prime, keven_larger_prime);
  sort_permutation(&lots_of_numbers);
  delete_everything_veryslowly(&lots_of_numbers);
  vector_dispose(&lots_of_numbers);
}

/** 
 * Function: free_string
 * --------------------
 * Understands how to free a C-string.  This
 * function should be used by all vectors that
 * store char *'s (but only when those char *s
 * point to dynamically allocated memory, as
 * they do with strings.)
 */

static void free_string(void *elem_addr) {
  char *s = *(char **) elem_addr;
  free(s); 
}

/** 
 * Function: print_str
 * ---------------------
 * Understands how to print a C-string stored
 * inside a vector.  The target FILE * should
 * be passed in via the aux_Data_ parameter.
 */

static void print_str(void *elem_addr, void *aux_data) {
  char *word = *(char **) elem_addr;
  FILE *fp = (FILE *) aux_data;
  fprintf(fp, "\t%s\n", word);
}

/**
 * Function: memory_test
 * --------------------
 * memory_test exercises the vector_t functionality by
 * populating one with pointers to dynamically allocated
 * memory.  The insertion process marks the transfer of
 * of responsibility from the client to the vector, so
 * we now need to specify a non-NULL vector_free_fun_t so
 * the a vector_t can apply it to the elements it inherits
 * from the client.  Make sure you understand why the
 * casts within the two functions above (free_string, print_str)
 * are char ** casts and not char *.  If you truly understand,
 * they you've learned what is probably the most difficult-to-
 * learn concept taught in CS107.
 */

static void memory_test()
{
  int i;
  const char * const kquestion_words[] = {"who", "what", "where", "how", "why"};
  const int knum_question_words = sizeof(kquestion_words) / sizeof(kquestion_words[0]);
  vector_t question_words;
  char *question_word;
  
  fprintf(stdout, "\n\n------------------------- Starting the memory tests...\n");
  fprintf(stdout, "Creating a vector_t designed to store dynamically allocated C-strings.\n");
  vector_new(&question_words, sizeof(char *), free_string, knum_question_words);
  fprintf(stdout, "Populating the char * vector_t with the question words.\n");
  for (i = 0; i < knum_question_words; i++) {
    question_word = malloc(strlen(kquestion_words[i]) + 1);
    strcpy(question_word, kquestion_words[i]);
    // printf("++++++ about to insert %s\n", question_word);
    vector_insert(&question_words, &question_word, 0);  // why the ampersand? isn't question_word already a pointer?
    // vector_map(&question_words, print_str, stdout);
  }
  
  fprintf(stdout, "Mapping over the char * vector_t (ask yourself: why are char **'s passed to print_str?!!)\n");
  vector_map(&question_words, print_str, stdout);
  //
  fprintf(stdout, "Finally, destroying the char * vector.\n");
  vector_dispose(&question_words);
}

/**
 * function: main
 * --------------
 * The enrty point into the test application.  The
 * first test is easy, the second one is medium, and
 8 the final test is hard.
 */

int main(int ignored, char **also_Ignored) {
  simple_test();
  challenging_test();
  memory_test();
  return 0;
}

