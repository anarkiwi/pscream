#include "pscream.h"


/* Check to see if number is prime by brute-force.

   Number is divided first by the first-defined prime, 2, and then
   every odd number up to the square-root of the number. If it survives
   every division with a remainder, then the number is prime. */
int main(void) {
  cardinal sqrti, i, j = FIRSTPRIME;

  printf("The number you'd like to test is? ");
  scanf(UL, &i);

  sqrti = sqrt(i) + 1;

  if (i % FIRSTPRIME)
    for (j = FIRSTPRIME + 1; (i % j) && (j < sqrti); j += FIRSTPRIME);

  printf("\n" UL " is ", i);

  if (!(i % j))
    printf("NOT (was divisible by " UL ", giving " UL ") ", j, i / j);

  printf("prime.\n");

  return EXIT_SUCCESS;
}
