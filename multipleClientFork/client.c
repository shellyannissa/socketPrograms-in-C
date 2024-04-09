#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 4444

int main(int argc, char **argv)
{
    int clientSocket;
    struct sockaddr_in serverAddr;

    char buffer[1024];

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Error connecting to server");
        exit(1);
    }

    while (1)
    {
        printf("Client: ");
        fgets(buffer, 1024, stdin);
        send(clientSocket, buffer, strlen(buffer), 0);

        if (strncmp(buffer, "exit", 4) == 0)
        {
            close(clientSocket);
            break;
        }

        if (recv(clientSocket, buffer, 1024, 0) < 0)
        {
            perror("Error receiving data from server");
            exit(1);
        }

        printf("Server: %s", buffer);
    }
    return 0;
}
