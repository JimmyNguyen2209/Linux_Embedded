#include "../inc/commands.h"
#include "../inc/connection.h"
#include "../inc/network.h"

void display_help() {
    printf("\n----- P2P Chat Application Help -----\n");
    printf("Available commands:\n");
    printf("  help                      - Display this help message\n");
    printf("  myip                      - Display your IP address\n");
    printf("  myport                    - Display the port you're listening on\n");
    printf("  connect <ip> <port>       - Connect to a peer\n");
    printf("  list                      - List all active connections\n");
    printf("  terminate <conn_id>       - Terminate a specific connection\n");
    printf("  send <conn_id> <message>  - Send a message to a specific peer\n");
    printf("  exit                      - Exit the application\n");
    printf("------------------------------------\n");
}

void process_command(char *input) {
    char cmd[20];
    
    // Parse command
    if (sscanf(input, "%19s", cmd) != 1) {
        return;
    }
    
    if (strcmp(cmd, "help") == 0) {
        display_help();
    } else if (strcmp(cmd, "myip") == 0) {
        display_my_ip();
    } else if (strcmp(cmd, "myport") == 0) {
        display_my_port();
    } else if (strcmp(cmd, "connect") == 0) {
        char ip[INET_ADDRSTRLEN];
        int port;
        if (sscanf(input, "%*s %15s %d", ip, &port) != 2) {
            printf("Error: Invalid arguments. Usage: connect <ip> <port>\n");
            return;
        }
        connect_to_peer(ip, port);
    } else if (strcmp(cmd, "list") == 0) {
        list_connections();
    } else if (strcmp(cmd, "terminate") == 0) {
        int conn_id;
        if (sscanf(input, "%*s %d", &conn_id) != 1) {
            printf("Error: Invalid arguments. Usage: terminate <connection_id>\n");
            return;
        }
        terminate_connection(conn_id);
    } else if (strcmp(cmd, "send") == 0) {
        int conn_id;
        char *message = NULL;
        char *token = strtok(input, " ");  // Get "send"
        if (token != NULL) {
            token = strtok(NULL, " ");  // Get connection ID
            if (token != NULL) {
                conn_id = atoi(token);
                message = strtok(NULL, "");  // Get the rest as message
                if (message != NULL) {
                    // Skip leading whitespace
                    while (*message == ' ') {
                        message++;
                    }
                    send_message(conn_id, message);
                } else {
                    printf("Error: No message provided. Usage: send <connection_id> <message>\n");
                }
            } else {
                printf("Error: Missing connection ID. Usage: send <connection_id> <message>\n");
            }
        }
    } else if (strcmp(cmd, "exit") == 0) {
        exit_program();
    } else {
        printf("Unknown command. Type 'help' for a list of commands.\n");
    }
}

void exit_program() {
    running = false;
    
    // Close all connections
    pthread_mutex_lock(&connections_mutex);
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].is_active) {
            close(connections[i].socket);
            connections[i].is_active = false;
        }
    }
    pthread_mutex_unlock(&connections_mutex);
    
    // Close the server socket
    if (server_socket != -1) {
        close(server_socket);
    }
    
    printf("All connections closed. Exiting...\n");
}