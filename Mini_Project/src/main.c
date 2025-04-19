#include "../inc/common.h"
#include "../inc/connection.h"
#include "../inc/network.h"
#include "../inc/commands.h"
#include "../inc/utils.h"

// Global variables
int listen_port = 0;
bool running = true;

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
        
        // Process the command
        process_command(input);
    }
    
    return EXIT_SUCCESS;
}