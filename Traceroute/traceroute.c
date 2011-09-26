/*
@author Michal Rychlik, 233242
*/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <unistd.h>
#include "sockwrap.h"
#include "icmp.h"
#include "misc.h"
#include "read.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: ./traceroute <ip_addr>\n");
        exit(EXIT_FAILURE);
    }
    // Variables declaration
    int ttl, j;
    char * ip_addr = argv[1];
    int seq_no = 1;
    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int my_pid = getpid();
    struct timeval timevals[PACKETS_PER_ITERATION];
    char * sender_addrs[PACKETS_PER_ITERATION];
    struct timeval times[PACKETS_PER_ITERATION];
    struct sockaddr_in remote_address;
    struct icmp icmp_packet;

    bzero(&remote_address, sizeof(remote_address));
    remote_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_addr, &remote_address.sin_addr) != 1) {
        printf("Provide valid ip_addr of form xxx.xxx.xxx.xxx\n");
        exit(EXIT_FAILURE);
    };
    
    icmp_packet.icmp_type = ICMP_ECHO;
    icmp_packet.icmp_code = 0;
    icmp_packet.icmp_id = my_pid;		
    for (ttl = 1; ttl <= ITERATIONS; ++ttl) {
        // Send PACKETS_PER_ITERATION echo requests with given ttl
        for (j = 0; j < PACKETS_PER_ITERATION; ++j) {
            icmp_packet.icmp_seq = seq_no;
            gettimeofday(&timevals[j], NULL);
            ++seq_no;
            icmp_packet.icmp_cksum = 0;
            icmp_packet.icmp_cksum = in_cksum((u_short*)&icmp_packet, 8, 0);
            Setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
            Sendto(sockfd, &icmp_packet, ICMP_HEADER_LEN, 0, &remote_address, sizeof(remote_address));
        }
        
        // Wait for responses
        int len_sender_addrs = 0;
        int len_times = 0;
        wait_a_second(sockfd, sender_addrs, &len_sender_addrs, times, &len_times,
                      my_pid, seq_no, timevals, ip_addr, ttl);
        printf("%d. ", ttl);

        // Parse and display results
        if (len_sender_addrs > 0) {
            int i;
            for(i = 0; i < len_sender_addrs; ++i) {
                printf("%s ", sender_addrs[i]);
            }
            if (len_times == PACKETS_PER_ITERATION) {
                double avarage_time = compute_average_time(times);
                printf("%g ms\n", avarage_time);
            } else {
                printf("???\n");
            }
        } else {
            printf("*\n");
        }

        // Clear received messages
        for (j = 0; j < PACKETS_PER_ITERATION; ++j) {
            sender_addrs[j] = NULL;
        }
    }
    return (EXIT_SUCCESS);
}

