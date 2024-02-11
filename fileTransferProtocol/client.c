#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define SIZE 1024


void write_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename = "fileClient.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("Error in creating file.");
        exit(1);
    }
    while(1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if(n<=0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        bzero(buffer, SIZE);
    }
    return;
    
}


int main()
{
    char *ip = "192.168.41.44";
    int port = 8080;
    // int port =9000;
    int e;

    int sockfd;
    struct sockaddr_in server_addr;
     char sendline[SIZE];
    // FILE *fp;
    // char *filename = "file2.txt";
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("Error in socket");
        exit(1);
    }


     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
        //  server_addr.sin_port = htons(port);
     server_addr.sin_addr.s_addr = inet_addr(ip);

     e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e == -1)
     {
         perror("Error in Connecting");
         exit(1);
     }

     printf("Enter filename:");

// fgets(sendline, SIZE, stdin);
scanf("%s", sendline);
  send(sockfd, sendline, strlen(sendline), 0);

         write_file(sockfd);
    // recv(sockfd, sendline,SIZE, 0);
    // printf("%s", sendline);
     close(sockfd);
     printf("Disconnected from the server. \n");
     return 0;

}