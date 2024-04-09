#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LISTENQ 10

struct thread_arguments
{
    char ip[20];
    int receiver_port;
    int sender_port;
};

static void *receiver(void *arg)
{
    struct thread_arguments *args;
    args = (struct thread_arguments *)arg;

    int n, listenfd, connfd;
    struct sockaddr_in cliaddr, servaddr;
    socklen_t clilen;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Problem in creating the socket");
        exit(2);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(args->receiver_port);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);
    printf("Listening %d\n", args->receiver_port);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

    char buf[1024];
    // int port=(int)ntohs(cliaddr.sin_port);
    while ((n = recv(connfd, buf, 1024, 0)) > 0)
    {

        printf("[%d] %s\n", args->sender_port, buf);
    }
}

int main(int argc, char **argv)
{
    int listenfd, sockfd, connfd;
    struct sockaddr_in cliaddr, servaddr;
    pthread_t h1;
    struct thread_arguments *args, *argr;
    socklen_t clilen;

    if (argc != 4)
    {
        printf("./program listenPort IP sendPort\n");
        return 0;
    }

    argr = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
    argr->receiver_port = atoi(argv[1]);
    argr->sender_port = atoi(argv[3]);
    pthread_create(&h1, NULL, receiver, (void *)argr);
    pthread_detach(h1);

    sleep(10);
    args = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
    strcpy(args->ip, argv[2]);
    args->receiver_port = atoi(argv[3]);
    args->sender_port = atoi(argv[1]);

    int n, sockfd1;
    struct sockaddr_in servaddr1;

    if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Problem in creating the socket");
        exit(2);
    }

    memset(&servaddr1, 0, sizeof(servaddr1));
    servaddr1.sin_family = AF_INET;
    servaddr1.sin_addr.s_addr = inet_addr(args->ip);
    servaddr1.sin_port = htons(args->receiver_port);

    if (connect(sockfd1, (struct sockaddr *)&servaddr1, sizeof(servaddr1)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(3);
    }
    else
    {
        printf("Connection success\n");
    }

    char buf[1024];

    while ((n = scanf("%[^\n]%*c", buf)) > 0)

        send(sockfd1, buf, strlen(buf) + 1, 0);

    return 0;
}
