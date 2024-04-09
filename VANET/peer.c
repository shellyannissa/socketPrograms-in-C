#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct VehicleData
{
    int vehicle_id;
    float speed;
    int direction; // 0 or 1 indicating direction of motion
};

void *receive_data(void *arg)
{
    int sockfd;
    struct sockaddr_in address;
    struct VehicleData vehicle_data;
    int addrlen = sizeof(address);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (const struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Receive data from other vehicles
        if (recvfrom(sockfd, (struct VehicleData *)&vehicle_data, sizeof(vehicle_data), 0, (struct sockaddr *)&address, (socklen_t *)&addrlen) < 0)
        {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        // Process received data
        printf("Received data from Vehicle ID %d: Speed=%.2f, Direction=%d\n",
               vehicle_data.vehicle_id, vehicle_data.speed, vehicle_data.direction);

        // Add logic here to update neighborhood based on received data
    }

    close(sockfd);
    return NULL;
}

int main()
{
    pthread_t receiver_thread;

    // Create receiver thread
    if (pthread_create(&receiver_thread, NULL, receive_data, NULL) != 0)
    {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // Main thread continues for sending data

    struct sockaddr_in address;
    int sockfd;
    struct VehicleData vehicle_data;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("255.255.255.255"); // Broadcast address

    // Set broadcast option
    int broadcast_enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Main loop for sending data
    while (1)
    {
        // Prepare data to send
        vehicle_data.vehicle_id = 123; // Example vehicle ID
        vehicle_data.speed = 60.0;     // Example speed
        vehicle_data.direction = 1;    // Example direction

        // Send data to all vehicles
        if (sendto(sockfd, (const struct VehicleData *)&vehicle_data, sizeof(vehicle_data), 0, (const struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("Sendto failed");
            exit(EXIT_FAILURE);
        }

        printf("Sent data: Vehicle ID %d, Speed=%.2f, Direction=%d\n",
               vehicle_data.vehicle_id, vehicle_data.speed, vehicle_data.direction);

        sleep(5); // Send data every 5 seconds
    }

    close(sockfd);
    pthread_join(receiver_thread, NULL);
    return 0;
}
