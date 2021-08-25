/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 to eliminate 
           dependency on global variable.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"


/* push a sieve bst node pointer onto the stack, and remember the height of
   the bst at which it was pushed. */
void prepush(prestacktype *stack, sieve **parent) {
  static prestacktype *newnode;

  if ((newnode = (prestacktype*)malloc(sizeof(prestacktype)))) {
/* the push is accomplished without modifying the stack pointer.
   first, a new stack node is allocated, and the current head copied
   into it. */
    *newnode = *stack;

/* then the head node is modified with the pushed values, and the head
   connected to the old head, now in newnode. */
    stack->parent = parent;
    stack->next = newnode;
  } else
    die("CORE prepush: couldn't malloc");
}


void postpush(poststacktype *stack, sieve *curr, const cardinal height) {
  static poststacktype *newnode; 

  if ((newnode = (poststacktype*)malloc(sizeof(poststacktype)))) {
/* the push is accomplished without modifying the stack pointer.
   first, a new stack node is allocated, and the current head copied
   into it. */
    *newnode = *stack;

/* then the head node is modified with the pushed values, and the head
   connected to the old head, now in newnode. */
    stack->curr = curr;
    stack->height = height;
    stack->next = newnode;
  } else
    die("CORE postpush: couldn't malloc");
}


/* pull off from the stack a sieve bst node pointer and return it, also
   return the height of the node pointer. */
void prepull(prestacktype *stack, sieve ***parent) {
  static prestacktype *oldnode;

  oldnode = stack->next;
  *parent = stack->parent;

/* the pull is accomplished without modifying the head of the stack pointer.
   first, the head node but one is isolated, its contents copied into the
   current head, and the isolated node disposed of. */
  *stack = *oldnode;
  free(oldnode);
}


sieve *postpull(poststacktype *stack, cardinal *height) {
  static poststacktype *oldnode;
  static sieve *curr;

  oldnode = stack->next;
  curr = stack->curr;
  *height = stack->height;

/* the pull is accomplished without modifying the head of the stack pointer.
   first, the head node but one is isolated, its contents copied into the
   current head, and the isolated node disposed of. */
  *stack = *oldnode;
  free(oldnode);

  return curr;
}


prestacktype *initprestack(sieve **ptrfirst) {
  prestacktype *stack = (prestacktype*)malloc(sizeof(prestacktype));

  if (stack) {
    stack->next = NULL;

    prepush(stack, ptrfirst);
  } else
    die("CORE initprestack: couldn't malloc");

  return stack;
}


poststacktype *initpoststack(sieve *first) {
  poststacktype *stack = (poststacktype*)malloc(sizeof(poststacktype));

  if (stack) {
    stack->curr = NULL;
    stack->height = 0;
    stack->next = NULL;

    postpush(stack, first, 0);
  } else
    die("CORE initpoststack: couldn't malloc");

  return stack;
}


/* dispose of a stack, in case of incomplete traversal, or when
   finished with the stack. */
void doneprestack(prestacktype *stack) {
  static prestacktype *oldnode;

/* while the stack isn't empty, pull off a node and dispose of it. */
  while (stack) {
    oldnode = stack;
    stack = stack->next;
    free(oldnode);
  }
}


void donepoststack(poststacktype *stack) {
  static poststacktype *oldnode;

/* while the stack isn't empty, pull off a node and dispose of it. */
  while (stack) {
    oldnode = stack;
    stack = stack->next;
    free(oldnode);
  }
}
