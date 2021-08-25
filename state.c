/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"


FILE *statefile;


/* Return processor time used in seconds since the program began. 
   This function used when implemented in the standard library. */
static float myclock(void) {
  return (float) clock() / CLOCKS_PER_SEC;
}


/* Return real time used since program began. Used if the standard
   library doesn't implement clock(). */
static float mytime(void) {
  static time_t firsttime = 0;

  if (firsttime == 0)
    firsttime = time(NULL);

  return (float) difftime(time(NULL), firsttime);
}


/* Do the equivilent of safediv for a cardinal and a float. Called
   persecond for clarity, since that's all it's ever used for. */
static float persecond(cardinal i, float seconds) {
  if (seconds > 0)
    return (float)(i) / seconds;

  return i;

/*return (seconds > 0 ? i / seconds : i); */
}


#define NO_CLOCK	(clock() == -1)
#define NO_TIME		(time(NULL) == -1)

static void inittimeused(void) {
 if (NO_CLOCK) {
   logs("I NCLK clock() unavailable, using time()");   

   if (NO_TIME)
     die("TIME no way to mark time; no clock() nor time()"); 

   volat.timeused = &mytime;
 } else
   volat.timeused = &myclock;
}


/* Write out the contents of a sieve node to the state file. */
void sieveout(cardinal a, cardinal b) {
  if (fprintf(statefile, UL " " UL "\n", a, b) < 0)
    die("FILE sieveout: couldn't write to sieve file");
}


/* Read in a node from the state file, and initialise it if it should be
   non-NULL. */
sieve *readnode(sieve **parent) {
  static cardinal newmultiple, newprime;

  if (fscanf(statefile, UL " " UL "\n", &newmultiple, &newprime) < 0)
    die("FILE readnode: unexpected EOF reading state file");

  if (newmultiple && newprime)
    *parent = initsievenode(newmultiple, newprime);

  return *parent;
}


/* Initialise the state-keeping structure to defaults. Would be called 
   "initstate" instead but for a name conflict with something inside
   an EMX header file. Sigh. */
void initmystate(void) {
  state.isfull = FALSE;

  state.highest = 0;
  state.i = FIRSTPRIME - 1;
  state.lasthighest = 0;
  state.lastlimit = 0;
  state.lasttotal = 0;
  state.max = 0;
  state.members = 0;
  state.total = 0;

  state.totltime = 0;

  state.pctival = (float) DEFAULT_INTERVAL;
}


void calcivals(void) {
/* Calculate sieve limits and report interval position and sizes. */
  volat.sqrtmax = sqrt(state.max) + 1;
  volat.ivalsize = state.pctival / 100 * state.max;

  if (!volat.ivalsize)
    die("SMAL specified interval percentage too small");
}


/* Initialise the run-time parameters structure. */
void initvolat(void) {
  inittimeused();

  volat.primesival = 0;

  volat.ivaltime = 0;

  volat.balance = 0;
  volat.done = 0;
  volat.ofival = 0;
  volat.ofsofar = 0;
  volat.primesivaltime = 0;
  volat.primestotltime = 0;
  volat.ivalipersec = 0;
  volat.totlipersec = 0;

  volat.startivaltime = 0;

  volat.gotsig = 0;

  calcivals();

  volat.limit = state.i / volat.ivalsize * volat.ivalsize;

  logu("S ISMX " UL " theoretical sieve max", state.max);
  logfl("S IVAL %.1f%%%% report intervals", state.pctival);
}


/* Calculate statistics prepartory to doing a report. */
static void calcinterval(void) {
  volat.ivaltime = volat.timeused() - volat.startivaltime;
  state.totltime += volat.ivaltime;
  volat.primesival = state.total - state.lasttotal;

  volat.ofival = safediv(volat.primesival, state.i - state.lastlimit);
  volat.ofsofar = safediv(state.total, state.i);

  volat.primesivaltime = persecond(volat.primesival, volat.ivaltime);
  volat.primestotltime = persecond(state.total, state.totltime);

  volat.ivalipersec = persecond(state.i - state.lastlimit, volat.ivaltime);
  volat.totlipersec = persecond(state.i, state.totltime);

  volat.done = safediv(state.i, state.max);

  state.lasttotal = state.total;
  state.lastlimit = state.i;
}


/* Log out the statistics. */
static void loginterval(void) {
  logblank();

  logpct("S DONE %.1f%%%% completed", volat.done);

  logu("P HIGH " UL " highest prime so far", state.highest);

  logu("P LAST " UL " found since last", volat.primesival);
  logpct("P HITS %.3f%%%% of those since last prime", volat.ofival);
  logu("P TOTL " UL " found so far", state.total);
  logpct("P AGGR %.3f%%%% of so far prime", volat.ofsofar);

  logfl("R IPPS %.1f primes/second since last",
    volat.primesivaltime);
  logfl("R TPPS %.1f primes/second so far", volat.primestotltime);
  logfl("R IIPS %.1f i/second since last", volat.ivalipersec);
  logfl("R TIPS %.1f i/second so far", volat.totlipersec);

  logt("T ITIM %s since last", volat.ivaltime);
  logt("T TTIM %s so far", state.totltime);
}


/* Calculate and log out a report. */
void doreport(void) {
  calcinterval();
  loginterval();
}


/* Log out a report when the structure reaches its maximum memory
   allocation. */
void domaxreport(void) {
  const cardinal kilobyte = 1024;

  logblank();

  logu("S ASMX " UL " adjusted sieve max", state.max);
  logu("S NODE " UL " nodes in sieve at maximum size", state.members);
  logfl("S SPAC %.1f kilobytes occupied by sieve", 
    (float) sizeof(sieve) * state.members / kilobyte);

  doreport();
}


/* Write out the current state of the sieve so it may be continued later. */
void dumpstate(void) {
  if ((statefile = fopen(options.statefilename, "w")))  {
    fprintf(statefile, "%f " UL "\n", VERSION, options.sievemax);
    fprintf(statefile, UL " " UL " %f\n", state.i, state.max, state.pctival);
    fprintf(statefile, UL " " UL "\n", state.isfull, state.members);
    fprintf(statefile, UL " " UL " " UL " " UL " " UL "\n",
      state.total, state.lasttotal, state.highest, state.lasthighest,
      state.lastlimit);
    fprintf(statefile, "%f\n", state.totltime); 
  
    dumpsieve();

    logpct("S FLAT %.2f%%%% sieve structure flatness", volat.balance);

    fclose(statefile);
  } else
    die("FILE couldn't write to state file");
}


/* Read in the state of a saved sieve, or do nothing if the file
   cannot be opened for reading. */
void readstate(void) {
  float statemax, version;

  if ((statefile = fopen(options.statefilename, "r"))) {
    fscanf(statefile, "%f %f", &version, &statemax);

/* Make sure that the statefile was written by the same version of the
   the program as the one running. */
   if (version != VERSION)
     die("VERS incompatible state file version");

/* Check that the state file contains integers small enough to be used
   in the copy of the program currently compiled. */
    if (options.sievemax < statemax)
      die("CMAX incompatible state file cardinal type");
    
    fscanf(statefile, UL " " UL " %f", &state.i, &state.max, &state.pctival);
    fscanf(statefile, UL " " UL, &state.isfull, &state.members);
    fscanf(statefile, UL " " UL " " UL " " UL " " UL,
      &state.total, &state.lasttotal, &state.highest, &state.lasthighest,
      &state.lastlimit);
    if (fscanf(statefile, "%f" , &state.totltime) < 0)
      die("FILE couldn't read from state file header");

    readsieve();
    
    fclose(statefile);
    logs("I READ read state file");
  }
}
