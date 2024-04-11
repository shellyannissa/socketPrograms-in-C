#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

struct vehicle
{
	int vehicle_id;
	float speed;
	int direction;
};

struct vehicle vehicleList[5];

#define VEHICLE_COUNT 5

void *receive_data(void *arg)
{
	int PORT = *(int *)arg;
	struct sockaddr_in address;
	socklen_t addr_len = sizeof(address);
	struct sockaddr_in cli_addr;
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error while creating socket");
		exit(1);
	}
	memset(&address, '\0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(1);
	}

	struct vehicle vehicle_data;
	while (1)
	{
		if (recvfrom(sockfd, (struct vehicle *)&vehicle_data, sizeof(vehicle_data), 0,
					 (struct sockaddr *)&cli_addr, &addr_len) < 0)
		{
			perror("Error while receieving");
			exit(1);
		}
		int idx = vehicle_data.vehicle_id - 8000;
		vehicleList[idx] = vehicle_data;
		struct vehicle self = vehicleList[PORT - 8000];

		for (int i = 0; i < VEHICLE_COUNT; i++)
		{
			struct vehicle temp = vehicleList[i];
			if (temp.vehicle_id != -1 && temp.vehicle_id != self.vehicle_id &&
				temp.direction == self.direction)
			{
				printf("Vehicle id: %d, speed: %.2f, direction: %d\n",
					   temp.vehicle_id, temp.speed, temp.direction);
			}
		}
		printf("\n");
	}
	close(sockfd);
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("format: %s <port number>", argv[0]);
		exit(1);
	}
	int PORT = atoi(argv[1]);
	if (PORT < 8000 || PORT > 8000 + VEHICLE_COUNT - 1)
	{
		printf("Invalid port number");
		exit(1);
	}

	int sockfd;
	pthread_t thread_id;
	struct sockaddr_in peer_addr;
	struct vehicle vehicle_data;

	// initialising vehicles datastructure
	for (int i = 0; i < VEHICLE_COUNT; i++)
		vehicleList[i].vehicle_id = -1;

	// creating thread for receieving data
	pthread_create(&thread_id, NULL, receive_data, (void *)&PORT);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error while creating socket");
		exit(1);
	}

	memset(&peer_addr, '\0', sizeof(peer_addr));
	peer_addr.sin_family = AF_INET;
	peer_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	vehicle_data.vehicle_id = PORT;

	while (1)
	{
		printf("Enter speed: ");
		scanf("%f", &vehicle_data.speed);
		printf("Enter direction(0/1): ");
		scanf("%d", &vehicle_data.direction);

		for (int i = 0; i < VEHICLE_COUNT; i++)
		{
			peer_addr.sin_port = htons(8000 + i);
			sendto(sockfd, (const struct vehicle *)&vehicle_data, sizeof(vehicle_data), 0,
				   (struct sockaddr *)&peer_addr, sizeof(peer_addr));
		}
		sleep(15);
	}

	close(sockfd);
	pthread_join(thread_id, NULL);
	return 0;
}
