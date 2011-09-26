/*
@author Michal Rychlik, 233242
*/

#include "misc.h"

/*
 * @args
 *       response_seq - sequance number read from incoming message
 *       response_id - id number read from incomint message
 *       my_pid - id of process this program is run in
 *       seq - first free sequence number (not sent yet)
 *
 * @return_value
 *       1 if message was sent to the program (id is equal to my_pid) and it is
 *         not an old one
 *       0 otherwise
 */

int is_packet_ok(int response_seq, int response_id, int my_pid, int seq) {
    return (response_seq >= seq - PACKETS_PER_ITERATION &&
            response_seq < seq && response_id == my_pid);
}

/*
 * @args
 *       len_sender_addrs - length of sended_addrs array
 *       sended_addrs - array of strings with ips that responded to echo request
 *       new_addr - respondend ip address to be tested
 * @return_value
 *       1 if new_addr was not recorded before (is not in sender_addrs array)
 *       0 otherwise
 */

int is_new_address(int len_sender_addrs, char ** sender_addrs, char * new_addr) {
    int i;
    for (i = 0; i < len_sender_addrs; ++i) {
        if (strcmp(sender_addrs[i], new_addr) == 0) {
            return 0;
        }
    }
    return 1;
}

/*
 *@args
 *      x, y - timeval structures to be substracted
 * @results
 *      result - (x - y)
 */

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

/*
 * @args
 *       tv - timeval struct to be converted to miliseconds
 *
 * @return_value
 *       Value of arg in miliseconds
 *
 */

double printable_timeval(struct timeval tv) {
    double dt;
    dt = (double)tv.tv_sec * 1000.0 +
	 (double)tv.tv_usec / 1000.0;
    return (dt);
}

/*
 * @args
 *         times - array of 3 timevals
 * @result
 *         average of 3 timevals in miliseconds
 */
double compute_average_time(struct timeval * times) {
    double sum = 0.0;
    int i;
    for (i = 0; i < PACKETS_PER_ITERATION; ++i) {
        sum += printable_timeval(times[i]);
    }
    return (sum / PACKETS_PER_ITERATION);
}
