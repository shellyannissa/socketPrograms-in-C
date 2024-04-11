#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_PORT 9000
struct Vehicle
{
    int id;
    int speed;
    int dir;
};

struct Vehicle vehicles[10];

void *receive_data(void *arg)
{
    int PORT = *(int *)arg;

    int fd;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    socklen_t addr_size = sizeof(addr);

    while (1)
    {
        int n = recvfrom(fd, vehicles, sizeof(vehicles), 0,
                         (struct sockaddr *)&addr, &addr_size);
        int count = n / sizeof(struct Vehicle);

        struct Vehicle temp;

        for (int i = 0; i < 10; i++)
        {
            temp = vehicles[i];
            if (temp.id == 0)
                break;
            printf("id: %d, speed: %d, dir: %d\n", temp.id, temp.speed, temp.dir);
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage is %s <port number>", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in serv_addr;
    int PORT = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    pthread_t recv;
    pthread_create(&recv, NULL, receive_data, (void *)&PORT);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.6");
    serv_addr.sin_port = htons(SERVER_PORT);

    while (1)
    {
        struct Vehicle data;
        printf("Enter speed: ");
        scanf("%d", &data.speed);
        printf("Enter direction(0/1): ");
        scanf("%d", &data.dir);

        data.id = PORT;

        if (sendto(sockfd, (struct Vehicle *)&data, sizeof(data), 0,
                   (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("Error while seding");
            exit(1);
        };

        sleep(20);
    }
    pthread_detach(&recv);
    close(sockfd);
    return 0;
}
