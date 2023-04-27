#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <thread>

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

struct AcceptedSocket *AcceptIncomingConnection(int serverSocketFD)
{
    struct sockaddr_in clientAddress;
    auto clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    struct AcceptedSocket *acceptedSocket = (struct AcceptedSocket *)malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = (clientSocketFD > 0);

    if (!acceptedSocket->acceptedSocketFD)
    {
        acceptedSocket->error = clientSocketFD;
    }
    return acceptedSocket;
}

void ReceiveData(AcceptedSocket *clientSocket)
{
    int bytesReceived = 0;
    const char *messageReceived = "message received";
    NetworkUtilities NetworkUtil;

    while (true)
    {
        std::string receivedMessage = "";
        bytesReceived = NetworkUtil.receiveData(clientSocket->acceptedSocketFD, receivedMessage);
        if (bytesReceived > 0)
        {
            std::cout << "Received client message: " << receivedMessage;
        }
        else
        {
            break;
        }

        NetworkUtil.sendData(clientSocket->acceptedSocketFD, messageReceived);
        std::cout << "Send response: " << messageReceived << std::endl;
    }
    close(clientSocket->acceptedSocketFD);
}

void AcceptIncomingConnections(int serverSocketFD)
{
    // Accept incoming connections and handle them
    while (true)
    {
        AcceptedSocket *clientSocket = AcceptIncomingConnection(serverSocketFD);
        if (clientSocket != nullptr)
        {
            if (clientSocket->error == -1)
            {
                exit(EXIT_FAILURE);
            }
            else
            {
                std::cout << "Accept client socket connection, (fd " << clientSocket->acceptedSocketFD << ")\n";
            }

            std::thread threadReceiveData(ReceiveData, clientSocket);
            threadReceiveData.detach(); //  detach the child threads from the main thread, allowing them to continue running independently.

            free (clientSocket);
            clientSocket = nullptr;   
        }
    }


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

    AcceptIncomingConnections(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    return 0;
}
