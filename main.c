/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28.

   See copyright and disclaimer in file pscream.h in this directory. */

#define DEFINE_DATA_HERE
#include "pscream.h"


/* Return the lesser of arguments a and b. */
cardinal mincard(cardinal a, cardinal b) {
  if (a < b)
    return a;
  else
    return b;
}


/* Divide two cardinals to return a float, avoiding the possibility of a
   division by zero. */
float safediv(cardinal a, cardinal b) {
  if (b)
    return ((float)(a) / (float)(b));

  return (float)a;
}


/* Set up for the sieve. */
static void init(int argc, const char *argv[]) {
  initmystate();
  initoptions();
  getoptions(argc, (char * const *)argv);
  initlog();
  initsieve();
  processoptions();
  initvolat();
  initsig();

  logs("I GOGO all data structures initialised");
}


/* Execute a normal exit from the program. */
void done(void) {
  logblank();
  donesig();
  donesieve();
  donelog();

  exit(volat.gotsig);
}


int main(int argc, const char *argv[]) {
  init(argc, argv);
  findprimes();
  done();
  return 0;
}
