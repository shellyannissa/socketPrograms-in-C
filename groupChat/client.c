#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define LISTENQ 8

struct thread_arguments
{
    int connectfd;
};

static void *sender(void *args)
{
    struct thread_arguments *arg_sender = (struct thread_arguments *)args;
    char buffer[BUFFER_SIZE] = {0};

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
    {
        send(arg_sender->connectfd, buffer, BUFFER_SIZE, 0);
    }
    return NULL;
}

static void *reciever(void *args)
{
    struct thread_arguments *arg_reciever = (struct thread_arguments *)args;

    char buffer[BUFFER_SIZE] = {0};

    while (recv(arg_reciever->connectfd, buffer, BUFFER_SIZE, 0) > 0)
    {
        printf("[RECV]: %s", buffer);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serv_addr;
    struct thread_arguments *arg_sender, *arg_receiver;
    pthread_t recv, send;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Error creating socket for sending\n");
        exit(1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to server \n");
        exit(1);
    }
    printf("Connection succes\n");
    // for recieving messages
    arg_receiver = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
    arg_receiver->connectfd = socket_fd;
    pthread_create(&recv, NULL, reciever, (void *)arg_receiver);
    pthread_detach(recv);

    // for sending messages
    arg_sender = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
    arg_sender->connectfd = socket_fd;
    pthread_create(&send, NULL, sender, (void *)arg_sender);
    pthread_detach(send);

    // preventing program from exiting
    while (3)
    {
    }
    exit(0);
}
