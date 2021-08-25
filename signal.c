/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"


/* Catch signals and either store them for handlesig() to process later,
   or act on them immediately, depending on their type. */
static void catchsig(int gotsig) {
  switch (volat.gotsig = gotsig) {
    case SIGHUP:
    case SIGINT:
    case SIGTERM:
      break;

    case SIGSEGV:
/* I do so hope this does not continue to happen. */
      die("SEGV went walkies in pointer land");
  }
}


/* Hook various signals. */
void initsig(void) {
  signal(SIGHUP, catchsig);
  signal(SIGINT, catchsig);
  signal(SIGTERM, catchsig);
  signal(SIGSEGV, catchsig);
}

/* Restore the signal handlers hooked by the program to their initial state. */
void donesig(void) {
  signal(SIGHUP, SIG_DFL);
  signal(SIGINT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
}


/* Check the caught signal flag, and deal with it if it has been set. */
void handlesig(void) {
  if (volat.gotsig) {
    logblank();

    switch (volat.gotsig) {
      case SIGHUP:
/* Force a report and a state-dump on a SIGHUP, though keep on going
   afterward. */
        logs("I RPRT got hangup signal");
        doreport();
        dumpstate();
        signal(SIGHUP, catchsig);
        break;        

      case SIGINT:
/* Stop as nicely and as quickly as possible on a SIGINT, though don't
   bother saving state. */
        logs("I STOP got interrupt signal");
        doreport();
        done();
        break;

/* We had to die for some reason, so report and dump state so that we
   can continue from precisely where we left off, later. */
      case SIGTERM:
        logs("I TERM got termination signal");
        doreport();
        dumpstate();
        done();
    }

/* Clear the caught signal flag once we have dealt with it. */
   volat.gotsig = 0;
  }
}

