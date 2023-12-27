#include <stdio.h>
#include <unistd.h>     // Benötigte Library für close()
#include <sys/socket.h> // Benötigte Bibliothek für die socket() funktion 
#include <sys/select.h> //select()
#include <arpa/inet.h> //struct sockaddr_in
#include <netinet/in.h>

#define PORT 8000
#define BUFFERSIZE 50

int main (int argc, char* argv[]){

    char recvbuf[BUFFERSIZE];

    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    if (sockfd == -1){
        perror("socket");
        return -1;
    }

    struct sockaddr_in6 my_addr;
    my_addr.sin6_len             = sizeof(my_addr);
    my_addr.sin6_family          = AF_INET6;
    my_addr.sin6_addr            = in6addr_any; 
    my_addr.sin6_port            = htons(PORT);

    struct sockaddr_in6 con_addr;

    if (bind(sockfd,(struct sockaddr *) &my_addr, sizeof(my_addr)) < 0 ){
        perror ("bind");
        close(sockfd);
        return -1;
    };

    listen(sockfd,5);
    socklen_t clilen = sizeof(con_addr);

    while(1){
        int newsockfd = accept(sockfd,(struct sockaddr* ) &con_addr, &clilen);
        if (newsockfd < 0){
            perror("accept");
            close(sockfd);
            return -1;
        };

        int recvlen = recv(newsockfd, recvbuf, BUFFERSIZE, 0);
        if(recvlen < 0){
            perror("recv");
            close(sockfd);
            close(newsockfd);
        }
        recvbuf[recvlen]= 0;
        printf("%s\n", recvbuf);

        close(sockfd);
        close(newsockfd);
        return 0;
    };
};
