# Computer-Networks-Assignment-2
# Client-Server Application with CPU Monitoring

This repository contains a simple client-server application implemented in C that demonstrates socket programming. The server accepts multiple client connections and echoes back messages sent by the clients. Additionally, it monitors and displays the top two CPU-consuming processes on the server when a new client connects.
Also, it includes TCP client-server program using multithreading, singlethreading and using select system call.

## Features
- **Client-Server Architecture**: A TCP server that can handle multiple clients concurrently.
- **Message Echoing**: The server echoes messages received from clients.
- **CPU Monitoring**: Displays the top two CPU-consuming processes when a new client connects.
- **Signal Handling**: Graceful shutdown of the server on receiving a SIGINT (Ctrl + C).

## Requirements
- **C Compiler**: Ensure you have a C compiler installed (e.g., GCC).
- **Libraries**: The program uses standard C libraries, so no additional installations are required.

## Installation

2. Compile the server and client programs:
    ```bash
    gcc server.c -o server
    gcc client.c -o client
    ```

## Usage

1. **Start the Server**:
   Open a terminal window and run:
   ```bash
   ./server
