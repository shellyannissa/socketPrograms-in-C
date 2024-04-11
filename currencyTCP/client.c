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

int main(int argc, char **argv)
{

    struct message data;
    int fd;

    printf("Enter currency format(USD/ INR): ");
    fgets(data.curr, 4, stdin);

    printf("Enter value: ");
    scanf("%f", &(data.value));

    fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    send(fd, &data, sizeof(data), 0);
    float res;
    recv(fd, &res, sizeof(res), 0);

    printf("Ans received from server: %.2f\n", res);
    close(fd);
    return 0;
}