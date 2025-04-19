#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

// Network functions
int connect_to_peer(const char *ip, int port);
void *listen_for_connections(void *arg);
bool send_message(int conn_id, const char *message);
void display_my_ip();
void display_my_port();

#endif /* NETWORK_H */