/* Primal Scream 1.0 prime number generator.
   (C) 1994-1995 Josh Bailey, josh@tutor.co.nz.

   Updated by Ewen McNeill <ewen@naos.co.nz>, 1999/8/28 for new stack protos.

   See copyright and disclaimer in file pscream.h in this directory. */

cardinal mincard(cardinal a, cardinal b);
float safediv(cardinal a, cardinal b);
prestacktype *initprestack(sieve **ptrfirst);
poststacktype *initpoststack(sieve *first);
sieve **searchsieve(const cardinal key);
sieve *initsievenode(const cardinal multiple, const cardinal prime);
sieve *postpull(poststacktype *stack, cardinal *height);
sieve *readnode(sieve **parent);
sieve *removesieve(sieve **parent);
void calcivals(void);
void die(const char *info);
void domaxreport(void);
void done(void);
void donelog(void);
void donepoststack(poststacktype *stack);
void doneprestack(prestacktype *stack);
void donesieve(void);
void donesig(void);
void doreport(void);
void dumpsieve(void);
void dumpstate(void);
void findprimes(void);
void getoptions(int argc, char * const *argv);
void handlesig(void);
void initlog(void);
void initmystate(void);
void initoptions(void);
void initsieve(void);
void initsig(void);
void initstruct(cardinal n);
void initvolat(void);
void logblank(void);
void logfl(const char *info, float floatinfo);
void logp(const char *info, float seconds);
void logpct(const char *info, float floatinfo);
void logs(const char *info);
void logt(const char *info, float totalseconds);
void logu(const char *info, cardinal uinfo);
void postpush(poststacktype *stack, sieve *curr, const cardinal height);
void prepull(prestacktype *stack, sieve ***parent);
void prepush(prestacktype *stack, sieve **parent);
void processoptions(void);
void readsieve(void);
void readstate(void);
void sieveout(cardinal a, cardinal b);
