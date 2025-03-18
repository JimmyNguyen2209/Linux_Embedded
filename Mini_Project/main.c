#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <signal.h>
#include <stdbool.h>

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024
#define MAX_MESSAGE_SIZE 100
#define LISTEN_BACKLOG 50

typedef struct {
    int id;
    int socket;
    char ip[INET_ADDRSTRLEN];
    int port;
    bool is_active;
} Connection;

Connection connections[MAX_CONNECTIONS];
pthread_mutex_t connections_mutex = PTHREAD_MUTEX_INITIALIZER;
int listen_port = 0;
int server_socket = -1;
bool running = true;

// Function prototypes
void display_help();
void display_my_ip();
void display_my_port();
int connect_to_peer(const char *ip, int port);
void list_connections();
bool terminate_connection(int conn_id);
bool send_message(int conn_id, const char *message);
void exit_program();
void *handle_connection(void *arg);
void *listen_for_connections(void *arg);
bool is_valid_ip(const char *ip);
int add_connection(int socket, const char *ip, int port);

// Signal handler for clean exit
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nReceived interrupt signal. Exiting gracefully...\n");
        exit_program();
        exit(0);
    }
}

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

void display_my_ip() {
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    // Find the first non-loopback IPv4 address
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {  // IPv4
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                           host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }

            // Skip loopback addresses
            if (strcmp(host, "127.0.0.1") != 0) {
                printf("Your IP: %s\n", host);
                freeifaddrs(ifaddr);
                return;
            }
        }
    }

    printf("No non-loopback IPv4 address found\n");
    freeifaddrs(ifaddr);
}

void display_my_port() {
    printf("Your Port: %d\n", listen_port);
}

bool is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

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

int connect_to_peer(const char *ip, int port) {
    // Input validation
    if (!is_valid_ip(ip)) {
        printf("Error: Invalid IP address format\n");
        return -1;
    }
    
    // Check for self-connection
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];
    bool is_self = false;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) {  // IPv4
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                           host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                continue;
            }

            if (strcmp(ip, host) == 0 && port == listen_port) {
                is_self = true;
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    
    if (is_self) {
        printf("Error: Cannot connect to yourself\n");
        return -1;
    }
    
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Set up the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }
    
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    
    // Add to connections list
    int conn_id = add_connection(sock, ip, port);
    if (conn_id < 0) {
        printf("Error: Could not add connection. Check for duplicates or if maximum connections reached.\n");
        close(sock);
        return -1;
    }
    
    // Create a thread to handle this connection
    pthread_t thread_id;
    int *arg = malloc(sizeof(int));
    if (!arg) {
        perror("malloc");
        terminate_connection(conn_id);
        return -1;
    }
    *arg = conn_id;
    
    if (pthread_create(&thread_id, NULL, handle_connection, arg) != 0) {
        perror("pthread_create");
        free(arg);
        terminate_connection(conn_id);
        return -1;
    }
    pthread_detach(thread_id);
    
    printf("Connected to %s:%d with connection ID %d\n", ip, port, conn_id);
    return conn_id;
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

bool send_message(int conn_id, const char *message) {
    pthread_mutex_lock(&connections_mutex);
    
    if (conn_id < 0 || conn_id >= MAX_CONNECTIONS || !connections[conn_id].is_active) {
        pthread_mutex_unlock(&connections_mutex);
        printf("Error: Invalid connection ID\n");
        return false;
    }
    
    // Check message length
    if (strlen(message) > MAX_MESSAGE_SIZE) {
        pthread_mutex_unlock(&connections_mutex);
        printf("Error: Message exceeds maximum length of %d characters\n", MAX_MESSAGE_SIZE);
        return false;
    }
    
    int sock = connections[conn_id].socket;
    pthread_mutex_unlock(&connections_mutex);
    
    // Send the message
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("send");
        return false;
    }
    
    printf("Message sent successfully to connection %d\n", conn_id);
    return true;
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

void *listen_for_connections(void *arg) {
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (errno == EINVAL && !running) {
                // Server socket was closed because program is exiting
                break;
            }
            perror("accept");
            continue;
        }
        
        // Get client IP and port
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);
        
        printf("Incoming connection from %s:%d\n", client_ip, client_port);
        
        // Add to connections list
        int conn_id = add_connection(client_socket, client_ip, client_port);
        if (conn_id < 0) {
            printf("Error: Maximum connections reached. Rejecting connection.\n");
            close(client_socket);
            continue;
        }
        
        // Create a thread to handle this connection
        pthread_t thread_id;
        int *arg = malloc(sizeof(int));
        if (!arg) {
            perror("malloc");
            terminate_connection(conn_id);
            continue;
        }
        *arg = conn_id;
        
        if (pthread_create(&thread_id, NULL, handle_connection, arg) != 0) {
            perror("pthread_create");
            free(arg);
            terminate_connection(conn_id);
            continue;
        }
        pthread_detach(thread_id);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    // Set up signal handler
    signal(SIGINT, signal_handler);
    
    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    listen_port = atoi(argv[1]);
    if (listen_port <= 0 || listen_port > 65535) {
        printf("Error: Invalid port number. Port must be between 1 and 65535.\n");
        return EXIT_FAILURE;
    }
    
    // Initialize connections array
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        connections[i].is_active = false;
    }
    
    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_socket);
        return EXIT_FAILURE;
    }
    
    // Bind the socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(listen_port);
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        return EXIT_FAILURE;
    }
    
    // Listen for connections
    if (listen(server_socket, LISTEN_BACKLOG) < 0) {
        perror("listen");
        close(server_socket);
        return EXIT_FAILURE;
    }
    
    printf("P2P Chat Application started. Listening on port %d\n", listen_port);
    
    // Create a thread to handle incoming connections
    pthread_t listen_thread;
    if (pthread_create(&listen_thread, NULL, listen_for_connections, NULL) != 0) {
        perror("pthread_create");
        close(server_socket);
        return EXIT_FAILURE;
    }
    pthread_detach(listen_thread);
    
    // Display welcome message and help
    printf("Welcome to P2P Chat Application!\n");
    display_help();
    
    // Main command loop
    char input[BUFFER_SIZE];
    char cmd[20];
    
    while (running) {
        printf("> ");
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Remove trailing newline
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[len-1] = '\0';
        }
        
        // Parse command
        if (sscanf(input, "%19s", cmd) != 1) {
            continue;
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
                continue;
            }
            connect_to_peer(ip, port);
        } else if (strcmp(cmd, "list") == 0) {
            list_connections();
        } else if (strcmp(cmd, "terminate") == 0) {
            int conn_id;
            if (sscanf(input, "%*s %d", &conn_id) != 1) {
                printf("Error: Invalid arguments. Usage: terminate <connection_id>\n");
                continue;
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
            break;
        } else {
            printf("Unknown command. Type 'help' for a list of commands.\n");
        }
    }
    
    return EXIT_SUCCESS;
}