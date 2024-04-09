#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        fprintf(stderr, "usage %s port\n", argv[0]);
        exit(1);
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[1024];

    socklen_t addr_size;

    int n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Error opening socket");
        exit(1);
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    n = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (n < 0)
    {
        perror("Error on binding");
        exit(1);
    }

    bzero(buffer, 1024);
    addr_size = sizeof(cli_addr);
    n = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&cli_addr, &addr_size);
    if (n < 0)
    {
        perror("Error on recvfrom");
        exit(1);
    }
    printf("Client: %s\n", buffer);
}