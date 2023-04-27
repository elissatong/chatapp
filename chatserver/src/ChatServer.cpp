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

struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket * AcceptIncomingConnection(int serverSocketFD)
{
    struct sockaddr_in clientAddress;
    auto clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    struct AcceptedSocket* acceptedSocket = (struct AcceptedSocket *) malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = (clientSocketFD > 0);

    if (!acceptedSocket->acceptedSocketFD)
    {
        acceptedSocket->error = clientSocketFD;
    }
    return acceptedSocket;
}

void ReceiveData(int clientSocketFD)
{
    int bytesReceived = 0;
    const char * messageReceived = "message received";
    NetworkUtilities NetworkUtil;

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

}

void StartAcceptingIncomingConnections(int serverSocketFD)
{
    // Accept incoming connections and handle them
    AcceptedSocket * clientSocket = AcceptIncomingConnection(serverSocketFD);
    //int clientSocketFD = NetworkUtil.acceptConnection(serverSocketFD);
    if (clientSocket->error == -1)
    {
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Accept client socket connection, (fd " << clientSocket->acceptedSocketFD << ")\n";
    }

    ReceiveData(clientSocket->acceptedSocketFD);
    close(clientSocket->acceptedSocketFD);
    free(clientSocket);
    clientSocket = nullptr;
}

int main()
{
    NetworkUtilities NetworkUtil;

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
    StartAcceptingIncomingConnections(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);    
    return 0;
}
