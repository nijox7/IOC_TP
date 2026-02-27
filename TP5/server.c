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

struct vote{
	char* name;
	char* choice;
	struct vote* next;
};

struct vote* v = NULL;

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int find_vote(char* name){
	// 0 -> pas trouvé
	// 1 -> trouvé
	struct vote* vote = v;
	for (; vote != NULL; vote = vote->next){
		if (strcmp(name, vote->name) == 0) return 1;
	}
	return 0;
}

void add_vote(char* name, char* choice){
	struct vote* first = malloc(sizeof(struct vote));
	first->name = malloc(strlen(name));
	strcpy(first->name, name);
	first->choice = malloc(strlen(choice));
	strcpy(first->choice, choice);
	first->next = v;
	v = first;
}

void free_vote(struct vote* v){
	if(v){
		free_vote(v->next);
		free(v->name);
		free(v->choice);
		free(v);
	}
}

void print_votes(){
	int hiver, ete = 0;
	int total = 0;

	struct vote* vote = v;
	for (; vote; vote=vote->next){
		printf("%s:%s\n", vote->name, vote->choice);
		total++;
		if(strcmp(vote->choice, "hiver") == 0) hiver++;
		else if(strcmp(vote->choice, "ete") == 0) ete++;
	}
	printf("État des votes actuel :\n");
	printf("Hiver:%d  Été:%d\n", hiver, ete);
	printf("Votes qui n'ont aucun sens WTFF:%d\n", total - (ete + hiver));
	printf("Nombre total de votes %d\n", total);
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


				if(strcmp(buffer, "fin") == 0) {
					close(newsockfd);
					break;
				}

				char buf[256];
				bcopy(buffer, buf, 256);

				char *reply = strtok(buf, ":");
				char *name = reply;
				reply = strtok(NULL, ":");
				char *choice = reply;

				// printf("name:%s, choice:%s\n", name, choice);

				if(find_vote(name)){
					printf("DEJA VOTE AAAAAAAAAAAAAA\n");
					strcpy(buffer, "Déjà voté"); 
					n = write(newsockfd,buffer,strlen(buffer)); // écrit un message sur la socket connectée au serveur
					if (n != strlen(buffer))
							error("ERROR message not fully transmetted");
				} else {
					strcpy(buffer, "Vote enregistré"); 
					n = write(newsockfd,buffer,strlen(buffer)); // écrit un message sur la socket connectée au serveur
					if (n != strlen(buffer))
							error("ERROR message not fully trasmetted");
					add_vote(name, choice);
				}

				print_votes();


                close(newsockfd); // ferme la socket de communication
        }

		free_vote(v);
        close(sockfd); // ferme la socket de connexion
        return 0;
}