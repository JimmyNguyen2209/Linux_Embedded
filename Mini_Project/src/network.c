#include "../inc/network.h"
#include "../inc/connection.h"
#include "../inc/utils.h"

// Global network variables
int server_socket = -1;

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

void *listen_for_connections(void *arg) {
    (void)arg;
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