#include "../inc/utils.h"

bool is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived interrupt signal. Exiting gracefully...\n");
        exit(0);
    }
}