#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"

// Connection structure
typedef struct {
    int id;
    int socket;
    char ip[INET_ADDRSTRLEN];
    int port;
    bool is_active;
} Connection;

// Global connections array
extern Connection connections[MAX_CONNECTIONS];
extern pthread_mutex_t connections_mutex;

// Connection management functions
int add_connection(int socket, const char *ip, int port);
bool terminate_connection(int conn_id);
void list_connections();
int find_free_connection_slot();

// Connection handler thread function
void *handle_connection(void *arg);

#endif /* CONNECTION_H */