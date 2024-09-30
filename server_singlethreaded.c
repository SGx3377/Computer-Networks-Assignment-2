// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 8080
// #define BUFFER_SIZE 1024

// int main() {
//     int server_fd, new_socket;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
//     char buffer[BUFFER_SIZE] = {0};
//     const char *response = "Message received by server\n";

//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Set up the server address structure
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     // Bind the socket to the specified IP and port
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections (only 1 at a time)
//     if (listen(server_fd, 1) < 0) {
//         perror("Listen failed");
//         exit(EXIT_FAILURE);
//     }
//     printf("Server is listening on port %d...\n", PORT);

//     // Accept and handle one client connection at a time
//     while (1) {
//         printf("Waiting for a connection...\n");
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
//             perror("Accept failed");
//             exit(EXIT_FAILURE);
//         }
//         printf("Connection established with client\n");

//         // Read the client's message
//         int valread = read(new_socket, buffer, BUFFER_SIZE);
//         printf("Received from client: %s\n", buffer);

//         // Send response back to the client
//         send(new_socket, response, strlen(response), 0);
//         printf("Response sent to client\n");

//         // Close the connection with the current client
//         close(new_socket);
//         printf("Connection with client closed\n");
//     }

//     return 0;
// }

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 8080
// #define BUFFER_SIZE 1024

// struct process_info {
//     char name[256];
//     int pid;
//     long unsigned int user_time;
//     long unsigned int kernel_time;
// };

// void get_top_two_cpu_consuming_processes(struct process_info *proc_info);

// int main() {
//     int server_fd, new_socket;
//     struct sockaddr_in address;
//     int addrlen = sizeof(address);
//     char buffer[BUFFER_SIZE] = {0};

//     // Creating socket file descriptor
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket failed");
//         exit(EXIT_FAILURE);
//     }

//     // Set up the server address structure
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(PORT);

//     // Bind the socket to the specified IP and port
//     if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     // Listen for incoming connections (1 at a time)
//     if (listen(server_fd, 1) < 0) {
//         perror("Listen failed");
//         exit(EXIT_FAILURE);
//     }
//     printf("Server is listening on port %d...\n", PORT);

//     // Sequentially accept and handle one client connection at a time
//     while (1) {
//         printf("Waiting for a connection...\n");
//         if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
//             perror("Accept failed");
//             exit(EXIT_FAILURE);
//         }
//         printf("Connection established with client\n");

//         // Read the client's message
//         int valread = read(new_socket, buffer, BUFFER_SIZE);
//         if (valread > 0) {
//             buffer[valread] = '\0';  // Null-terminate the received message
//             printf("Received from client: %s\n", buffer);
//         }

//         // Get top two CPU-consuming processes
//         struct process_info proc_info[2];
//         get_top_two_cpu_consuming_processes(proc_info);

//         // Prepare response for the client
//         snprintf(buffer, sizeof(buffer),
//                  "Top CPU-consuming processes:\n"
//                  "1. %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
//                  "2. %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
//                  proc_info[0].name, proc_info[0].pid, proc_info[0].user_time, proc_info[0].kernel_time,
//                  proc_info[1].name, proc_info[1].pid, proc_info[1].user_time, proc_info[1].kernel_time);

//         // Send response back to the client
//         send(new_socket, buffer, strlen(buffer), 0);
//         printf("Response sent to client\n");

//         // Close the connection with the current client
//         close(new_socket);
//         printf("Connection with client closed\n");
//     }

//     return 0;
// }

// void get_top_two_cpu_consuming_processes(struct process_info *proc_info) {
//     FILE *fp;
//     char path[1024];
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct process_info {
    char name[256];
    int pid;
    long unsigned int user_time;
    long unsigned int kernel_time;
};

void get_top_two_cpu_consuming_processes(struct process_info *proc_info);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections (1 at a time)
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    // Sequentially accept and handle one client connection at a time
    int client_count = 0;  // Track number of clients
    while (1) {
        printf("Waiting for a client to connect...\n");

        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Increment and print client number
        client_count++;
        printf("Client %d connected\n", client_count);

        // Get client's IP address and log the connection
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Client connected from IP: %s, Port: %d\n", client_ip, ntohs(address.sin_port));

        // Read the client's message
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';  // Null-terminate the received message
            printf("Message from Client %d [%s:%d]: %s\n", client_count, client_ip, ntohs(address.sin_port), buffer);
        }

        // Get top two CPU-consuming processes
        struct process_info proc_info[2];
        printf("Processing Client %d's request to retrieve top two CPU-consuming processes...\n", client_count);
        get_top_two_cpu_consuming_processes(proc_info);

        // Prepare response for the client
        snprintf(buffer, sizeof(buffer),
                 "Top CPU-consuming processes:\n"
                 "1. %s (PID: %d), User Time: %lu, Kernel Time: %lu\n"
                 "2. %s (PID: %d), User Time: %lu, Kernel Time: %lu\n",
                 proc_info[0].name, proc_info[0].pid, proc_info[0].user_time, proc_info[0].kernel_time,
                 proc_info[1].name, proc_info[1].pid, proc_info[1].user_time, proc_info[1].kernel_time);

        // Send response back to the client
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Response sent to Client %d [%s:%d]\n", client_count, client_ip, ntohs(address.sin_port));

        // Close the connection with the current client
        close(new_socket);
        printf("Client %d disconnected\n", client_count);
    }

    return 0;
}

void get_top_two_cpu_consuming_processes(struct process_info *proc_info) {
    FILE *fp;
    char path[1024];
    int pid;
    long unsigned int user_time, kernel_time;
    struct process_info temp_proc;
    struct process_info top_two[2] = { { "", -1, 0, 0 }, { "", -1, 0, 0 } };

    for (pid = 1; pid < 32768; pid++) {  // Iterate over possible PIDs
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
