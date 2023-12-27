#include "../include/paket_structure.h"
#include <stdio.h>
#include <string.h>     // Benötigte Library für strcpy()
#include <unistd.h>     // Benötigte Library für close()
#include <sys/socket.h> // Benötigte Bibliothek für die socket() funktion 
#include <sys/select.h> //select()
#include <arpa/inet.h> //struct sockaddr_in

#define PORT 8000
#define SNUMMER "s85551"

int main (int argc, char* argv[]){

    //Variablendeklarierung und -initialisierung
    struct packet send_paket, recv_paket;
    int maxsock, recvlen, stdinfd, sockfd, newsockfd;
    fd_set read_fds;

    //Initialiserung des Timeout structs für select
    struct timeval timeout;
    timeout.tv_sec = 1;  // Wartezeit in Sekunden
    timeout.tv_usec = 0; // Zusätzliche Wartezeit in Mikrosekunden
    
    //Verarbeitung der Parameter
    strcpy(send_paket.sNummer, SNUMMER);

    //Initialisierung des stdin Filedeskriptors
    stdinfd = fileno(stdin);

    //Initialisierung des IPv6 TCP Sockets
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("socket");
        return -1;
    }

    struct sockaddr_in6 my_addr, con_addr;
    socklen_t clilen = sizeof(con_addr);
    bzero((char*)&my_addr, sizeof(my_addr));
    my_addr.sin6_len             = sizeof(my_addr);
    my_addr.sin6_family          = AF_INET6;
    my_addr.sin6_addr            = in6addr_any; 
    my_addr.sin6_port            = htons(PORT);

    //Binden der Adresse an den Filedeskriptor
    if (bind(sockfd,(struct sockaddr *) &my_addr, sizeof(my_addr)) < 0 ){
        perror ("bind");
        close(sockfd);
        return -1;
    };

    //Hören ob jemand auf den Socket sich verbinden möchte, maximal 5 clients erlaubt
    listen(sockfd,5);

    //Akzeptieren einer eingehenden Verbindungsanfrage
    newsockfd = accept(sockfd,(struct sockaddr* ) &con_addr, &clilen);
    if (newsockfd < 0){
        perror("accept");
        close(sockfd);
        return -1;
    };

    //Setup des Filedescriptorsets
    FD_ZERO(&read_fds);
    FD_SET(newsockfd, &read_fds);
    FD_SET(stdinfd,&read_fds);

    //Überprüfung nach dem größten Filedescriptor im Set
    if(stdinfd > newsockfd){
        maxsock = stdinfd;
    }else{
        maxsock = newsockfd;
    }

    while(1){
        //Initialisierung eines temporären fd_sets da select destructive ist
        fd_set current_fds = read_fds;        
        //Aufruf von select zum Multiplexing
        int data_status = select(maxsock+1, &current_fds, NULL,NULL, &timeout);  // Überprüft ob ein input kam und speichert diese information in data_status

        //Überprüfung ob beim Ausführen von select() ein Fehler aufgetreten ist
        if(data_status == -1){   
            perror("select");
            close(newsockfd);
            close(sockfd);
            return -1;
        };

        //Überprüfung ob ein Input erkannt wurde, falls ja Überprüfung was für eine Art von Input
        if(data_status > 0){
            //Falls Keyboard Input erkannt, dann wird dieser gesendet
            if(FD_ISSET(stdinfd,&current_fds)){
                fgets(send_paket.text,sizeof(send_paket.text),stdin);
                send(newsockfd,(void*) &send_paket,sizeof(struct packet),0);
            };
            //Falls Input vom Kommunikationspartner erkannt dann wird dieser received und ausgegeben
            if(FD_ISSET(newsockfd,&current_fds)){
                recvlen = recv(newsockfd, (void *)&recv_paket, sizeof(struct packet), 0);
                if(recvlen < 0){
                perror("recv");
                }
                printf("%s:%s",recv_paket.sNummer,recv_paket.text);
            };
        }
    };
    //socket schließen und programm sauber beenden
    //Toter Code da vorher eine endlosschleife läuft und mit strg+c gekillt werden muss
    close(sockfd);
    return 0;
};
