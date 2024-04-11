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
    int fd;
};

void *handle_thread(void *arg)
{
    struct message data = *((struct message *)arg);
    int fd = data.fd;

    float res;

    if ((strncmp(data.curr, "USD", 3)) == 0)
    {
        res = data.value * 83.38;
    }
    if ((strncmp(data.curr, "INR", 3)) == 0)
    {
        res = data.value / 83.38;
    }

    send(fd, &res, sizeof(float), 0);

    close(fd);
    return NULL;
}

int main()
{
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));

    listen(fd, 5);

    while (1)
    {
        int clifd = accept(fd, NULL, NULL);

        struct message data;
        recv(clifd, &data, sizeof(data), 0);
        data.fd = clifd;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_thread, (void *)&data);

        pthread_detach(thread_id);
    }
    while (1)
    {
    };
    close(fd);
    return 0;
}