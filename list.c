#include "pscream.h"


/* Find all the prime numbers under a given n, dividing each number
   by the primes under the square-root of n, stored in a linked-list.

   This program provided to show the impracticability of using division. */

typedef struct list {
  cardinal key;
  struct list *next;
} list;


#define INDIVISIBLE(i, key)	(i % key)

/* Take i, and divide it by the primes in stored in the list, until the 
   entire list has been processed, or i is found to be divisible. If i
   survives with no even divisions, then i is prime. */
static BOOLEAN isprime(const cardinal i, list *primeslist) {
  while (INDIVISIBLE(i, primeslist->key) && (primeslist = primeslist->next))
    ;

/* If we're at the end of the list, then i survived every attempted division,
   so i is prime. */
  return !primeslist;
}


/* Append a new prime to the end of the list, if it is needed. */
static void append(const cardinal i, list ***tailoflist) {
  static list *newnode;

  newnode = (list*)malloc(sizeof(list));

  newnode->key = i;
  newnode->next = NULL;

/* Update the pointer to the end of the list. */
  **tailoflist = newnode;
  *tailoflist = &(newnode->next);
}


/* Free the list, once done with. */
static void donelist(list *primeslist) {
  list *oldlist;

  do {
    oldlist = primeslist;
    primeslist = primeslist->next;
    free(oldlist);
  } while (primeslist);
}


/* Check each number up to and including i to see if it is prime. */
static cardinal findprimesunder(const cardinal n) {
  register cardinal sqrtn = sqrt(n) + 1;
  register cardinal i = FIRSTPRIME + 1, total = 1;

  register BOOLEAN listfull = FALSE;

  list *primeslist = NULL;
  list **tailoflist = &primeslist;

/* Initialise the list with the first-defined prime, 2. */
  append(FIRSTPRIME, &tailoflist);

  for (; i <= n; i++)
/* If the number is prime, keep a count. */
    if (isprime(i, primeslist)) {
      total++;

/* If we need more primes to check up to n, then add the prime just found
   to the list. */
      if (!listfull) {
        if (i < sqrtn)
          append(i, &tailoflist);
        else
          listfull = TRUE;
      }
    }

  donelist(primeslist);

  return total;
}


int main(void) {
  cardinal n;

  printf("Find all primes under what n? ");
  scanf(UL , &n);

  printf("There are " UL " primes under " UL ".\n", findprimesunder(n), n);

  return EXIT_SUCCESS;
}
