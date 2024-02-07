/*
Problem Statement:
The server is supposed to work as a monitoring station that receives the temperature and battery level details at from client stations
The server will tabulate the average of temperature readings and send it back to client, and would also notify if the battery levels drops below the critical value 30
*/

#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 20 // Maximum number of concurrent clients

// Define sizes for station and temperature
#define MAX_LEN 256
#define TEMPERATURE_SIZE 256
#define FORECAST_PORT 3000

double cumulativeTemp = 0;
double clientCount = 0;


// The arguments needed to send a response
struct thread_arguments {
    char temperature[MAX_LEN];
    char battery[MAX_LEN];  
    struct sockaddr client_address;
    socklen_t       client_length;
};

// Function to handle client requests
static void *send_response(void *arg)
{
    char out[TEMPERATURE_SIZE];
    struct thread_arguments *args = (struct thread_arguments *)arg;

    // Simulate some processing or fetching temperature data
    // and preparing the response
    double temperature = atoi(args->temperature);
    double battery = atoi(args->battery);

    cumulativeTemp += temperature;
    clientCount ++;
    double avg = cumulativeTemp / clientCount;
    
    if(battery < 30) {
        snprintf(out, TEMPERATURE_SIZE, "\nBattery Level below critical Value.\nAverage:%5.1f", avg);
    }
    else
        snprintf(out, TEMPERATURE_SIZE, "\nAverage:%5.1f", avg);

    int fd = socket(AF_INET, SOCK_DGRAM, 0); 
    sendto(fd, out, TEMPERATURE_SIZE, 0, &(args->client_address), args->client_length);
    close(fd);

    // Free the memory used by the arguments
    free(args);

    return NULL;
}

int main(void)
{
    char in[MAX_LEN];
    int fd;
    pthread_t helpers[MAX_CLIENTS];
    struct sockaddr_in address, client_address;
    socklen_t client_length = sizeof(struct sockaddr);

    // Create an IP4/UDP socket
    fd = socket(AF_INET, SOCK_DGRAM, 0); 

    // Initialize the address of this host
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(FORECAST_PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY); // In case the host has multiple

    // Bind the socket to the address
    bind(fd, (struct sockaddr *)&address, sizeof(struct sockaddr));

    // Process requests
    while (1)
    {
        struct thread_arguments *args = (struct thread_arguments *)malloc(sizeof(struct thread_arguments));
        // Receive the request
        recvfrom(fd, in, MAX_LEN, 0,
                (struct sockaddr *)&client_address, &client_length);
        strncpy(args->temperature, in, MAX_LEN);
        recvfrom(fd, in, MAX_LEN, 0,
                (struct sockaddr *)&client_address, &client_length);
        strncpy(args->battery, in, MAX_LEN);

        // Make a copy of the message and the client address
        // for the thread that will handle the response
        memcpy(&(args->client_address), &client_address, client_length);
        args->client_length = client_length;

        // Create a thread to handle the response
        pthread_t *helper = malloc(sizeof(pthread_t));
        pthread_create(helper, NULL, send_response, (void *)args);

        // Store the thread handle for later joining
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (!helpers[i]) {
                helpers[i] = *helper;
                break;
            }
        }
        if (i == MAX_CLIENTS) {
            fprintf(stderr, "Maximum clients reached, cannot accept more connections\n");
            free(helper);
        }
    }

    // Join all the threads
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (helpers[i]) {
            pthread_join(helpers[i], NULL);
        }
    }

    close(fd);
    return 0;
}



