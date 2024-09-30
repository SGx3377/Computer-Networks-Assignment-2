#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_clients>\n", argv[0]);
        return -1;
    }

    int num_clients = atoi(argv[1]);  // Number of client connections to create

    for (int i = 1; i <= num_clients; i++) {
        int sock = 0;
        struct sockaddr_in serv_addr;
        char buffer[BUFFER_SIZE] = {0};
        const char *message = "Requesting top CPU-consuming processes";

        printf("Client %d attempting to connect to server...\n", i);

        // Creating socket file descriptor
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\nSocket creation error\n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/Address not supported\n");
            return -1;
        }

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed\n");
            return -1;
        }

        printf("Client %d connected to server\n", i);

        // Send message to the server
        send(sock, message, strlen(message), 0);
        printf("Client %d: Request sent to server\n", i);

        // Read server's response
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            buffer[valread] = '\0';  // Null-terminate the received message
            printf("Client %d: Response from server:\n%s\n", i, buffer);
        }

        // Close the socket
        close(sock);
        printf("Client %d disconnected\n", i);
    }

    return 0;
}
