#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

struct message
{
    char sender[1024];
    char message[1024];
};

void *receive_data(void *arg)
{
    int fd = *(int *)arg;
    struct message data;
    while ((recv(fd, &data, sizeof(data), 0)) > 0)
    {
        printf("[%s]: %s", data.sender, data.message);
    }
    return NULL;
}

int main()
{
    int fd;
    struct sockaddr_in addr;

    printf("Enter Your name: ");
    struct message data;
    scanf("%s", data.sender);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_data, (void *)&fd);

    while (1)
    {
        if (fgets(data.message, 1024, stdin) == NULL || strncmp(data.message, "bye", 3) == 0)
            break;

        send(fd, &data, sizeof(data), 0);
    }
    close(fd);
    return 0;
}