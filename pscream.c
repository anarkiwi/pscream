/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 to special case 2.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"


/* Not so trivial macro to search for i in the sieve, put the result in
   parent, and return whether or not the node was found. */
#define IN_SIEVE(parent, i) (*(parent = searchsieve(i))) 

#define INSERT_SIEVE(parent, newnode) \
	(IN_SIEVE(parent, newnode->multiple) ? \
	 NULL : (*parent = newnode))

/* Keep on trying to insert the next multiple of the current prime into
   the sieve, until the next multiple in unique in the sieve. This is 
   where the significant addition is done.  Uses the tricky macro,
   above. */   /* Hack: skip even numbers */
static void insertmultiple(sieve *newnode) {
  static sieve **parent;

#ifdef SPECIAL_CASE_TWO
  do
    newnode->multiple += newnode->prime;
  while (!(newnode->multiple & 1) || !INSERT_SIEVE(parent, newnode));
#else
  do
    newnode->multiple += newnode->prime;
  while (!INSERT_SIEVE(parent, newnode));
#endif
}


/* Macro to take a node out of the sieve, and then put it back at the next
   non-colliding multiple. */
#define REINSERT(parent)	insertmultiple(removesieve(parent))

/* Macro to put a completely new node in the sieve */
#define NEW_INSERT(i)		insertmultiple(initsievenode(i, i))

/* Trivial macro to decide whether we need to insert a newly-found prime 
   in the sieve or not. */
#define	NEED_PRIME(i, sqrtn)	(i < sqrtn)


/* Find primes while we still need to add to the sieve. */
static BOOLEAN doinitinterval(void) {
  register sieve **parent;

/* Start the clock at the beginning of the reporting interval. */
  if (state.isfull) 
    return TRUE;
  else {
    volat.startivaltime = volat.timeused();

#ifdef SPECIAL_CASE_TWO
    /* Handle 2 as prime, the first time around */
    if (state.i < 2) {
        state.total++;
        state.lasthighest = state.highest;
        state.highest = 2;
	state.i = 1;
    }
#endif

    while (state.i < volat.limit && !state.isfull) {
#ifdef SPECIAL_CASE_TWO
      state.i += 2; 
#else
      ++state.i;
#endif

      if (IN_SIEVE(parent, state.i))
/* Number wasn't prime, so we put the next non-colliding multiple of it
   into the sieve. */
        REINSERT(parent);
      else {
/* Number WAS prime, keep some statistics. */
        state.total++;
        state.lasthighest = state.highest;
        state.highest = state.i;

/*  If the sieve isn't yet full, then insert the prime in the sieve. */
        if (NEED_PRIME(state.i, volat.sqrtmax)) {
          NEW_INSERT(state.i);
          state.members++;
        } else {
/* Adjust the sieve's limit upwards if possible, since we now have the
   highest i that can be considered without rollover. */
          if (state.max == options.sievemax) {
            state.max = CARD_MAX - 2 * state.lasthighest;
            calcivals();
	  }

/* Check that we can call the sieve full, since volat.sqrtmax might've been
   upped by the above code. */
          if ((state.isfull = !NEED_PRIME(state.i, volat.sqrtmax))) {
            domaxreport();
            dumpstate();
	  }
	}
        
/* Every time we find a prime, check for caught signals. */
        handlesig();
      }
    }

    return state.isfull;
  }
}


/* Now that the sieve is full, we can just spin along. */
static void dointerval(void) {
  register sieve **parent;

/* If sieve isn't full yet, then check each prime as it is found, until
   we don't need any more. */
  if (doinitinterval()) {
    volat.startivaltime = volat.timeused();

    while (state.i < volat.limit) {
#ifdef SPECIAL_CASE_TWO
      state.i += 2; 
#else
      ++state.i;
#endif

      if (IN_SIEVE(parent, state.i))
        REINSERT(parent);
      else {
        state.total++;
        state.highest = state.i;
        handlesig();
      }
    }

/* Do a report and a state-dump at the beginning of each interval. */
    doreport();
    dumpstate();
  }
}


/* Split up the sieve range into intervals, and sieve through each one. */
extern void findprimes(void) {
  do {
/* Make sure that we actually sieve to the limit, if the range doesn't
   divide evenly into percentages. i.e. advance the current limit toward
   the maximum, either to the next percentage interval or the actual maximum
   whichever is closer. */
    volat.limit += mincard(volat.ivalsize, state.max - volat.limit);
    dointerval();
  } while (volat.limit < state.max);
}
