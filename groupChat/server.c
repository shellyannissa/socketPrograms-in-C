#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define LISTENQ 8

struct thread_arguments
{
    int connectfd;
    int sockfd;
    struct sockaddr_in cli_addr;
};

struct thread_arguments *clients[10];
int client_count = 0;

static void *handle_client(void *args)
{
    struct thread_arguments *thread_args = (struct thread_arguments *)args;
    clients[client_count] = thread_args;
    client_count++;
    char buffer[BUFFER_SIZE] = {0};

    while (recv(thread_args->connectfd, buffer, BUFFER_SIZE, 0) > 0)
    {
        printf("recieved from client: %s", buffer);
        for (int i = 0; i < client_count; i++)
        {
            if (clients[i]->connectfd != thread_args->connectfd)
            {
                send(clients[i]->connectfd, buffer, BUFFER_SIZE, 0);
            }
        }
        memset(buffer, '\0', sizeof(buffer));
    }
    close(thread_args->connectfd);
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        perror("Usage: ./client <ip>");
        exit(1);
    }

    int sockfd;
    pthread_t helper;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t addr_size = sizeof(cli_addr);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding");
        exit(1);
    }

    listen(sockfd, LISTENQ);
    while (1)
    {

        int connectfd = accept(sockfd, (struct sockaddr *)&cli_addr, &addr_size);
        if (connectfd < 0)
        {
            perror("Error accepting connection");
            exit(1);
        }

        struct thread_arguments *arg = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
        arg->connectfd = connectfd;
        arg->sockfd = sockfd;
        memcpy(&(arg->cli_addr), &cli_addr, addr_size);

        pthread_create(&helper, NULL, handle_client, (void *)arg);

        pthread_detach(helper);
    }
    exit(0);
}
