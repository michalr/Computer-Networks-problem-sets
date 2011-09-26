#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define CONST_MSG_LENGTH 7

char * prepare_message(int start, int length) {
    /* GET start długość\n */
    int len_start = 1;
    if (start) {
        len_start += (int)log10(start);
    }
    int len_length = (int)log10(length) + 1;
    char * msg = calloc(CONST_MSG_LENGTH + len_start + len_length, sizeof(char));
    sprintf(msg, "GET %d %d\n", start, length);
    return msg;
}

int timeval_subtract (result, x, y)
     struct timeval *result, *x, *y;
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
     
double printable_timeval(struct timeval tv) {
    double dt;
    dt = (double)tv.tv_sec * 1000.0 +
	 (double)tv.tv_usec / 1000.0;
    return (dt);
}
