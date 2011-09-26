#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

struct addrinfo * prepare_addrinfo(const char * addr, const char * port) {
        int status;
        struct addrinfo hints;
        struct addrinfo * servinfo;
        memset(&hints, 0, sizeof hints); // make sure the struct is empty
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM; // UDP sockets
        status = getaddrinfo(addr, port, &hints, &servinfo);
        if (status < 0) {
            fprintf(stderr, "ERROR, getaddrinfo\n");
            exit(EXIT_FAILURE);
        }
        return servinfo;
}

int create_connected_socket(const char * addr, const char * port) {
        struct addrinfo * res = prepare_addrinfo(addr, port);
        int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s < 0) {
            fprintf(stderr, "ERROR, socket\n");
            exit(EXIT_FAILURE);
        }
        if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
            fprintf(stderr, "ERROR, connect\n");
            exit(EXIT_FAILURE);
        }
        return s;
}

void send_msg(int connected_socket, char * msg) {
        int bytes_to_send = strlen(msg);
        int bytes_sent;
        while (bytes_to_send) {
                bytes_sent = send(connected_socket, msg, strlen(msg), 0);
                if (bytes_sent < 0) {
                    fprintf(stderr, "ERROR, send\n");
                    exit(EXIT_FAILURE);
                }
                bytes_to_send -= bytes_sent;
                msg += bytes_sent;
        }
}

