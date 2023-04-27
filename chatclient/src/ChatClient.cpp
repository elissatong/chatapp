#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // For close(<socket file descriptor)
#include <thread>

#include "HelloWorld.hpp"

using namespace std;

void StartListeningToServerMessages(int clientSocketFD)
{
    char response[1024];
    while (true)
    {
        // Receive a response from the server
        int bytes_received = recv(clientSocketFD, response, sizeof(response), 0);
        if (bytes_received < 0)
        {
            cerr << "Failed to receive server response." << endl;
            break;
        }
        else
        {
            // Print the response from the server
            response[bytes_received] = '\0';
            cout << "Server Response: " << response;
        }
    }
}

int main()
{

    cout << "Hello Client!\n";

    // Create a socket
    int clientSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketFD == -1)
    {
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
    server_address.sin_port = htons(2000);                   // use ports > 1000, not the reserved ports

    // Connect to the server
    if (connect(clientSocketFD, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Client Connection failed." << endl;
        perror("Failure error");
        return 1;
    }
    else
    {
        cout << "Socket connected successfully\n";
    }

    std::thread threadServerListener(StartListeningToServerMessages, clientSocketFD);
    threadServerListener.detach();

    char *message = nullptr;
    size_t messageSize = 0;

    while (true)
    {
        // Get the client user's chat messages, until user exits
        cout << "Type your message (or type \"exit\")...\n";
        ssize_t charCount = getline(&message, &messageSize, stdin);

        if (charCount > 0)
        {
            ssize_t amountWasSent = send(clientSocketFD, message, charCount, 0);
            if (amountWasSent != charCount)
            {
                cerr << "Failed to send message." << endl;
                perror("Failure error");
                exit(EXIT_FAILURE);
            }

            if (strcmp(message, "exit\n") == 0)
            {
                // Client user exits chat room
                break;
            }
        }
    }

    // Close the socket
    close(clientSocketFD);

    return 0;
}
