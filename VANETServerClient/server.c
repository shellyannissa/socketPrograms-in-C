#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9000
#define MAX_VEHICES 10

struct Vehicle
{
    int id;
    int speed;
    int dir;
};

struct Vehicle vehicleList[10];

int clientCount = 0;
pthread_mutex_t lock;

void *receive_data(void *arg)
{
    int sockfd;
    socklen_t addr_size;
    struct sockaddr_in serv_addr, cli_addr;
    addr_size = sizeof(serv_addr);
    struct Vehicle data;

    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error while creating socket\n");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error while binding");
        exit(1);
    }

    while (1)
    {
        recvfrom(sockfd, (struct Vehicle *)&data, sizeof(data), 0,
                 (struct sockaddr *)&cli_addr, &addr_size);

        int id = data.id;
        int flag = 0;

        pthread_mutex_lock(&lock);

        for (int i = 0; i < clientCount; i++)
        {

            if (vehicleList[i].id == id)
            {
                vehicleList[i].dir = data.dir;
                vehicleList[i].speed = data.speed;
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            vehicleList[clientCount].id = data.id;
            vehicleList[clientCount].speed = data.speed;
            vehicleList[clientCount].dir = data.dir;
        }
        clientCount++;
        pthread_mutex_unlock(&lock);
    }
}
int main()
{
    int sockfd;
    pthread_t thread_id;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&thread_id, NULL, receive_data, (void *)NULL);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("error while socket creation");
        exit(1);
    }
    struct sockaddr_in cli_addr;

    memset(&cli_addr, 0, sizeof(cli_addr));

    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    while (1)
    {
        for (int i = 0; i < clientCount; i++)
        {
            if (vehicleList[i].id == 0)
                break;
            cli_addr.sin_port = htons(vehicleList[i].id);
            sendto(sockfd, (struct Vehicle **)vehicleList, sizeof(vehicleList), 0,
                   (struct sockaddr *)&cli_addr, sizeof(cli_addr));
        }
        sleep(30);
    }

    pthread_join(thread_id, NULL);

    pthread_mutex_destroy(&lock);

    return 0;
}