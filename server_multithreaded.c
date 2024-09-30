// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <fcntl.h>
// #include <errno.h>

// #define PORT 8080
// #define MAX_CLIENTS 100
// #define BUFFER_SIZE 1024

// struct process_info {
//     char name[256];
//     int pid;
//     long unsigned int user_time;
//     long unsigned int kernel_time;
// };

// void *handle_client(void *arg);
// void get_top_two_cpu_consuming_processes(struct process_info *proc_info);

// int main() {
//     int server_fd, new_socket, *new_sock;
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);

//     // Create socket
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Set socket options
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
//         perror("setsockopt failed");
//         exit(EXIT_FAILURE);
//     }

//     // Define server address
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     // Bind the socket
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     // Start listening
//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         perror("Listen failed");
//         exit(EXIT_FAILURE);
//     }
//     printf("Server listening on port %d\n", PORT);

//     // Accept incoming connections
//     while (1) {
//         new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
//         if (new_socket < 0) {
//             perror("Accept failed");
//             exit(EXIT_FAILURE);
//         }
//         printf("Accepted connection from client\n");

//         pthread_t client_thread;
//         new_sock = malloc(1);
//         *new_sock = new_socket;
//         if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
//             perror("Could not create thread");
//             exit(EXIT_FAILURE);
//         }
//         pthread_detach(client_thread);
//     }

//     return 0;
// }

// void *handle_client(void *arg) {
//     int sock = *(int *)arg;
//     free(arg);
//     char buffer[BUFFER_SIZE];
//     struct process_info proc_info[2];

//     // Receive message from client (not really necessary as client just requests info)
//     read(sock, buffer, BUFFER_SIZE);

//     // Get top two CPU-consuming processes
//     get_top_two_cpu_consuming_processes(proc_info);

//     // Send the process info to the client
//     snprintf(buffer, sizeof(buffer), "Process 1: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
//                                      "Process 2: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
//              proc_info[0].name, proc_info[0].pid, proc_info[0].user_time, proc_info[0].kernel_time,
//              proc_info[1].name, proc_info[1].pid, proc_info[1].user_time, proc_info[1].kernel_time);
//     send(sock, buffer, strlen(buffer), 0);

//     close(sock);
//     return 0;
// }

// void get_top_two_cpu_consuming_processes(struct process_info *proc_info) {
//     FILE *fp;
//     char path[1024], line[1024];
//     int pid;
//     long unsigned int user_time, kernel_time;
//     struct process_info temp_proc;
//     struct process_info top_two[2] = { { "", -1, 0, 0 }, { "", -1, 0, 0 } };

//     for (pid = 1; pid < 32768; pid++) {  // Iterate over possible PIDs
//         snprintf(path, sizeof(path), "/proc/%d/stat", pid);
//         fp = fopen(path, "r");
//         if (fp == NULL) continue;

//         // Read the relevant fields from /proc/[pid]/stat
//         fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &pid, temp_proc.name, &user_time, &kernel_time);
//         fclose(fp);

//         temp_proc.pid = pid;
//         temp_proc.user_time = user_time;
//         temp_proc.kernel_time = kernel_time;

//         // Check if the current process is one of the top two CPU consumers
//         long unsigned int total_time = user_time + kernel_time;
//         if (total_time > top_two[0].user_time + top_two[0].kernel_time) {
//             top_two[1] = top_two[0];
//             top_two[0] = temp_proc;
//         } else if (total_time > top_two[1].user_time + top_two[1].kernel_time) {
//             top_two[1] = temp_proc;
//         }
//     }

//     proc_info[0] = top_two[0];
//     proc_info[1] = top_two[1];
// }


// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <fcntl.h>
// #include <errno.h>
// #include <signal.h>

// #define PORT 8080
// #define MAX_CLIENTS 100
// #define BUFFER_SIZE 1024

// struct process_info {
//     char name[256];
//     int pid;
//     long unsigned int user_time;
//     long unsigned int kernel_time;
// };

// void *handle_client(void *arg);
// void get_top_two_cpu_consuming_processes(struct process_info *proc_info);
// void handle_sigint(int sig);

// int server_fd;  // Global server socket descriptor
// int client_counter = 0;
// pthread_mutex_t client_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
// int running = 1;  // Global flag to indicate server status

// int main() {
//     struct sockaddr_in address;
//     int opt = 1;
//     int addrlen = sizeof(address);

//     // Install signal handler for SIGINT (Ctrl + C)
//     signal(SIGINT, handle_sigint);

//     // Create socket
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Set socket options
//     if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
//         perror("setsockopt failed");
//         exit(EXIT_FAILURE);
//     }

//     // Define server address
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     // Bind the socket
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     // Start listening
//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         perror("Listen failed");
//         exit(EXIT_FAILURE);
//     }
//     printf("Server is listening on port %d\n", PORT);

//     // Accept incoming connections
//     while (running) {
//         int new_socket;
//         int *new_sock;
        
//         printf("Waiting for connections...\n");
//         new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
//         // Break loop if the server is stopped due to SIGINT
//         if (!running) break;

//         if (new_socket < 0) {
//             perror("Accept failed");
//             continue;
//         }

//         pthread_mutex_lock(&client_counter_mutex);
//         client_counter++;
//         int client_id = client_counter;
//         pthread_mutex_unlock(&client_counter_mutex);

//         printf("Client %d connected\n", client_id);

//         pthread_t client_thread;
//         new_sock = malloc(sizeof(int));
//         *new_sock = new_socket;

//         if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
//             perror("Could not create thread");
//             free(new_sock);
//             continue;
//         }
//         pthread_detach(client_thread);
//     }

//     // Clean up resources and shut down the server
//     printf("\nShutting down the server...\n");
//     close(server_fd);
//     pthread_mutex_destroy(&client_counter_mutex);

//     return 0;
// }

// void *handle_client(void *arg) {
//     int sock = *(int *)arg;
//     free(arg);
//     char buffer[BUFFER_SIZE];
//     struct process_info proc_info[2];

//     // Get the client ID by locking the mutex
//     pthread_mutex_lock(&client_counter_mutex);
//     int current_client = client_counter;
//     pthread_mutex_unlock(&client_counter_mutex);

//     printf("Processing client %d\n", current_client);

//     // Receive message from client (even if not necessary)
//     read(sock, buffer, BUFFER_SIZE);

//     // Get top two CPU-consuming processes
//     get_top_two_cpu_consuming_processes(proc_info);

//     // Send the process info to the client
//     snprintf(buffer, sizeof(buffer), "Process 1: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
//                                      "Process 2: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
//              proc_info[0].name, proc_info[0].pid, proc_info[0].user_time, proc_info[0].kernel_time,
//              proc_info[1].name, proc_info[1].pid, proc_info[1].user_time, proc_info[1].kernel_time);
//     send(sock, buffer, strlen(buffer), 0);

//     printf("Sent data to client %d\n", current_client);

//     // Close the socket and log client disconnection
//     close(sock);
//     printf("Client %d disconnected\n", current_client);

//     return 0;
// }

// void get_top_two_cpu_consuming_processes(struct process_info *proc_info) {
//     FILE *fp;
//     char path[1024], line[1024];
//     int pid;
//     long unsigned int user_time, kernel_time;
//     struct process_info temp_proc;
//     struct process_info top_two[2] = { { "", -1, 0, 0 }, { "", -1, 0, 0 } };

//     for (pid = 1; pid < 32768; pid++) {  // Iterate over possible PIDs
//         snprintf(path, sizeof(path), "/proc/%d/stat", pid);
//         fp = fopen(path, "r");
//         if (fp == NULL) continue;

//         // Read the relevant fields from /proc/[pid]/stat
//         fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &pid, temp_proc.name, &user_time, &kernel_time);
//         fclose(fp);

//         temp_proc.pid = pid;
//         temp_proc.user_time = user_time;
//         temp_proc.kernel_time = kernel_time;

//         // Check if the current process is one of the top two CPU consumers
//         long unsigned int total_time = user_time + kernel_time;
//         if (total_time > top_two[0].user_time + top_two[0].kernel_time) {
//             top_two[1] = top_two[0];
//             top_two[0] = temp_proc;
//         } else if (total_time > top_two[1].user_time + top_two[1].kernel_time) {
//             top_two[1] = temp_proc;
//         }
//     }

//     proc_info[0] = top_two[0];
//     proc_info[1] = top_two[1];
// }

// // Signal handler for SIGINT (Ctrl + C)
// void handle_sigint(int sig) {
//     printf("\nSIGINT caught, shutting down the server...\n");
//     running = 0;
//     close(server_fd);  // Close the server socket to stop accepting new connections
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

// Structure to hold process info
struct process_info {
    char name[256];
    int pid;
    long unsigned int user_time;
    long unsigned int kernel_time;
};

// Structure to hold client data
struct client_data {
    int client_id;
    int sock;
    struct sockaddr_in address;
};

void *handle_client(void *arg);
void get_top_two_cpu_consuming_processes(struct process_info *proc_info);
void handle_sigint(int sig);

int server_fd;  // Global server socket descriptor
int client_counter = 0;
pthread_mutex_t client_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
int running = 1;  // Global flag to indicate server status

// Main function
int main() {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Install signal handler for SIGINT (Ctrl + C)
    signal(SIGINT, handle_sigint);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);

    // Accept incoming connections
    while (running) {
        int new_socket;
        struct client_data *data;

        printf("Waiting for connections...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        
        if (!running) break;  // Break loop if server is stopped due to SIGINT

        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_mutex_lock(&client_counter_mutex);
        client_counter++;
        int client_id = client_counter;
        pthread_mutex_unlock(&client_counter_mutex);

        // Allocate memory for client data and save client info
        data = malloc(sizeof(struct client_data));
        data->sock = new_socket;
        data->client_id = client_id;
        memcpy(&data->address, &address, sizeof(address));

        // Print client IP and port information
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(address.sin_port);

        printf("Client %d connected from IP: %s, Port: %d\n", client_id, client_ip, client_port);

        // Create a thread to handle the client
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, handle_client, (void*)data) < 0) {
            perror("Could not create thread");
            free(data);
            continue;
        }
        pthread_detach(client_thread);  // Detach the thread to handle the client independently
    }

    // Clean up resources and shut down the server
    printf("\nShutting down the server...\n");
    close(server_fd);
    pthread_mutex_destroy(&client_counter_mutex);

    return 0;
}

// Function to handle each client
void *handle_client(void *arg) {
    struct client_data *data = (struct client_data *)arg;
    int sock = data->sock;
    int current_client = data->client_id;
    struct sockaddr_in client_address = data->address;
    free(data);

    char buffer[BUFFER_SIZE];
    struct process_info proc_info[2];

    // Print client IP and port again for this thread
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);
    int client_port = ntohs(client_address.sin_port);

    printf("Processing client %d (IP: %s, Port: %d)\n", current_client, client_ip, client_port);

    // Receive message from client (even if not necessary)
    read(sock, buffer, BUFFER_SIZE);

    // Get top two CPU-consuming processes
    get_top_two_cpu_consuming_processes(proc_info);

    // Send the process info to the client
    snprintf(buffer, sizeof(buffer), "Process 1: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
                                     "Process 2: %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
             proc_info[0].name, proc_info[0].pid, proc_info[0].user_time, proc_info[0].kernel_time,
             proc_info[1].name, proc_info[1].pid, proc_info[1].user_time, proc_info[1].kernel_time);
    send(sock, buffer, strlen(buffer), 0);

    printf("Sent data to client %d (IP: %s, Port: %d)\n", current_client, client_ip, client_port);

    // Close the socket and log client disconnection
    close(sock);
    printf("Client %d (IP: %s, Port: %d) disconnected\n", current_client, client_ip, client_port);

    return 0;
}

// Function to get the top two CPU-consuming processes
void get_top_two_cpu_consuming_processes(struct process_info *proc_info) {
    FILE *fp;
    char path[1024], line[1024];
    int pid;
    long unsigned int user_time, kernel_time;
    struct process_info temp_proc;
    struct process_info top_two[2] = { { "", -1, 0, 0 }, { "", -1, 0, 0 } };

    // Iterate over possible PIDs
    for (pid = 1; pid < 32768; pid++) {
        snprintf(path, sizeof(path), "/proc/%d/stat", pid);
        fp = fopen(path, "r");
        if (fp == NULL) continue;

        // Read the relevant fields from /proc/[pid]/stat
        fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &pid, temp_proc.name, &user_time, &kernel_time);
        fclose(fp);

        temp_proc.pid = pid;
        temp_proc.user_time = user_time;
        temp_proc.kernel_time = kernel_time;

        // Check if the current process is one of the top two CPU consumers
        long unsigned int total_time = user_time + kernel_time;
        if (total_time > top_two[0].user_time + top_two[0].kernel_time) {
            top_two[1] = top_two[0];
            top_two[0] = temp_proc;
        } else if (total_time > top_two[1].user_time + top_two[1].kernel_time) {
            top_two[1] = temp_proc;
        }
    }

    proc_info[0] = top_two[0];
    proc_info[1] = top_two[1];
}

// Signal handler for SIGINT (Ctrl + C)
void handle_sigint(int sig) {
    printf("\nSIGINT caught, shutting down the server...\n");
    running = 0;
    close(server_fd);  // Close the server socket to stop accepting new connections
}
