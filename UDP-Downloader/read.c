#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define MAX_HEADER_SIZE 20
#define CONST_RESP_LENGTH 7

char * read_msg(int sock, int * r_start, int * r_length) {
    char * header = calloc(MAX_HEADER_SIZE, sizeof(char));
    recv(sock, header, MAX_HEADER_SIZE, MSG_PEEK);
    int start;
    int length;
    int status = sscanf(header, "DATA %d %d\n", &start, &length);
    if (status == EOF || status < 2) {
        return NULL;
    }
    free(header);
    int len_start = 1;
    if (start) {
        len_start += (int)log10(start);
    }
    int len_length = (int)log10(length) + 1;
    int header_size = CONST_RESP_LENGTH + len_start + len_length;
    char * resp = calloc(header_size + length, sizeof(char));
    int msg_len = recv(sock, resp, header_size + length, 0);
    if (msg_len < header_size + length) {
        return NULL;
    }
    char * msg = calloc(length, sizeof(char));
    memcpy(msg, resp + header_size, length);
    free(resp);
    *r_start = start;
    *r_length = length;
    return msg;
}

char * wait_for_response(int sock, struct timeval tv, int * r_start, int * r_length) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);
    //printf("SELECT sock = %d tv.sec = %d tv.usec = %d\n", sock, tv.tv_sec, tv.tv_usec);
    if (select(sock + 1, &readfds, NULL, NULL, &tv) < 0) {
        perror("ERROR select\n");
        exit(EXIT_FAILURE);
    }
    if (FD_ISSET(sock, &readfds)) {
        char * chunk = read_msg(sock, r_start, r_length);
        return chunk;
    }
    return NULL;
}
