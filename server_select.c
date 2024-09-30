#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define PROC_STAT_PATH "/proc"

// Server file descriptor and client sockets array
int server_fd;
int client_socket[MAX_CLIENTS];

// Signal handler for SIGINT (Ctrl + C)
void handle_sigint(int sig) {
    printf("\nCaught signal %d. Shutting down server gracefully...\n", sig);

    // Close all client sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_socket[i] != 0) {
            close(client_socket[i]);
            client_socket[i] = 0;
        }
    }

    // Close server socket
    if (server_fd != 0) {
        close(server_fd);
        printf("Server socket closed.\n");
    }

    exit(0);
}

// Function to gather top 2 CPU-consuming processes
void get_top_cpu_processes(char *result) {
    struct dirent *entry;
    DIR *dp = opendir(PROC_STAT_PATH);
    if (dp == NULL) {
        perror("Error opening /proc");
        exit(EXIT_FAILURE);
    }

    long max1 = 0, max2 = 0;
    int pid1 = -1, pid2 = -1;
    char name1[256] = {0}, name2[256] = {0};
    long user1 = 0, sys1 = 0, user2 = 0, sys2 = 0;

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            char stat_file[300];
            snprintf(stat_file, sizeof(stat_file), "/proc/%s/stat", entry->d_name);

            FILE *file = fopen(stat_file, "r");
            if (file) {
                int pid;
                char comm[256];
                char state;
                long utime, stime;

                // Read pid, process name, and CPU times (user + system)
                fscanf(file, "%d %s %c", &pid, comm, &state);
                for (int i = 0; i < 11; i++) fscanf(file, "%*s"); // Skip to 14th field
                fscanf(file, "%ld %ld", &utime, &stime);
                fclose(file);

                long total_time = utime + stime;

                // Check if this process has more CPU time than the current maxes
                if (total_time > max1) {
                    max2 = max1;
                    pid2 = pid1;
                    strcpy(name2, name1);
                    user2 = user1;
                    sys2 = sys1;

                    max1 = total_time;
                    pid1 = pid;
                    strcpy(name1, comm);
                    user1 = utime;
                    sys1 = stime;
                } else if (total_time > max2) {
                    max2 = total_time;
                    pid2 = pid;
                    strcpy(name2, comm);
                    user2 = utime;
                    sys2 = stime;
                }
            }
        }
    }
    closedir(dp);


    snprintf(result, 1024, 
             "Top 2 CPU-consuming processes:\n1. PID: %d, Name: %s, User Time: %ld, System Time: %ld\n"
             "2. PID: %d, Name: %s, User Time: %ld, System Time: %ld\n", 
             pid1, name1, user1, sys1, pid2, name2, user2, sys2);
}

int main() {
    int new_socket, activity, i, sd;
    int max_sd;
    struct sockaddr_in address;
    fd_set readfds;

    signal(SIGINT, handle_sigint);

    // Initialize all client_socket[] to 0 so they're not checked initially
    for (i = 0; i < MAX_CLIENTS; i++) client_socket[i] = 0;

    // Create the server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    int addrlen = sizeof(address);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Clear the socket set and add server_fd to the set
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to the set
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];
            if (sd > 0) FD_SET(sd, &readfds);  // Add valid sockets
            if (sd > max_sd) max_sd = sd;      // Update max_sd for select()
        }

        // Wait for activity on one of the sockets (blocking)
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
        }

        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            
            printf("New connection:\nSocket FD: %d, IP: %s, Port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Check if there is space for this new client
            int client_assigned = 0;  // Flag to track if a client slot is available
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to client list as client #%d (Socket FD: %d)\n", i + 1, new_socket);
                    client_assigned = 1;  
                    break;
                }
            }

            // If no slot was found, the server is full
            if (!client_assigned) {
                // Inform the client the server is full
                char *full_message = "Server full: too many connections. Please try again later.\n";
                send(new_socket, full_message, strlen(full_message), 0);

                // Close the connection
                printf("Server full. Closing connection for new client on Socket FD: %d\n", new_socket);
                close(new_socket);
            }
        }


        // Check if any client sockets have incoming data
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                char buffer[1025];
                int valread;
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // Client disconnected
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Client disconnected:\nSocket FD: %d, IP: %s, Port: %d\n", sd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                } else {
                    // Handle client message
                    buffer[valread] = '\0';
                    printf("Received message from client on Socket FD: %d:\n%s\n", sd, buffer);

                    // Get top CPU-consuming processes and send response
                    char response[1024];
                    get_top_cpu_processes(response);
                    send(sd, response, strlen(response), 0);
                    printf("Sent CPU process info to client on Socket FD: %d\n", sd);
                }
            }
        }
    }

    return 0;
}


