#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "NetworkUtilities.hpp"

const int PORT = 2000;
const int BUFFER_SIZE = 1024;

extern int errno;

int main()
{
    NetworkUtilities NetworkUtil;

    int clientSocketFD = 0;
    int bytesReceived = 0;
    int opt = 1;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from server";

    // Creating socket file descriptor
    int serverSocketFD = NetworkUtil.createTCPSocket();
    if (serverSocketFD == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the specified address and port
    if (NetworkUtil.bindSocket(serverSocketFD, PORT) == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (NetworkUtil.listenSocket(serverSocketFD) == -1)
    {
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // Accept incoming connections and handle them
    while (true)
    {
        clientSocketFD = NetworkUtil.acceptConnection(serverSocketFD);
        if (clientSocketFD == -1)
        {
            exit(EXIT_FAILURE);
        }
        else
        {
            std::cout << "Accept client socket connection, (fd " << clientSocketFD << ")\n";
        }

        bytesReceived = NetworkUtil.receiveData(clientSocketFD, buffer, 1024);
        if (bytesReceived > 0)
        {
            std::cout << "Received message from client: " << buffer << std::endl;
        }
        
        send(clientSocketFD, hello, strlen(hello), 0);
        std::cout << "Hello message sent" << std::endl;

        close(clientSocketFD);
    }

    return 0;
}
