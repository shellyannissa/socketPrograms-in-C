

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_MSG_LEN 1024

void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <recipient_ip>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[MAX_MSG_LEN];
    int n;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Error opening socket");

    // Initialize server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address from command line argument to network address structure
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid address or address not supported\n");
        exit(EXIT_FAILURE);
    }

    // Connect to the recipient
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Error connecting");

    printf("Connected to the recipient. Start chatting!\n");

    // Start chatting
    while (1)
    {
        printf("You: ");
        fgets(buffer, MAX_MSG_LEN, stdin);
        if (strcmp(buffer, "bye\n") == 0)
            break;

        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("Error writing to socket");

        bzero(buffer, MAX_MSG_LEN);
        n = read(sockfd, buffer, MAX_MSG_LEN - 1);
        if (n < 0)
            error("Error reading from socket");

        printf("Recipient: %s", buffer);
    }

    // Close socket
    close(sockfd);
    printf("Connection closed. Bye!\n");

    return 0;
}
