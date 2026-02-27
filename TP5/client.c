#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
        perror(msg);
        exit(0);
}

int main(int argc, char *argv[])
{
        int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        char buffer[256];

        // Le client doit connaitre l'adresse IP du serveur, et son numero de port
        if (argc < 5) {
                fprintf(stderr,"usage %s hostname port\n", argv[0]);
                exit(0);
        }
        portno = atoi(argv[2]);

        // 1) Création de la socket, INTERNET et TCP

        sockfd = socket(AF_INET, SOCK_STREAM, 0); // créer une socket en protocole IPV4(AF_INET) et TCP(SOCK_STREAM)
        if (sockfd < 0)
                error("ERROR opening socket");

        server = gethostbyname(argv[1]); // récupère des informations sur l'hôte (list de ses adresses,)
        if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }

        // On donne toutes les infos sur le serveur

        bzero((char *) &serv_addr, sizeof(serv_addr)); // remplit la structure serv_addr de 0
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); // copie l'adresse du serveur dans le champ correspondant
        serv_addr.sin_port = htons(portno); // convertit les 2 octets (short) du ports dans le bon endianness

        // On se connecte. L'OS local nous trouve un numéro de port, grâce auquel le serveur
        // peut nous renvoyer des réponses, le \n permet de garantir que le message ne reste
        // pas en instance dans un buffer d'emission chez l'emetteur (ici c'est le clent).

        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) // tentative de connexion au serveur
                error("ERROR connecting");

        if(strcmp(argv[4], "fin") == 0) {
                sprintf(buffer, "fin");
                n = write(sockfd,buffer,strlen(buffer));
                goto fin;
        }


        sprintf(buffer, "%s:%s", argv[3], argv[4]); // on envoie nom:vote (exemple "clement:hiver")
        n = write(sockfd,buffer,strlen(buffer)); // écrit un message sur la socket connectée au serveur
        if (n != strlen(buffer))
                error("ERROR message not fully trasmetted");

        n = read(sockfd, buffer, 255);
        if (n < 0) error("ERROR reading from socket");
        printf("Réponse du serveur: %s\n", buffer);

        fin:
        // On ferme la socket
        close(sockfd);
        return 0;
}