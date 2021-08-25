/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 for encapsulation.
   Updated by Ewen McNeill <ewen@naos.co.nz>, 2001/12/02 to handle data
        structures properly.

   See copyright and disclaimer in file pscream.h in this directory. */

#ifndef TYPES_H
#define TYPES_H

#define	FALSE		0
#define	TRUE		1

#define	DUMMY_KEY	0	/* A key-placeholder for fake nodes. */
#define	LINELEN		81	/* Maximum number of characters per line. */

#define	FIRSTPRIME	2	/* The first prime by pedantic definition. */

/* Get the right format string and maximum for the integer type we're using. */
#ifdef SHRT

typedef unsigned short cardinal;
#define	CARD_MAX	USHRT_MAX
#define UL		"%hu"

#elif defined(LONG)

typedef unsigned long cardinal;
#define	CARD_MAX	ULONG_MAX
#define UL		"%lu"

#else

/* Default to plain unsigned integers. */

typedef unsigned cardinal;
#define	CARD_MAX	UINT_MAX
#define UL		"%u"

#endif

/* Program version, both for display and for statefile compatibility
   checking. */
#define	VERSION		((float) 1.1)

/* Constant strings used in forming banners. */
#define VERSTR		"%.2f"
#define	NAME		"Primal Scream"
#define	COPYRIGHT	"(C) 1994-1995 Josh Bailey"
#define BANNER		" " NAME " v" VERSTR " " COPYRIGHT

/* Flag data-type; not at all widely used, and then only for clarity. */
#define	BOOLEAN		cardinal

/* Ensure that data gets defined in only one place -- EDM, 2001/12/02 */
#ifndef DEFINE_DATA_HERE
#define STRUCTTYPE extern
#else
#define STRUCTTYPE
#endif

/* Structure to contain all of the sieve's vital information, that
   can be saved between runs. */
STRUCTTYPE struct {
  BOOLEAN isfull;		/* When no more primes needed to sieve. */

  cardinal highest;		/* Highest prime found so far. */
  cardinal i;			/* Number under consideration. */
  cardinal lasthighest;		/* Last highest prime but one; used in
				   adjusting the sieve's theoretical
				   limit. */
  cardinal lastlimit;		/* Where the last limit ended. */
  cardinal lasttotal;		/* Total primes found at last report. */
  cardinal max;			/* The sieve's limit. */
  cardinal members;		/* Nodes in the sieve structure. */
  cardinal total;		/* Total number of primes found so far. */

  float totltime;		/* Total time so far. */

  float pctival;		/* Percentage interval between reports. */
} state;


/* Structure to contain sieve parameters that are calculated at run-time. */
STRUCTTYPE struct {
  cardinal ivalsize;		/* Size of each reporting interval. */
  cardinal limit;		/* Where the current interval is to end. */
  cardinal primesival;		/* Primes found in this interval. */
  cardinal sqrtmax;		/* Square-root of the sieve's maximum. */

  float ivaltime;		/* Time spent since last report. */

  float balance;		/* How balanced the sieve is. */
  float done;			/* Fraction of the sieve completed. */
  float ofival;			/* Fraction of numbers since last report
				   that are prime. */
  float ofsofar;		/* Fraction of numbers considered so far
				   that are prime. */
  float primesivaltime;		/* Primes per second since last report. */
  float primestotltime;		/* Primes per second so far. */

  float ivalipersec;		/* i per second since last report. */
  float totlipersec;		/* i per second so far. */

  float startivaltime;		/* When the current reporting interval
				   started. */

  int gotsig;			/* Caught signal flag. */

  float (*timeused)(void);	/* Pointer to function used to mark time. */
} volat;


/* Structure to contain user-specified options. */
STRUCTTYPE struct {
  BOOLEAN ignorestatefile;	/* Specify whether to ignore an existing
				   statefile. */
  cardinal sievemax;		/* Maximum safe sieve limit. */
  cardinal max;			/* Sieve maximum as specified by user. */

  char logfilename[LINELEN];	/* The name of the logfile. */
  char statefilename[LINELEN];	/* The name of the statefile. */

  float pctival;		/* Report interval as specified by user. */
} options;


/* Sieve-node structure; a typical BST, containing two data fields, of
   which "multiple" is the key.  The pointer subfields lesser and greater
   point to nodes whse keys are lesser or greater than the current key,
   respectively. */
typedef struct sieve {
  cardinal multiple;		/* Number that cannot be prime, having
				   being already eliminated by the sieve. */
  cardinal prime;		/* Prime that originally generated the
				   multiple. */
  struct sieve *lesser, *greater;
} sieve;


/* Stack structures used by the non-recursive treewalking routines. */
typedef struct prestacktype {
  struct sieve **parent;	/* Pointers to the bst node. */
  struct prestacktype *next;	/* Pointer to the next stack node. */
} prestacktype;


typedef struct poststacktype {
  struct sieve *curr;		/* Pointers to the bst node. */
  cardinal height;		/* Height of the tree at the current node. */
  struct poststacktype *next;	/* Pointer to the next stack node. */
} poststacktype;
#endif
