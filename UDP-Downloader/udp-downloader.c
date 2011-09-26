#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "connection.h"
#include "misc.h"
#include "read.h"

#define CHUNKS_IN_QUEUE 100
#define CHUNK_SIZE 1000
#define TIMEOUT 2

char serv_addr[] = "aisd.ii.uni.wroc.pl";

typedef struct chunk_info {
    int start;
    int length;
    struct timeval to_wait;
} chunk_info;

int main(int argc, char** argv) {
    if (argc < 4) {
        printf("Too few arguments\n");
        return (EXIT_SUCCESS);
    }
    char * port = argv[1];
    char * output_file_name = argv[2];
    /*
        bytes_to_download = ~3000 -> CHUNKS_IN_QUEUE = 10
        bytes_to_download = ~300000 -> CHUNKS_IN_QUEUE = 100
    */
    int bytes_to_download;
    sscanf(argv[3], "%d", &bytes_to_download);
    int chunks_in_queue_based_on_bytes;
    if (bytes_to_download < 10000) {
        chunks_in_queue_based_on_bytes = 10;
    } else {
        chunks_in_queue_based_on_bytes = 100;
    }
    char * output_data = calloc(bytes_to_download, sizeof(char));
    FILE * output = fopen(output_file_name, "w");
    if (!output) {
        perror("ERROR fopen\n");
        exit(EXIT_FAILURE);
    }
    chunk_info * chunks_to_process = calloc((bytes_to_download / CHUNK_SIZE) +
                                            (bytes_to_download % CHUNK_SIZE > 0),
                                            sizeof(chunk_info));
    int i;
    for (i = 0; i < bytes_to_download / CHUNK_SIZE; ++i) {
        chunk_info ci;
        ci.start = i * CHUNK_SIZE;
        ci.length = CHUNK_SIZE;
        ci.to_wait.tv_sec = TIMEOUT;
        ci.to_wait.tv_usec = 0;
        chunks_to_process[i] = ci;
    }
    if (bytes_to_download % CHUNK_SIZE > 0) {
        chunk_info ci;
        ci.start = i * CHUNK_SIZE;
        ci.length = bytes_to_download % CHUNK_SIZE;
        ci.to_wait.tv_sec = TIMEOUT;
        ci.to_wait.tv_usec = 0;
        chunks_to_process[i] = ci;
    }
    int chunks_to_send = (bytes_to_download / CHUNK_SIZE) +
                         (bytes_to_download % CHUNK_SIZE > 0);
    chunk_info * chunk_queue = calloc(chunks_in_queue_based_on_bytes, sizeof(chunk_info));
    int chunks_queued = 0;
    int sock = create_connected_socket(serv_addr, port);
    char * msg;
    for (i = 0; i < chunks_in_queue_based_on_bytes && i < chunks_to_send; ++i) {
        chunk_queue[i] = chunks_to_process[chunks_queued++];
        msg = prepare_message(chunk_queue[i].start, chunk_queue[i].length);
        send_msg(sock, msg);
        free(msg);
    }
    struct timeval min_to_wait;
    min_to_wait.tv_sec = TIMEOUT;
    min_to_wait.tv_usec = 0;
    int chunks_downloaded = 0;
    char * chunks_to_resend = calloc(chunks_in_queue_based_on_bytes, sizeof(char));
    while (chunks_downloaded < chunks_to_send) {
        int r_start;
        int r_length;
        struct timeval start, stop, delta, tmp;
        gettimeofday(&start, NULL);
        /* WAIT FOR min_to_wait time for any response */
        char * chunk = wait_for_response(sock, min_to_wait, &r_start, &r_length);
        gettimeofday(&stop, NULL);
        timeval_subtract(&delta, &stop, &start);
        min_to_wait.tv_sec = TIMEOUT;
        min_to_wait.tv_usec = 0;
        /* Update timeouts for chunks in queue */
        for (i = 0; i < chunks_in_queue_based_on_bytes && i < chunks_to_send; ++i) {
            if (timeval_subtract(&chunk_queue[i].to_wait, &chunk_queue[i].to_wait, 
                &delta) == 1) {
                chunks_to_resend[i] = 1;
                chunk_queue[i].to_wait.tv_sec = TIMEOUT;
                chunk_queue[i].to_wait.tv_usec = 0;
            } else {
                chunks_to_resend[i] = 0;
            }
            /* Update min time to wait for chunks in queue */
            struct timeval tmp_min;
            tmp_min.tv_sec = min_to_wait.tv_sec;
            tmp_min.tv_usec = min_to_wait.tv_usec;
            if (timeval_subtract(&tmp, &chunk_queue[i].to_wait, &tmp_min) == 1) {
                min_to_wait = chunk_queue[i].to_wait;
            }
        }
        /* Some msg received */
        if (chunk) {
            /* Check if we are waiting for such message */
            for (i = 0; i < chunks_in_queue_based_on_bytes && i < chunks_to_send; ++i) {
                if (chunk_queue[i].start == r_start && 
                    chunk_queue[i].length == r_length) {
                    printf("Downloaded %d%%\n", (chunks_downloaded + 1) * 100 / chunks_to_send);
                    memcpy(output_data + r_start, chunk, r_length);
                    if (++chunks_downloaded < chunks_to_send && chunks_queued < chunks_to_send) {
                        chunk_queue[i] = chunks_to_process[chunks_queued++];
                        msg = prepare_message(chunk_queue[i].start, chunk_queue[i].length);
                        send_msg(sock, msg);
                        chunks_to_resend[i] = 0;
                        free(msg);
                    } else {
                        /* Prevent from reading duplicates again */
                        chunk_queue[i].start = -1;
                        chunk_queue[i].length = -1;
                    }
                    break;
                }
            }
            free(chunk);
        }
        for (i = 0; i < chunks_in_queue_based_on_bytes && i < chunks_to_send; ++i) {
            if (chunks_to_resend[i]) {
                msg = prepare_message(chunk_queue[i].start, chunk_queue[i].length);
                send_msg(sock, msg);
                chunks_to_resend[i] = 0;
                free(msg);
            }
        }
    }
    fwrite(output_data, bytes_to_download, sizeof(char), output);
    free(output_data);
    fclose(output);
    return (EXIT_SUCCESS);
}

