#ifndef COMMON_H
#define COMMON_H

// Standard library includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>

// Network includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>

// Threading includes
#include <pthread.h>

// Constants
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024
#define MAX_MESSAGE_SIZE 100
#define LISTEN_BACKLOG 50

// Global variables (declared as extern to be defined in appropriate files)
extern int listen_port;
extern int server_socket;
extern bool running;

// Signal handler prototype
void signal_handler(int sig);

#endif /* COMMON_H */