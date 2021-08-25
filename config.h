/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 for special case 2

   See copyright and disclaimer in file pscream.h in this directory. 

   Unsigned integer type to use. Must be large enough to hold the largest
   number to consider. Defaults to unsigned int if not overridden by one
   of the following. */

/*#define	SHRT */
/*#define	LONG */

/* Default percentage of maximum to elapse before a report
   and state-dump. */
#define		DEFAULT_INTERVAL	10

/* Default filename of the state file if not specified. */
#define		DEFAULT_STATEFILE	"pscream.out"

/* Default filename of the logfile if not specified. */
#define		DEFAULT_LOGFILE		"pscream.log"

/* If defined, multiples of 2 will be special cased */
#define	SPECIAL_CASE_TWO

/* Size of hash table to use in hashbst.  MUST be a power of 2, ie 2**n. */
#define HASH_SIZE 4096

#include <stdlib.h>
#include <unistd.h>
