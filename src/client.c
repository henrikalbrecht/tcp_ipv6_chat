#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> // Benötigte Bibliothek für die socket() funktion 
#include <sys/select.h> // Benötigte Bibliothek für die select() funktion
#include <sys/time.h>
#include <string.h>     // benötigte Bibliothek für String funktionen, z.B. memcpy(), strlen()
#include <arpa/inet.h> //struct sockaddr_in
#include <netinet/in.h>

int main (int argc, char* argv[]){
    
    //Speciherung der Argumente in passende Variablen
    char server_ipv6[128];
    memcpy(server_ipv6,argv[1], strlen(argv[1])+1);
    int server_port         = atoi(argv[2]);
    int s_nummer            = atoi(argv[3]);

    //fd_set read_fds;    //deklariert das Lese Filedeskriptorset
    //fd_set write_fds;   //deklariert das Schreib Filedeskriptorset
    //fd_set error_fds;   //deklariert das Fehler Filedeskriptorset 

    printf("Anzahl der übergebenen Parameter: %i \n", argc);
    printf("Die IPv6 Adresse lautet: %s\n", server_ipv6);
    printf("Der Port lautet: %i\n", server_port);
    printf("Die sNummer lautet: %i\n", s_nummer);

    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    if (sockfd == -1){
        perror("socket");
        return -1;
    }

    struct sockaddr_in6 server_addr;;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin6_len             = sizeof(server_addr);
    server_addr.sin6_family          = AF_INET6;
    server_addr.sin6_port            = htons(server_port);

    if(inet_pton(AF_INET6, server_ipv6,server_addr.sin6_addr.s6_addr) < 1){
        perror("inet_pton");
        return -1;
    } 


    if (connect(sockfd,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        return -1;
    }


    //Endlosschleife
   while(1){       
//
   // struct timeval timeout;
   // timeout.tv_sec = 5;  // Wartezeit in Sekunden
   // timeout.tv_usec = 0; // Zusätzliche Wartezeit in Mikrosekunden
//
   //     printf("eins vor select");
   //     int data_status = select(error_descriptor+1, &read_fds, &write_fds, &error_fds, &timeout);  // Überprüft ob ein input kam und speichert diese information in data_status
//
   //     printf("select geht durch");
   //     //Überprüfung ob beim Ausführen von select() ein Fehler aufgetreten ist
   //     if(data_status == -1){   
   //         printf("select");
   //         return -1;
   //     };
//
   //     //Überprüfung ob ein Input erkannt wurde, falls ja Überprüfung was für eine Art von Input
   //     if(data_status > 0){
   //         if(FD_ISSET(read_descriptor,&read_fds)){
   //             printf("Lese Input erkannt");
   //         };
   //         if(FD_ISSET(write_descriptor,&write_fds)){
   //             printf("Schreib Input erkannt");
   //         };
   //         if(FD_ISSET(error_descriptor,&error_fds)){
   //             printf("Fehler Input erkannt");
   //         };
   //     }
   //     printf("Kein Input Erkannt!");
//
   // timeout.tv_sec = 5;  // Wartezeit in Sekunden
   // timeout.tv_usec = 0; // Zusätzliche Wartezeit in Mikrosekunden
    };




}