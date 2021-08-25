/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 to add extra help opt.

   See copyright and disclaimer in file pscream.h in this directory. */

#include "pscream.h"


/* Display a help screen in the case of badly-specified options. */
static void displayhelp(const char *progname) {
  printf("prime number generator:" BANNER "\n\n", VERSION);

  printf(
    "usage: %s [-V] [-c] [-h] [-i ival] [-l log] [-m max] [-s state] [-o]",
    progname);

  printf("\n\n-V\tprint version.\n");
  printf("-M\tsieve to compiled-in theoretical maximum\n");
  printf("-c\tprint compiled-in theoretical maximum (is " UL ").\n",
   options.sievemax);
  printf("-h\tprint this help screen.\n");
  printf("-i\tlogging interval, defaults to %.1f%%.\n", 
    (float) DEFAULT_INTERVAL);
  printf("-l\tlog file name, defaults to \"%s\".\n", DEFAULT_LOGFILE);
  printf("-m\tsieve maximum.\n");
  printf("-s\tstate file name, defaults to \"%s\".\n", DEFAULT_STATEFILE);
  printf("-o\toverride any existing state file.\n");

  exit(EXIT_SUCCESS);
}


/* Set default command line options. */
void initoptions(void) {
  strcpy(options.logfilename, DEFAULT_LOGFILE);
  strcpy(options.statefilename, DEFAULT_STATEFILE);

  options.ignorestatefile = FALSE;
  options.max = 0;
  options.pctival = 0.0;
  options.sievemax = CARD_MAX - (float) sqrt(CARD_MAX) * 2;
}


#define OPTSTRING	"VMch?i:l:m:s:o"

/* Parse command line options into the options structure.  This function
   *requires* an implementation of getopt. */
void getoptions(int argc, char * const *argv) {
  int mychar;
  extern char *optarg;

  while ((mychar = getopt(argc, argv, OPTSTRING)) != EOF)
    switch (mychar) {
      case 'V':
/* Just print our version number if asked. */
        printf(VERSTR "\n", VERSION);
        exit(EXIT_SUCCESS);

      case 'M':
        options.max = options.sievemax;
        break;

      case 'c':
/* Print out compiled maximum if asked. */
        printf(UL "\n", options.sievemax);
        exit(EXIT_SUCCESS);

/* Accept a reporting percentage interval. */
      case 'i':
        options.pctival = atof(optarg);
        if (options.pctival <= 0 || options.pctival > 100)
          die("OIVL specified percentage interval out of range");
        break;

/* Accept a logfile name. */
      case 'l':
        strcpy(options.logfilename, optarg);
        break;

/* Accept a sieve maximum limit. */
      case 'm':
        if (options.sievemax < atof(optarg))
          die("OMAX specified maximum larger than cardinal maximum");
        else
          options.max = strtoul(optarg, (char**)NULL, 10);
        break;

/* Accept a statefile name. */
      case 's':
        strcpy(options.statefilename, optarg);
        break;

/* Accept a "override" existing statefile flag. */
      case 'o':
        options.ignorestatefile = TRUE;
        break;

/* Print a help screen if asked or in the case of an unrecognised option. */
      case 'h':
      case '?':
      default:
        displayhelp(argv[0]);
    }
}


/* Validate the given options. */
void processoptions(void) {
/* If we are not to ignore and existing statefile, then try to read it in. */
  if (!options.ignorestatefile)
    readstate();

/* If a sieve-limit was specified: */
  if (options.max) {
/* Then make sure it does not conflict with any in the state-file, if it
   was read in. */
    if (state.max) {  
/* We can increase the maximum if the sieve wasn't full.  Otherwise, we're
   limited by the existing sieve. */
      if (options.max > state.max && state.isfull)
        die("OMAX specified maximum greater than in state file");

      if (options.max < state.i)
        die("UMAX specified maximum already past in state file");
    }

/* Having a maximum smaller than the current size sieve can cope with will
   slow the program, though not invalidate its use. */
    if (options.max < state.max)
      logs("I SLOW excessive specified maximum");

    state.max = options.max;
  }

  if (options.pctival)
    state.pctival = options.pctival;

/* If neither a statefile could be read in, nor a maximum specified, then
   we have nothing to do. */
  if (!state.max)
    die("NMAX no sieve maximum specified");
}
