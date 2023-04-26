#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // For close(<socket file descriptor)

#include "HelloWorld.hpp"

using namespace std;

int main() {

    cout << "Hello Client!\n";
    
    // Create a socket
    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD == -1) {
        cerr << "Failed to create socket." << endl;
        perror("Failure error");
        return 1;
    }
    else
    {
        cout << "Socket created successfully with client FD: " << clientSocketFD << endl;
    }

    // Specify the address and port of the server to connect to
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //"10.0.0.1"); //"142.251.46.206"); // google.com's IP address
    server_address.sin_port = htons(2000); // use ports > 1000, not the reserved ports

    // Connect to the server
    if (connect(clientSocketFD, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        cerr << "Client Connection failed." << endl;
        perror("Failure error");
        return 1;
    }
    else
    {
        cout << "Socket connected successfully\n";
    }

    // Send a message to the server
    string message = "Hello, server! From client fd " + std::to_string(clientSocketFD);
    if (send(clientSocketFD, message.c_str(), message.size(), 0) != message.size()) {
        cerr << "Failed to send message." << endl;
        perror("Failure error");
        return 1;
    }

    // Receive a response from the server
    char response[1024];
    int bytes_received = recv(clientSocketFD, response, sizeof(response), 0);
    if (bytes_received < 0) {
        cerr << "Failed to receive response." << endl;
        return 1;
    }
    else
    {
        // Print the response from the server
        response[bytes_received] = '\0';
        cout << "Received server response message successfully\n";
        cout << "Server response: " << response << endl;
    }

    
    // Close the socket
    close(clientSocketFD);

    return 0;
}
