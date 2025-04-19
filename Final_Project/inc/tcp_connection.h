/* include/tcp_connection.h */
#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "common.h"

// Initialize TCP server
int init_tcp_server(int port);

// Accept new connection
int accept_connection(int server_fd);

// Send message over TCP
int send_message(int socket_fd, Message *msg);

// Receive message over TCP
int receive_message(int socket_fd, Message *msg);

// Close connection
void close_connection(int socket_fd);

#endif /* TCP_CONNECTION_H */

/* src/tcp_handler.c */
#include "../include/tcp_connection.h"

int init_tcp_server(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("socket failed");
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        error("setsockopt failed");
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("bind failed");
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        error("listen failed");
    }
    
    return server_fd;
}

int accept_connection(int server_fd) {
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        error("accept failed");
    }
    
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
    char log_msg[100];
    sprintf(log_msg, "New connection from %s:%d", client_ip, ntohs(address.sin_port));
    log_message("TCP", log_msg);
    
    return new_socket;
}

int send_message(int socket_fd, Message *msg) {
    int bytes_sent = send(socket_fd, msg, sizeof(Message), 0);
    if (bytes_sent < 0) {
        perror("send failed");
        return -1;
    }
    return bytes_sent;
}

int receive_message(int socket_fd, Message *msg) {
    int bytes_received = recv(socket_fd, msg, sizeof(Message), 0);
    if (bytes_received < 0) {
        perror("recv failed");
        return -1;
    } else if (bytes_received == 0) {
        // Connection closed
        return 0;
    }
    return bytes_received;
}

void close_connection(int socket_fd) {
    close(socket_fd);
}