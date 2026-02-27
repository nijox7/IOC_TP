/* A simple server in the internet domain using TCP The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        if (argc < 2) {
                fprintf(stderr, "ERROR, no port provided\n");
                exit(1);
        }

        // 1) on crée la socket, SOCK_STREAM signifie TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INTE: IPV4, SOCK_STREAM->TCP, 0 pas d'options
        if (sockfd < 0)
                error("ERROR opening socket");

        // 2) on réclame au noyau l'utilisation du port passé en paramètre 
        // INADDR_ANY dit que la socket va être affectée à toutes les interfaces locales

        bzero((char *) &serv_addr, sizeof(serv_addr));  // remplit de zéro la zone mémoire à l'adresse serv_addr
        portno = atoi(argv[1]);
        serv_addr.sin_family = AF_INET;                 // famille d'adresse IPV4              
        serv_addr.sin_addr.s_addr = INADDR_ANY;         // accepte n'importe quelle IP (IPV4)
        serv_addr.sin_port = htons(portno);             // port d'écoute
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                error("ERROR on binding");


        // On commence à écouter sur la socket. Le 5 est le nombre max
        // de connexions pendantes

        listen(sockfd, 5);  // crée une file d'attente pour se connecter sur la socket sockfd
        while (1) {
                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // accepte la connexion d'un client
                                                                                    // et récupère la socket de communication avec le client
                if (newsockfd < 0)
                    error("ERROR on accept");

                bzero(buffer, 256); // on vide le buffer (remplit de 0)
                n = read(newsockfd, buffer, 255); // lit ce que le client écrit dans la socket (receive)
                if (n < 0)
                    error("ERROR reading from socket");

                printf("Received packet from %s:%d\nData: [%s]\n\n",
                       inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port),
                       buffer);
                        // inet_ntoa -> convertit l'adresse ip en chaine de caractères (network -> ascii)
                        // ntohs  -> convertit le port dans le bon endianness
                        // buffer -> message reçu
                close(newsockfd); // ferme la socket de communication
        }

        close(sockfd); // ferme la socket de connexion
        return 0;
}