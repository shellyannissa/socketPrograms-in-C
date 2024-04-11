#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

struct message
{
    char curr[4];
    float value;
    int port;
};

void *handle_input(void *arg)
{
    struct message data = *((struct message *)arg);
    float res;
    int fd;

    if ((strncmp(data.curr, "USD", 3)) == 0)
    {
        res = data.value * 83.38;
    }
    if ((strncmp(data.curr, "INR", 3)) == 0)
    {
        res = data.value / 83.38;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(data.port);

    sendto(fd, &res, sizeof(res), 0,
           (struct sockaddr *)&addr, sizeof(addr));
    close(fd);
    return NULL;
}

int main()
{
    int fd;
    struct sockaddr_in serv_addr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9000);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    struct message data;

    while (1)
    {
        int n = recvfrom(fd, (struct message *)&data, sizeof(data), 0,
                         NULL, NULL);
        pthread_t helper;

        pthread_create(&helper, NULL, handle_input, (void *)&data);
        pthread_detach(helper);
    }
    return 0;
}