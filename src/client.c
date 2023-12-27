#include "../include/paket_structure.h"
#include <stdio.h>
#include <stdlib.h>     // Benötigte Library für atoi()
#include <unistd.h>     // Benötigte Library für close()
#include <sys/socket.h> // Benötigte Bibliothek für die socket() funktion 
#include <sys/select.h> // Benötigte Bibliothek für die select() funktion
#include <string.h>     // benötigte Bibliothek für String funktionen, z.B. memcpy(), strlen()
#include <arpa/inet.h>  //struct sockaddr_in

int main (int argc, char* argv[]){
    
    if (argc != 4){
        printf("Missing arguments\n You need to provide the following arguments: ipv6 adress, port, s_number");
        return -1;
    };
    //Variablendeklarierung und -initialisierung
    struct packet send_paket, recv_paket;
    char server_ipv6[128];
    int server_port, maxsock, recvlen, stdinfd, sockfd;
    fd_set read_fds;
    
    //Initialiserung des Timeout structs für select
    struct timeval timeout;
    timeout.tv_sec = 1;  // Wartezeit in Sekunden
    timeout.tv_usec = 0; // Zusätzliche Wartezeit in Mikrosekunden

    //Verarbeitung der Parameter
    strcpy(server_ipv6,argv[1]);
    server_port = atoi(argv[2]);
    strcpy(send_paket.sNummer,argv[3]);

    //Initialisierung des stdin Filedeskriptors
    stdinfd = fileno(stdin);

    //Initialisierung des IPV6 TCP Sockets
    sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("socket");
        return -1;
    }

    //Initialisieren des Sockaddr_in6 structs
    struct sockaddr_in6 server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin6_len             = sizeof(server_addr);
    server_addr.sin6_family          = AF_INET6;
    server_addr.sin6_port            = htons(server_port);

    //Setup des Filedescriptorsets
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    FD_SET(stdinfd,&read_fds);

    //Überprüfung nach dem größten Filedescriptor im Set
    if(stdinfd > sockfd){
        maxsock = stdinfd;
    }else{
        maxsock = sockfd;
    }

    //setzen der IPv6 Adresse in das server_addr struct
    if(inet_pton(AF_INET6, server_ipv6,server_addr.sin6_addr.s6_addr) < 1){
        perror("inet_pton");
        close(sockfd);
        return -1;
    } 

    //connection aufruf zum Server -> Start des three way handshake
    if (connect(sockfd,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        close(sockfd);
        return -1;
    }
    
    while(1){
        //Initialisierung eines temporären fd_sets da select destructive ist
        fd_set current_fds = read_fds;

        //Aufruf von select zum Multiplexing
        int data_status = select(maxsock+1, &current_fds, NULL,NULL, &timeout);

        //Überprüfung ob beim Ausführen von select() ein Fehler aufgetreten ist
        if(data_status == -1){   
            perror("select");
            close(sockfd);
            return -1;
        };

        //Überprüfung ob ein Input erkannt wurde, falls ja Überprüfung was für eine Art von Input
        if(data_status > 0){
            //Falls Keyboard Input erkannt, dann wird dieser gesendet
            if(FD_ISSET(stdinfd,&current_fds)){
                fgets(send_paket.text,sizeof(send_paket.text),stdin);
                if(send(sockfd,(void*) &send_paket,sizeof(struct packet),0) != sizeof(struct packet)){
                    perror("send");
                    close(sockfd);
                    return -1;
                };
                
            };
            //Falls Input vom Kommunikationspartner erkannt dann wird dieser received und ausgegeben
            if(FD_ISSET(sockfd,&current_fds)){
                recvlen = recv(sockfd, (void *)&recv_paket, sizeof(struct packet), 0);
                if(recvlen < 0){
                perror("recv");
                }
                printf("%s:%s",recv_paket.sNummer,recv_paket.text);
            };
        }

    }

    //socket schließen und programm sauber beenden
    //Toter Code da vorher eine endlosschleife läuft und mit strg+c gekillt werden muss
    close(sockfd);
    return 0;

}