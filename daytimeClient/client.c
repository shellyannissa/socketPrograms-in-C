// dayTimeClient.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[100];
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        perror("usage: a.out <IPaddress>");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);

    /*The inet_pton function is used to convert an IPv4 or IPv6 address from its text representation
    to a binary format that can be used by network functions.*/
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton error");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }

    while ((n = read(sockfd, recvline, 100)) > 0)
    {
        recvline[n] = 0;
        if (fputs(recvline, stdout) == EOF)
            perror("fputs error");
    }

    if (n < 0)
        perror("read error");

    exit(0);
}

// daytime server address 206.168.112.96