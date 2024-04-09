#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        fprintf(stderr, "usage of %s port\n", argv[0]);
        exit(1);
    }
    int sockfd, newsockdf, portno, n;

    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    socklen_t clilen;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Error on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockdf = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockdf < 0)
    {
        error("Error on accept");
    }
    FILE *fp;
    int ch = 0;
    fp = fopen("new_file.txt", "a");
    int words;

    read(newsockdf, &words, sizeof(int));
    while (ch != words)
    {
        read(newsockdf, buffer, 256);
        fprintf(fp, " %s", buffer);
        ch++;
    }

    printf("The file has been received successfully\n");
}