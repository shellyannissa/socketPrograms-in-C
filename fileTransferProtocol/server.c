#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#define SIZE 1024

void send_file(FILE *fp, int sockfd)
{
    char data[SIZE] = {0};

    while(fgets(data, SIZE, fp)!=NULL)
    {
        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("Error in sendung data");
            exit(1);
        }
        bzero(data, SIZE);
    }
}



int main ()
{
    char *ip = "192.168.41.44";
    int port = 8080;
    // int port = 3002;
    int e;
    pid_t childpid;

    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];


        FILE *fp;
    char filename[SIZE] ;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        perror("Error in socket");
        exit(1);
    }
     printf("Server socket created. \n");

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = port;
     server_addr.sin_addr.s_addr = inet_addr(ip);

     e = bind(sockfd,(struct sockaddr*)&server_addr, sizeof(server_addr));
     if(e<0)
     {
         perror("Error in Binding");
         exit(1);
     }
     printf("Binding Successfull.\n");

     e = listen(sockfd, 10);
     if(e==0)
     {
         printf("Listening...\n");
     }
     else 
     {
         perror("Error in Listening");
         exit(1);
     }


    for(;;){
        addr_size = sizeof(new_addr);
        new_sock = accept(sockfd,(struct sockaddr*)&new_addr, &addr_size);


        if((childpid =fork())==0){
            close(sockfd);
            int n;
            n =recv(new_sock, filename, SIZE, 0);
            fp = fopen(filename ,"r");
            if(fp == NULL)
            { 
                n = recv(sockfd, buffer, SIZE, 0);
                perror("Error in reading file.");
                exit(1);
            }
            send_file(fp,new_sock);
            printf("Data written in the text file\n");
            // send(new_sock, "File data sent", 1024, 0);
            exit(0);
        }
        close(new_sock);
        bzero(filename, SIZE);

    }
    return 0;

}