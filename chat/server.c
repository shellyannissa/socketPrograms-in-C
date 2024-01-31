#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>//for read() and write()
#include<string.h>
#include<stdio.h>


void error(const char * msg){
    perror(msg);
    exit(0);
}
// port number has to be provided as command line argument

int main(int argc, char * argv[]){
    if(argc < 2){
        fprintf(stderr, "Port number not provided. Program terminated\n");
        exit(1);
    }
    int sockfd, newsockfd, portno, n;
    char buffer[255];

    struct sockaddr_in serv_addr,cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){
        error("Error opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);//host to network short

    //serv_addr is being typecaseted to sockaddr because bind takes a pointer to sockaddr
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Binding failed");
    }

    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if(newsockfd < 0){
        error("Error on accept");
    }

    while(1){
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if(n < 0){
            error("Error on reading");
        }
        printf("Client: %s", buffer);
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);

        n = write(newsockfd, buffer, strlen(buffer));
        if(n < 0){
            error("Error on writing");
        }

        int i = strncmp("Bye", buffer, 3);
        if(i == 0){
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}


