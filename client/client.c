#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "../messages.h"
#include "../virement.h"
#include "../utils_v1.h"

#define SEM_KEY 667
#define SHM_KEY 020
#define PERM 0666
// PRE: ServierIP : a valid IP address
//      ServerPort: a valid port number
// POST: on success connects a client socket to ServerIP:port
//       return socket file descriptor
//       on failure, displays error cause and quits the program
int initSocketClient(char ServerIP[16], int Serverport)
{
    int sockfd = ssocket();
    sconnect(ServerIP, Serverport, sockfd);
    return sockfd;
}

int main(int argc, char **argv)
{
    /* retrieve player name */
    printf("Bienvenue dans le programe d'inscription au serveur de jeu\n");
    printf("Pour participer entrez votre nom :\n");
    StructMessage msg;
    Virement virement;

    int ret = sread(0, msg.messageText, MAX_PSEUDO);
    msg.messageText[ret - 1] = '\0';

    virement.num_destinataire = 4;
    virement.num_expediteur = 5;
    virement.montant = 45;

    int sockfd = initSocketClient(SERVER_IP, SERVER_PORT);
    swrite(sockfd, &msg, sizeof(msg));
    swrite(sockfd, &virement, sizeof(virement));

    /* wait server response
    sread(sockfd, &msg, sizeof(msg));

    if (msg.code == INSCRIPTION_OK)
    {
        printf("Réponse du serveur : Inscription acceptée\n");
    }
    else if (msg.code == INSCRIPTION_KO)
    {
        printf("Réponse du serveur : Inscription refusée\n");
    }*/

    sclose(sockfd);
    return 0;
}