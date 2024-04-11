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

int main(int argc, char **argv)
{
    int fd;
    struct message data;

    if (argc != 2)
    {
        printf("usage %s <port number>", argv[0]);
        exit(1);
    }
    data.port = atoi(argv[1]);

    printf("Enter currency format(USD/ INR): ");
    fgets(data.curr, 4, stdin);

    printf("Enter value: ");
    scanf("%f", &(data.value));

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr, cli_addr;

    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(data.port);
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    sendto(fd, (struct message *)&data, sizeof(data), 0,
           (struct sockaddr *)&addr, sizeof(addr));
    float res;
    recvfrom(fd, &res, sizeof(res), 0,
             NULL, NULL);

    printf("Ans received from server: %.2f\n", res);
    close(fd);
    return 0;
}