#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10  // Define maximum number of clients

// Structure to hold client information
struct client_info {
    int client_id;
};

// Function to run the client
void *run_client(void *arg) {
    struct client_info *info = (struct client_info *)arg;
    int client_id = info->client_id;

    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Request CPU info";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Client %d: Socket creation error\n", client_id);
        pthread_exit(NULL);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Client %d: Invalid address\n", client_id);
        pthread_exit(NULL);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Client %d: Connection failed\n", client_id);
        pthread_exit(NULL);
    }

    printf("Client %d: Connected to server.\n", client_id);

    send(sock, message, strlen(message), 0);
    printf("Client %d: Message sent to server.\n", client_id);

    int valread = read(sock, buffer, 1024);
    printf("Client %d: Server response:\n%s\n", client_id, buffer);

    close(sock);
    printf("Client %d: Disconnected.\n", client_id);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_clients>\n", argv[0]);
        return -1;
    }

    int num_clients = atoi(argv[1]);

    // Check if the number of clients exceeds the maximum limit
    if (num_clients > MAX_CLIENTS) {
        printf("Error: Number of clients exceeds the maximum limit of %d\n", MAX_CLIENTS);
        printf("Only processing the first %d clients.\n", MAX_CLIENTS);
        num_clients = MAX_CLIENTS; 
    }

    pthread_t threads[MAX_CLIENTS];
    struct client_info clients[MAX_CLIENTS];

    for (int i = 0; i < num_clients; i++) {
        clients[i].client_id = i + 1; // Assign client ID

        // Create a new thread for each client
        if (pthread_create(&threads[i], NULL, run_client, &clients[i]) != 0) {
            printf("Error creating thread for client %d\n", i + 1);
            return -1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_clients; i++) {
        pthread_join(threads[i], NULL);
    }

    // If there were more than 10 clients requested
    if (num_clients > 10) {
        printf("Connection failed for additional clients beyond %d.\n", MAX_CLIENTS);
    }

    return 0;
}

