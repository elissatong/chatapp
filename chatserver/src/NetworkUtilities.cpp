#include "NetworkUtilities.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

extern int errno;

NetworkUtilities::NetworkUtilities()
{
}

int NetworkUtilities::createTCPSocket()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation error");
        return -1;
    }

    return sockfd;
}

int NetworkUtilities::bindSocket(int sockfd, int port)
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int result = ::bind(sockfd, (sockaddr *)&address, sizeof(address)); // use the bind() from the global namespace
    if (result == -1)
    {
        std::cerr << "bind failed" << std::endl;
        perror("Bind error message: ");
        closeSocket(sockfd);
        return -1;
    }
    return 0;
}

int NetworkUtilities::listenSocket(int sockfd, int numConnectionsQueued)
{
    if (listen(sockfd, numConnectionsQueued) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        closeSocket(sockfd);
        return -1;
    }

    return 0;
}

int NetworkUtilities::acceptConnection(int sockfd)
{
    int newSockfd = accept(sockfd, NULL, NULL);
    if (newSockfd < 0)
    {
        std::cerr << "Accept failed" << std::endl;
        closeSocket(sockfd);
        return -1;
    }

    return newSockfd;
}

int NetworkUtilities::connectSocket(int sockfd, const std::string &server, int port)
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (inet_pton(AF_INET, server.c_str(), &address.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (connect(sockfd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    return 0;
}

int NetworkUtilities::sendData(int sockfd, const char *data, int length)
{
    int bytesSent = send(sockfd, data, length, 0);
    if (bytesSent < 0)
    {
        std::cerr << "Send failed" << std::endl;
        return -1;
    }
    return bytesSent;
}

int NetworkUtilities::sendData(int sockfd, std::string data)
{
    int n = write(sockfd, data.c_str(), data.length());
    if (n < 0)
    {
        std::cerr << "Error writing to socket" << std::endl;
        return -1;
    }

    return 0;
}

int NetworkUtilities::receiveData(int sockfd, char *buffer, int bufferLength)
{
    int bytesReceived = recv(sockfd, buffer, bufferLength, 0);
    if (bytesReceived < 0)
    {
        std::cerr << "Receive failed" << std::endl;
        return -1;
    }
    return bytesReceived;
}

int NetworkUtilities::receiveData(int sockfd, std::string &data)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n < 0)
    {
        std::cerr << "Error reading from socket" << std::endl;
        return -1;
    }

    data = buffer;
    return 0;
}

void NetworkUtilities::closeSocket(int sockfd)
{
    close(sockfd);
}
