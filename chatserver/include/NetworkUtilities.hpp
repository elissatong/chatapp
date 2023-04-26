#ifndef NETWORK_UTILITIES_HPP
#define NETWORK_UTILITIES_HPP

#include <string>

class NetworkUtilities
{
public:

    static const int NUM_CONNECTIONS_QUEUED = 5;
    NetworkUtilities();

    // Create a socket file descriptor
    int createTCPSocket();

    // Bind the socket to a port
    int bindSocket(int sockfd, int port);

    // Listen for incoming connections
    // The numConnectionsQueued argument provides a hint to the implementation which the implementation shall use to limit the number of outstanding connections in the socket's listen queue. 
    // Implementations may impose a limit on backlog and silently reduce the specified value. 
    int listenSocket(int sockfd, int numConnectionsQueued = NUM_CONNECTIONS_QUEUED);

    // Accept an incoming connection
    int acceptConnection(int sockfd);

    // Connect to a remote server
    int connectSocket(int sockfd, const std::string& server, int port);

    // Send data over a socket
    int sendData(int sockfd, const char* data, int length);
    int sendData(int sockfd, std::string data);

    // Receive data from a socket
    int receiveData(int sockfd, char* buffer, int bufferLength);
    int receiveData(int sockfd, std::string &data);

    // Close a socket descriptor
    void closeSocket(int sockfd);
};

#endif // NETWORK_UTILITIES_HPP 
