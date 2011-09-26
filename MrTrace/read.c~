/*
@author Michal Rychlik, 233242
*/

#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>

#include "read.h"
#include "misc.h"
#include "sockwrap.h"

/*
 * @args
 *       sockfd - socked to read from
 *       my_pid - id of process this program is run in
 *       seq_no - first free sequence number (not sent yet)
 *       timevals - moments in time echo requests were sent
 *       ip_addr - target ip
 *       ttl - last ttl value that packets where sent with
 *
 * @results
 *      sender_addrs - array of ips that responded
 *      len_sender_addrs - number of ips that responded
 *      times - array of moments in time that messages from responded where
 *              recevied in
 *      len_times - length of times array
 *
 *@return_value
 *      1 if target ip address was reached
 *      0 otherwise
 *
 */

int read_data(int sockfd, char ** sender_addrs, int * len_sender_addrs,
              struct timeval * times, int * len_times, int my_pid,
              int seq_no, struct timeval * timevals, char * ip_addr, int ttl) {
    unsigned char buffer[IP_MAXPACKET + 1];
    unsigned char* buffer_ptr = buffer;
    struct timeval tmp;
    Recvfrom(sockfd, buffer_ptr, IP_MAXPACKET, 0, NULL, NULL);
    struct ip* packet = (struct ip*) buffer_ptr;
    struct in_addr source = packet->ip_src;
    char str[20];
    inet_ntop(AF_INET, &(source), str, sizeof(str));
    buffer_ptr += sizeof(struct ip);
    struct icmp* icmp_packet = (struct icmp*) buffer_ptr;
    int response_type = icmp_packet->icmp_type;
    int response_code = icmp_packet->icmp_code;
    int response_seq;
    int response_id;
    // IF REPLY
    if (response_type == ICMP_ECHOREPLY) {
        response_seq = icmp_packet->icmp_seq;
        response_id = icmp_packet->icmp_id;
    } else {
        buffer_ptr += sizeof(struct icmp);
        struct icmp* icmp_packet_returned = (struct icmp*) buffer_ptr;
        response_seq = icmp_packet_returned->icmp_seq;
        response_id = icmp_packet_returned->icmp_id;
    }
    if (is_packet_ok(response_seq, response_id, my_pid, seq_no)) {
        // TLE
        if (response_type == ICMP_TIME_EXCEEDED &&
            response_code == ICMP_EXC_TTL) {
            sender_addrs[*len_sender_addrs] = calloc(IP_ADDR_LEN, sizeof(char));
            memcpy(sender_addrs[*len_sender_addrs], str, IP_ADDR_LEN);
            if (is_new_address(*len_sender_addrs, sender_addrs, sender_addrs[*len_sender_addrs])) {
                *len_sender_addrs += 1;
            } else {
                free(sender_addrs[*len_sender_addrs]);
                sender_addrs[*len_sender_addrs] = NULL;
            }
            gettimeofday(&tmp, NULL);
            timeval_subtract(&times[*len_times], &tmp, &timevals[seq_no - response_seq - 1]);
            *len_times += 1;
        // ECHO REPLY
        } else {
            if (response_type == ICMP_ECHOREPLY) {
                char echo_source[IP_ADDR_LEN];
                inet_ntop(AF_INET, &(source), echo_source, sizeof(echo_source));
                if (strcmp(echo_source, ip_addr) == 0) {
                    gettimeofday(&tmp, NULL);
                    struct timeval final;
                    timeval_subtract(&final, &tmp, &timevals[seq_no - response_seq - 1]);
                    printf("%d. %s %g ms\n", ttl, echo_source, printable_timeval(final));
                    return 1;
                }
            }
        }
    }
    return 0;
}


/*
 *@args
 *      sockfd - socked to listen on
 *      my_pid - id of process this program is run in
 *      seq_no - first free sequence number (not sent yet)
 *      timevals - moments in time echo requests were sent
 *      ip_addr - target ip
 *      ttl - last ttl value that packets where sent with
 *
 * @results
 *      sender_addrs - array of ips that responded
 *      len_sender_addrs - number of ips that responded
 *      times - array of moments in time that messages from responded where
 *              recevied in
 *      len_times - length of times array
 *
 * Waits for a TIME_TO_WAIT amount of time using select (=non-blocking) for
 * responses. It DOESN'T rely on some UNIX-like systems select implementations,
 * that decrease timeout variable passed in select call. When socket is ready
 * for reading, read_data is called.
 */
void wait_a_second(int sockfd, char ** sender_addrs, int * len_sender_addrs,
                   struct timeval * times, int * len_times, int my_pid, int seq_no,
                   struct timeval * timevals, char * ip_addr, int ttl) {
    struct timeval tv;
    tv.tv_sec = TIME_TO_WAIT;
    tv.tv_usec = 0;
    while (printable_timeval(tv) > 0) {
        fd_set rfds;
        int retval;
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        struct timeval start, stop, tmp;
        gettimeofday(&start, NULL);
        retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);
        gettimeofday(&stop, NULL);
        if (retval == -1) {
            printf("select() error\n");
            exit(EXIT_FAILURE);
        } else {
           if (retval) {
                int reached = 0;
                reached = read_data(sockfd, sender_addrs, len_sender_addrs,
                times, len_times, my_pid, seq_no, timevals, ip_addr,ttl);
                if (reached) {
                    exit(EXIT_SUCCESS);
                }
           }
        }
        timeval_subtract(&tmp, &stop, &start);
        timeval_subtract(&tv, &tv, &tmp);
    }
}
