#include "../inc/connection.h"
#include "../inc/network.h"

// Global connection variables
Connection connections[MAX_CONNECTIONS];
pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;

int find_free_connection_slot() {
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (!connections[i].is_active) {
            return i;
        }
    }
    return -1;
}

int add_connection(int socket, const char *ip, int port) {
    pthread_mutex_lock(&connections_mutex);
    
    // Check if this is a duplicate connection
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].is_active && 
            strcmp(connections[i].ip, ip) == 0 && 
            connections[i].port == port) {
            pthread_mutex_unlock(&connections_mutex);
            return -1; // Duplicate connection
        }
    }
    
    int slot = find_free_connection_slot();
    if (slot == -1) {
        pthread_mutex_unlock(&connections_mutex);
        return -1; // No free slots
    }
    
    connections[slot].id = slot;
    connections[slot].socket = socket;
    strncpy(connections[slot].ip, ip, INET_ADDRSTRLEN);
    connections[slot].port = port;
    connections[slot].is_active = true;
    
    pthread_mutex_unlock(&connections_mutex);
    return slot;
}

bool terminate_connection(int conn_id) {
    pthread_mutex_lock(&connections_mutex);
    
    if (conn_id < 0 || conn_id >= MAX_CONNECTIONS || !connections[conn_id].is_active) {
        pthread_mutex_unlock(&connections_mutex);
        printf("Error: Invalid connection ID\n");
        return false;
    }
    
    // Close the socket
    close(connections[conn_id].socket);
    connections[conn_id].is_active = false;
    printf("Connection %d terminated\n", conn_id);
    
    pthread_mutex_unlock(&connections_mutex);
    return true;
}

void list_connections() {
    pthread_mutex_lock(&connections_mutex);
    printf("\n----- Active Connections -----\n");
    printf("ID\tIP Address\tPort\n");
    printf("---------------------------\n");
    
    bool found = false;
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].is_active) {
            printf("%d\t%s\t%d\n", i, connections[i].ip, connections[i].port);
            found = true;
        }
    }
    
    if (!found) {
        printf("No active connections\n");
    }
    printf("---------------------------\n");
    pthread_mutex_unlock(&connections_mutex);
}

void *handle_connection(void *arg) {
    int conn_id = *((int *)arg);
    free(arg);
    
    char buffer[BUFFER_SIZE];
    int bytes_read;
    
    pthread_mutex_lock(&connections_mutex);
    int sock = connections[conn_id].socket;
    char peer_ip[INET_ADDRSTRLEN];
    strcpy(peer_ip, connections[conn_id].ip);
    int peer_port = connections[conn_id].port;
    pthread_mutex_unlock(&connections_mutex);
    
    while (running) {
        bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
                printf("Peer %s:%d disconnected\n", peer_ip, peer_port);
            } else {
                perror("recv");
            }
            break;
        }
        
        buffer[bytes_read] = '\0';
        printf("Message from %s:%d: %s\n", peer_ip, peer_port, buffer);
    }
    
    terminate_connection(conn_id);
    return NULL;
}