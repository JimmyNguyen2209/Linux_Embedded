/* include/sensor_node.h */
#ifndef SENSOR_NODE_H
#define SENSOR_NODE_H

#include "common.h"

// Initialize sensor node
void init_sensor_node(const char *sensorId, const char *serverIp, int port);

// Connect to data manager
int connect_to_data_manager(const char *serverIp, int port);

// Send sensor data
void send_sensor_data(int socket_fd, const char *sensorId, const char *data);

// Receive status updates
void receive_status_updates(int socket_fd);

#endif /* SENSOR_NODE_H */

/* src/sensor_node.c */
#include "../include/sensor_node.h"
#include "../include/tcp_connection.h"

void init_sensor_node(const char *sensorId, const char *serverIp, int port) {
    log_message("SensorNode", "Initializing sensor node");
    
    // Connect to data manager
    int socket_fd = connect_to_data_manager(serverIp, port);
    if (socket_fd < 0) {
        error("Failed to connect to data manager");
    }
    
    log_message("SensorNode", "Connected to data manager");
    
    // Send connection request
    Message msg;
    msg.type = MSG_CONNECT_REQUEST;
    msg.timestamp = time(NULL);
    strncpy(msg.sensorId, sensorId, MAX_SENSOR_NAME - 1);
    msg.sensorId[MAX_SENSOR_NAME - 1] = '\0';
    strncpy(msg.data, "CONNECT_REQUEST", MAX_DATA_SIZE - 1);
    msg.data[MAX_DATA_SIZE - 1] = '\0';
    
    if (send_message(socket_fd, &msg) < 0) {
        close_connection(socket_fd);
        error("Failed to send connection request");
    }
    
    log_message("SensorNode", "Sent connection request");
    
    // Main loop - in a real sensor this would read actual sensor data
    while (1) {
        // Simulate reading sensor data
        char data[MAX_DATA_SIZE];
        sprintf(data, "SENSOR_VALUE=%d", rand() % 100);
        
        // Send data
        send_sensor_data(socket_fd, sensorId, data);
        
        // Receive status updates
        receive_status_updates(socket_fd);
        
        // Wait before next reading
        sleep(5);
    }
    
    close_connection(socket_fd);
}

int connect_to_data_manager(const char *serverIp, int port) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverIp, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    
    return sock;
}

void send_sensor_data(int socket_fd, const char *sensorId, const char *data) {
    Message msg;
    msg.type = MSG_DATA;
    msg.timestamp = time(NULL);
    strncpy(msg.sensorId, sensorId, MAX_SENSOR_NAME - 1);
    msg.sensorId[MAX_SENSOR_NAME - 1] = '\0';
    strncpy(msg.data, data, MAX_DATA_SIZE - 1);
    msg.data[MAX_DATA_SIZE - 1] = '\0';
    
    if (send_message(socket_fd, &msg) < 0) {
        log_message("SensorNode", "Failed to send data");
    } else {
        log_message("SensorNode", "Sent sensor data");
    }
}

void receive_status_updates(int socket_fd) {
    Message msg;
    fd_set readfds;
    struct timeval tv;
    int activity;
    
    // Set timeout for select
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    
    // Clear the socket set
    FD_ZERO(&readfds);
    
    // Add socket to set
    FD_SET(socket_fd, &readfds);
    
    // Wait for response with timeout
    activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);
    
    if (activity < 0) {
        perror("select error");
        return;
    }
    
    if (activity > 0) {
        if (FD_ISSET(socket_fd, &readfds)) {
            // Read message
            if (receive_message(socket_fd, &msg) > 0) {
                if (msg.type == MSG_STATUS) {
                    log_message("SensorNode", "Received status update");
                    log_message("SensorNode", msg.data);
                }
            }
        }
    }
}