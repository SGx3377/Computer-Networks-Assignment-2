// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <pthread.h>

// #define PORT 8080
// #define BUFFER_SIZE 1024

// void *connect_to_server(void *arg);

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <number_of_connections>\n", argv[0]);
//         exit(EXIT_FAILURE);
//     }

//     int n = atoi(argv[1]);
//     pthread_t threads[n];

//     // Create n client connections
//     for (int i = 0; i < n; i++) {
//         if (pthread_create(&threads[i], NULL, connect_to_server, NULL) != 0) {
//             perror("pthread_create failed");
//             exit(EXIT_FAILURE);
//         }
//     }

//     // Wait for all threads to finish
//     for (int i = 0; i < n; i++) {
//         pthread_join(threads[i], NULL);
//     }

//     return 0;
// }

// void *connect_to_server(void *arg) {
//     int sock;
//     struct sockaddr_in serv_addr;
//     char buffer[BUFFER_SIZE] = {0};

//     // Create socket
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//         perror("Socket creation error");
//         return NULL;
//     }

//     // Define server address
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(PORT);

//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
//         perror("Invalid address / Address not supported");
//         return NULL;
//     }

//     // Connect to server
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
//         perror("Connection failed");
//         return NULL;
//     }

//     // Send a simple request to server (e.g., "GET CPU INFO")
//     send(sock, "GET CPU INFO", strlen("GET CPU INFO"), 0);

//     // Read server response
//     read(sock, buffer, BUFFER_SIZE);
//     printf("Server Response: \n%s\n", buffer);

//     close(sock);
//     return NULL;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *connect_to_server(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_connections>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    pthread_t threads[n];

    // Create n client connections
    for (int i = 0; i < n; i++) {
        printf("Creating client %d\n", i + 1);
        if (pthread_create(&threads[i], NULL, connect_to_server, (void*)(long)(i + 1)) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void *connect_to_server(void *arg) {
    int client_id = (int)(long)arg;
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    printf("Client %d attempting to connect to the server...\n", client_id);

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return NULL;
    }

    // Define server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return NULL;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return NULL;
    }

    printf("Client %d connected to server\n", client_id);

    // Send a simple request to server (e.g., "GET CPU INFO")
    send(sock, "GET CPU INFO", strlen("GET CPU INFO"), 0);

    // Read server response
    read(sock, buffer, BUFFER_SIZE);
    printf("Client %d received server response:\n%s\n", client_id, buffer);

    close(sock);
    printf("Client %d disconnected\n", client_id);
    return NULL;
}

