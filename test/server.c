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

int clientCount = 0;
int fdList[10] = {-1};

void *recieve_data(void *arg)
{
    int serv_fd = *(int *)arg;
    struct message data;
    while ((recv(serv_fd, &data, sizeof(struct message), 0)) > 0)
    {
        for (int i = 0; i < 10; i++)
        {
            if (fdList[i] == -1)
            {
                break;
            }
            if (fdList[i] == serv_fd)
            {
                continue;
            }
            send(fdList[i], &data, sizeof(struct message), 0);
        }
    }
    close(serv_fd);
    return NULL;
}

int main()
{
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    listen(fd, 10);
    while (1)
    {
        int cli_fd = accept(fd, NULL, NULL);

        fdList[clientCount] = cli_fd;
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, recieve_data, (void *)&cli_fd);
        pthread_detach(thread_id);
        clientCount++;
    }
    close(fd);
    return 0;
}