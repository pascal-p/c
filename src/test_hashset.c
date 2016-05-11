#include "hashset.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <assert.h>

const int kNumBuckets = 26;

struct frequency {
  char ch;		// a particular letter
  int occurrences;	// the number of times it occurs
};

/**
 * Fucntion: hash_frequency
 * -----------------------
 * Hash function used to partition frequency structures into buckets.  Our
 * hash function is pretty simplistic, we simply mod the char by the
 * number of buckets.  This provides a number in the range 0 to numBuckets - 1,
 * which is the proper range for our hashset_t.
 */

static int hash_frequency(const void *elem, int numBuckets) {
  struct frequency *freq = (struct frequency *)elem;
  return (freq->ch % numBuckets);
}

/**
 * Function: print_frequency
 * -------------------------
 * Mapping function used to print one frequency stored in a hashset_t.  The
 * file is passed as the client data, so that it can be used to print to
 * any FILE *.
 */

static void print_frequency(void *elem, void *fp) {
  struct frequency *freq = (struct frequency *)elem;
  fprintf((FILE *)fp, "Character %c occurred %4d times\n", 
          freq->ch, freq->occurrences);
}

/**
 * Function: cmp_letter
 * -----------------------
 * Comparator function used to compare two frequency entries within hashset_t.
 * Two frequencies are considered the same if they refer to the same char.
 * This function is used in hashset_lookup to decide if a char already has been
 * entered.  This function has the same return semantics as strcmp: 
 * (negative if 1<2, zero if 1==2, positive if 1>2).
 */

static int cmp_letter(const void *elem1, const void *elem2) {
  struct frequency *freq1 = (struct frequency *)elem1;
  struct frequency *freq2 = (struct frequency *)elem2;    
  return (freq1->ch - freq2->ch);
}

/**
 * Function: cmp_occurrences
 * -----------------------------
 * Comparator function used to compare two frequency entries.  This one sorts
 * by occurrence as the primary key, and if occurrences are the same, it uses
 * the char as the secondary key, thus the call to cmp_Letter which
 * compares the chars.  It "reverse-sorts" so a higher frequency bubbles to
 * the top.
 */

static int cmp_occurrences(const void *elem1, const void *elem2) {
  struct frequency *freq1 = (struct frequency *)elem1;
  struct frequency *freq2 = (struct frequency *)elem2;
  
  if (freq1->occurrences > freq2->occurrences) return -1; 
  else if (freq1->occurrences < freq2->occurrences) return 1;
  else return cmp_letter(freq1, freq2);
}

/**
 * Function: buildtableoflettercounts
 * ----------------------------------
 * Opens a file (in this case, the hashsettest.c you are currently reading).
 * and pulls characters out one by one and counts the number of times
 * each character occurs.  The frequecy information about the alphabetic
 * characters is stored in the counts hashset_t passed in by address.
 */

static void build_table_of_letter_count(hashset_t *counts) {
  struct frequency localFreq, *found;
  int ch;
  FILE *fp = fopen("src/test_hashset.c", "r"); // open self as file
  
  assert(fp != NULL);
  while ((ch = getc(fp)) != EOF) {
    if (isalpha(ch)) { // only count letters
      localFreq.ch = tolower(ch);
      localFreq.occurrences = 1;
      
      // See if we already have an entry for this char		
      found = (struct frequency *) hashset_lookup(counts, &localFreq);	
      if (found != NULL) 		// increment if already there 
        localFreq.occurrences = found->occurrences + 1;
      hashset_enter(counts, &localFreq); // enter should overwrite if needed
    }
  }
  fclose(fp);
}

/**
 * Function: add_frequency
 * ----------------------
 * Mapping function used to take a frequency stored 
 * in a hashset_t and append it to an vector of frequencies.  
 * The address of the vector is passed in as the auxiliarydata.
 */

static void add_frequency(void *elem, void *v) {
  vector_append((vector_t *) v, elem);
}

/**
 * Function: test_hash_table
 * -----------------------
 * Runs a test of the hashset_t using a frequency structure as the element 
 * type.  It will open a file, read each char, and count the number of
 * times each char occurs.  Tests enter, lookup, and mapping for the hashset_t. 
 * Prints contents of table to stdout.  Then it dumps all the table elements 
 * into a vector and sorts them by frequency of occurrences and 
 * prints the array out.  Note that this particular stress test passes
 * 0 as the initialAllocation, which the vector is required to handle
 * gracefully - be careful!
 */
static void test_hash_table(void) {
  hashset_t counts;
  vector_t sortedCounts;
  
  hashset_new(&counts, sizeof(struct frequency), kNumBuckets, hash_frequency, cmp_letter, NULL);
  
  fprintf(stdout, "\n\n ------------------------- Starting the HashTable test\n");
  build_table_of_letter_count(&counts);
  
  fprintf(stdout, "Here is the unordered contents of the table:\n");
  hashset_map(&counts, print_frequency, stdout);  // print contents of table
  
  vector_new(&sortedCounts, sizeof(struct frequency), NULL, 0);
  hashset_map(&counts, add_frequency, &sortedCounts);   // add all freq to array
  vector_sort(&sortedCounts, cmp_letter);      // sort by char
  fprintf(stdout, "\nHere are the trials sorted by char: \n");
  vector_map(&sortedCounts, print_frequency, stdout);
  
  vector_sort(&sortedCounts, cmp_occurrences); //sort by occurrences
  fprintf(stdout, "\nHere are the trials sorted by occurrence & char: \n");
  vector_map(&sortedCounts, print_frequency, stdout);	// print out array 
  
  vector_dispose(&sortedCounts);				// free all storage 
  hashset_dispose(&counts);
}

int main(int ununsed, char **alsoUnused) {
  test_hash_table();	
  return 0;
}

