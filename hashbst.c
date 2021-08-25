/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Written by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 to use hash
   of BST in order to reduce overall depth of BSTs. (based on bst.c)

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"

/* Hash of BSTs: we use an array of pointers to BST roots.  For
 * efficiency the array must be 2**N in size, so we can get the bucket
 * to use by simply taking the low order bits of the key.  Larger sizes
 * will take more memory, but will reduce the number of hash collisions.
 */
#ifndef HASH_SIZE
#define HASH_SIZE 4096
#warning HASH_SIZE defaulting to 4096
#endif

#define HASH_MASK (HASH_SIZE - 1)

/* Roots of sieve */
sieve *roots[HASH_SIZE];


/* Allocate a new sieve node, and initialise it with the given values. */
sieve *initsievenode(const cardinal multiple, const cardinal prime) {
  static sieve *newnode;

  if ((newnode = (sieve*)malloc(sizeof(sieve)))) {
    newnode->multiple = multiple;
    newnode->prime = prime;
    newnode->lesser = NULL;
    newnode->greater = NULL;
  } else
    die("CORE initsievenode: couldn't malloc");

  return newnode;
}


/* Walk down the tree looking for a particular key, and return the address of
   the pointer that points to the node containing the key. */
sieve **searchsieve(const cardinal key) {
  register sieve **parent = &(roots[key & HASH_MASK]), *curr;

/* Keep going, as long as the current node isn't empty, and isn't the one
   we're looking for. */
  while ((curr = *parent) && (curr->multiple != key)) {
/* Go to lesser or greater if our key is lesser or greater than the key of 
   the current node. */
    if (key < curr->multiple)
      parent = &(curr->lesser);
    else
      parent = &(curr->greater);
  }

  return parent;
}


/* Remove the node pointed to by "parent" from the sieve, and return it,
   so that it can be reinserted. Saves having to reallocate memory. */
sieve *removesieve(sieve **parent) {
  register sieve *curr = *parent;

/* If the node has only one child, then removal's no problem. Rather like
   removing a node from a linked list. */
  if (!curr->lesser) {
    *parent = curr->greater;
    curr->greater = NULL;
  } else if (!curr->greater) {
    *parent = curr->lesser;
    curr->lesser = NULL;
  } else {
/* OH NO! The node to remove has TWO children! GASP!

   Drop down to the left subtree of the current node. We're going to copy
   another node into the old one, to maintain the order of the tree. */

   curr = curr->lesser;

/* Walk down the right subtree of the current node until we can walk no
   further. My Shiflet's algorithm textbook says I have to do an in-order
   traversal. Wirth says I just have to walk down the right subtree of the 
   left subtree. Logical really, since the the traversal will always end at
   the rightmost branch of the subtree. 

   Shiftlet's textbook has cover art, so I used Wirth's algorithm. :) */

   while (curr->greater) 
     curr = curr->greater;

/* Copy the node we found into the node we are effectively deleting. */

   (*parent)->multiple = curr->multiple;
   (*parent)->prime = curr->prime;

/* Clean up so we can return a pointer to the node we removed from
   the tree. */

   curr = curr->lesser;
   curr->lesser = NULL;
  }

  return curr; 
}


/* Sieve is empty when dummy node is reached. */
#define NOT_EMPTY(stack) (stack->next)

/* Balance now calculated as:

   1 - (average_branch_length / sievemembers)

   should really be something involving log2sievemembers which would be
   the worst case for a perfectly balanced tree, but the standard library
   evidently doesn't include a log2 function and my maths isn't good enough
   to write my own :(. 

   Present calculation could be said to measure the tree's "flatness;"
   a perfectly linear tree would have a flatness of 0%, while one that was
   at right angles to the root, spreading horizontally instead of
   vertically, would have a flatness tending toward 100%. */
static void prewalk(sieve **rootparent, sieve* (*visit)(sieve **parent)) {
  cardinal sievemembers = 0;
  prestacktype *stack = initprestack(rootparent);
  sieve *curr, **parent;

  do {
    prepull(stack, &parent);

    if ((curr = visit(parent))) { 
      sievemembers++;
      
      prepush(stack, &(curr->greater));
      prepush(stack, &(curr->lesser));
    }
  } while (NOT_EMPTY(stack));

  doneprestack(stack);

#if 0
/* Sanity check.... */
  if (sievemembers != state.members)
    die("MALF prewalk: tree walk/state.member count mismatch");
#endif 
}


/* Calculate the average height of the tree branches, given the total
   height and the number of branches. Just a simple division, but makes
   the code a little clearer. */
#define	averageheight(branches, totalheight)	safediv(branches, totalheight)

/* A sieve node is a leaf if both its lesser and greater fields are NULL. */
#define IS_LEAF(curr)	(!curr->lesser && !curr->greater)

static void postwalk(sieve *root, void (*visit)(sieve *curr)) {
  cardinal sievemembers = 0, totalheight = 0, branches = 0, height = 0;
  poststacktype *stack = initpoststack(root);
  sieve *curr;

  do {
    if ((curr = postpull(stack, &height))) { 
      sievemembers++;
      height++;

      if (IS_LEAF(curr)) {
        branches++;
        totalheight += height;
      }

      postpush(stack, curr->greater, height);
      postpush(stack, curr->lesser, height);
    } 

    visit(curr);
  } while (NOT_EMPTY(stack));

  donepoststack(stack);

#if 0
/* Sanity check.... */
  if (sievemembers != state.members)
    die("MALF postwalk: tree walk/state.member count mismatch");
#endif

  if (sievemembers) 
    volat.balance = 1 - averageheight(totalheight, branches) / sievemembers;
  else
    volat.balance = 0;
}


/* Initialise the sieve structure with the DummyKey value. Used to make a
   fake root so that we never have to insert before it. Simplifies tonnes of
   stuff. */
void initsieve(void) {
  int i;
  for (i = 0; i < HASH_SIZE; ++i)
    roots[i] = NULL;

  logs("I INIT sieve structure initialised");
}


/* Free a node allocated in the sieve. */
static void donenode(sieve *curr) {
  if (curr)
    free(curr);
}


/* Free up the sieve structure */
void donesieve(void) {
  int i;
  for (i = 0; i < HASH_SIZE; ++i)
    if (roots[i]) {
      postwalk(roots[i], donenode);
    }

  logs("I GONE sieve structure disposed of");
}


/* Write out the contents of a sieve node if non-NULL, or write out a
   "0 0" place holder. */
static void dumpnode(sieve *curr) {
  if (curr) {
    sieveout(curr->multiple, curr->prime);
  }
  else
    sieveout(DUMMY_KEY, DUMMY_KEY);
}


/* Write out the sieve to the state file. */
void dumpsieve(void) {
  int i;
  for (i = 0; i < HASH_SIZE; ++i) {
    postwalk(roots[i], dumpnode);
  }

  logu("S DUMP " UL " at sieve dump", state.i);
}


/* Read in the sieve from the state file. */
void readsieve(void) {
  int i;
  for (i = 0; i < HASH_SIZE; ++i)
    prewalk(&(roots[i]), readnode);
}
