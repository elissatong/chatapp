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
const char *MESSAGE_RECEIVED = "message received\n";

extern int errno;

struct AcceptedSocket
{
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

// Store up to 10 sockets accepted
const int MAX_ACCEPTED_SOCKETS = 10;
struct AcceptedSocket acceptedSockets[MAX_ACCEPTED_SOCKETS];
int acceptedSocketsCount = 0;

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

void SendReceivedMessageToAllClients(int currentClientSocketFD, std::string &receivedMessage)
{
    NetworkUtilities NetworkUtil;
    for (int i = 0; i < acceptedSocketsCount; ++i)
    {
        if (acceptedSockets[i].acceptedSocketFD != currentClientSocketFD) // not the current client who sent the message
        {
            NetworkUtil.sendData(acceptedSockets[i].acceptedSocketFD, receivedMessage);
        }
    }
}

void RemoveClientOnExit(int currentClientSocketFD)
{
    if (acceptedSocketsCount == 0)
    {
        return;
    }

    // If this client socket exits, remove it from the acceptedSockets[] array
    // 1. Found the index in which the socket is stored
    int i = 0;
    for (i = 0; i < acceptedSocketsCount; ++i)
    {
        if (acceptedSockets[i].acceptedSocketFD == currentClientSocketFD) // not the current client who sent the message
        {
            // found index
            break;
        }

    }

    // 2. One less socket, decrement first, so count is the last index
    acceptedSocketsCount--;

    // 3. Swap with last index (which is the count - 1)
    if (acceptedSocketsCount >= 2)
    {
        acceptedSockets[i] = acceptedSockets[acceptedSocketsCount];
    }
    
}

void ReceiveData(AcceptedSocket *clientSocket)
{
    int bytesReceived = 0;
    NetworkUtilities NetworkUtil;
    std::string receivedMessage = "";
    while (true)
    {
        receivedMessage = "";   
        bytesReceived = NetworkUtil.receiveData(clientSocket->acceptedSocketFD, receivedMessage);
        if (bytesReceived > 0)
        {
            if (receivedMessage == "exit\n")
            {
                std::cout << "Client FD " << clientSocket->acceptedSocketFD << " exited.\n";
                // TODO: RemoveClientOnExit(clientSocket->acceptedSocketFD);
            }
            else
            {
                std::cout << "Received client message: " << receivedMessage;
                // TODO: 
                SendReceivedMessageToAllClients(clientSocket->acceptedSocketFD, receivedMessage);
            }
            
        }
        else
        {
            break;
        }

        NetworkUtil.sendData(clientSocket->acceptedSocketFD, MESSAGE_RECEIVED);
        std::cout << "Response: " << MESSAGE_RECEIVED;
                
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

            if (acceptedSocketsCount < MAX_ACCEPTED_SOCKETS)
            {
                acceptedSockets[acceptedSocketsCount++] = *clientSocket;
            }

            std::thread threadReceiveData(ReceiveData, clientSocket);
            threadReceiveData.detach(); //  detach the child threads from the main thread, allowing them to continue running independently.

            // free (clientSocket);
            // clientSocket = nullptr;
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
