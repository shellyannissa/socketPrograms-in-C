#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 4444
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    if (sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    int val = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizoef(val));

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while (1)
    {
        fgets(buffer, sizeof(buffer), stdin);

        sendto(sockfd, buffer, 1024, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    }
}