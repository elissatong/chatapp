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

    int bytesReceived = 0;
    const char * messageReceived = "message received";

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
    int clientSocketFD = NetworkUtil.acceptConnection(serverSocketFD);
    if (clientSocketFD == -1)
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Accept client socket connection, (fd " << clientSocketFD << ")\n";
    }

    while (true)
    {
        std::string receivedMessage = "";
        bytesReceived = NetworkUtil.receiveData(clientSocketFD, receivedMessage);
        if (bytesReceived > 0)
        {
            std::cout << "Received client message: " << receivedMessage;
        }
        else
        {
            break;
        }
        
        NetworkUtil.sendData(clientSocketFD, messageReceived);
        std::cout << "Send response: " << messageReceived << std::endl;
    }

    close(clientSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);

    return 0;
}
