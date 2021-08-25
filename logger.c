/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"

FILE *logfile;

/* Global variable used by the log formatting functions; each function
   sprintfs into this array. */
static char infolog[LINELEN];


/* Return a string containing the time in the local format. Removes the
   \n appended by ctime in the standard library. */
static char *myctime(void) {
  time_t currtime;

  time(&currtime);
  return strtok(ctime(&currtime), "\n");
}


/* Print a message on standard error, then abnormally terminate. */
void die(const char *info) {
  fprintf(stderr, "%s #E %s\n", myctime(), info);
  fflush(stderr);
  exit(EXIT_FAILURE);
}


/* Write out the fully-formatted logging information to both the log file 
   and standard output. */
static void writelog(const char *finallog) {
  if (fprintf(logfile, finallog) < 0 )
    die("FILE couldn't write to log file");
  else
    printf(finallog);
}


/* Combines both the contents of infolog and the current time, then write
   out the line by calling the above function. */   /* FIXME */
static void dolog(void) {
  static char finallog[LINELEN];

  sprintf(finallog, "%s #%s\n", myctime(), infolog);
  writelog(finallog);
}


/* Log just a blank line with no time or other information. */
void logblank(void) {
  writelog("\n");
}


/* Log out a string containing a percentage. */        /* FIXME */
void logpct(const char *info, float floatinfo) {
  sprintf(infolog, info, floatinfo * 100);
  dolog();
}


/* Log out a string containing a float value. */
void logfl(const char *info, float floatinfo) {
  sprintf(infolog, info, floatinfo);
  dolog();
}


/* Log out a string containing a value of type cardinal. */
void logu(const char *info, cardinal uinfo) {
  sprintf(infolog, info, uinfo);
  dolog();
}


/* Log out just an arbitary string. */
void logs(const char *info) {
  sprintf(infolog, info);
  dolog();
}


/* Log out a nicely human-readable time value given one in total seconds. */
void logt(const char *info, float totalseconds) {
  const cardinal secsinmin = 60;
  const cardinal secsinhour = 60 * secsinmin;
  const cardinal secsinday = 24 * secsinhour;

  char timeinfo[LINELEN], logstr[LINELEN];
  unsigned days = 0, hours = 0, minutes = 0;

/* Need to get absolute value because my library returns -0 for values very
   close to 0. */
  float seconds = totalseconds = fabs(totalseconds);

  days = seconds / secsinday; 
  seconds = seconds - days * secsinday;

  hours = seconds / secsinhour;
  seconds = seconds - hours * secsinhour;

  minutes = seconds / secsinmin;
  seconds = seconds - secsinmin * minutes;
   
  sprintf(timeinfo, "%.1f sec (%u days, %02u:%02u:%04.1f)", 
    totalseconds, days, hours, minutes, seconds);
  sprintf(logstr, info, timeinfo);

  logs(logstr);
}


/* Log out a primes-per-second value. */
void logp(const char *info, float seconds) {
  sprintf(infolog, info, seconds);
  dolog();
}


/* Open the log file and log an opening banner. */
void initlog(void) {
  logfile = fopen(options.logfilename, "a");

  setbuf(logfile, NULL);
  logblank();
  logfl("I HIYA" BANNER, VERSION);
}


/* Log out a closing banner, then close the log file. */
void donelog(void) {
  logfl("I BYBY" BANNER, VERSION);

  fclose(logfile);
}
