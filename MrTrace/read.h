#ifndef READ_H
#define	READ_H

int read_data(int sockfd, char ** sender_addrs, int * len_sender_addrs,
              struct timeval * times, int * len_times, int my_pid,
              int seq_no, struct timeval * timevals, char * ip_addr, int ttl);
void wait_a_second(int sockfd, char ** sender_addrs, int * len_sender_addrs,
                   struct timeval * times, int * len_times, int my_pid, int seq_no,
                   struct timeval * timevals, char * ip_addr, int ttl);

#endif	/* READ_H */

