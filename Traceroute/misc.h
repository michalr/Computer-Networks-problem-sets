#ifndef MISC_H
#define	MISC_H

#include <sys/time.h>
#include <string.h>

#define IP_ADDR_LEN 20
#define ITERATIONS 30
#define PACKETS_PER_ITERATION 3
#define TIME_TO_WAIT 1

int is_packet_ok(int response_seq, int response_id, int my_pid, int seq);
int is_new_address(int len_sender_addrs, char ** sender_addrs, char * new_addr);
int timeval_subtract (struct timeval * result, struct timeval * x, struct timeval * y);
double printable_timeval(struct timeval tv);
double compute_average_time(struct timeval * times);


#endif	/* MISC_H */

