#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../messages.h"
#include "../utils_v1.h"

#define MAX_PLAYERS 2
#define BACKLOG 5
#define TIME_INSCRIPTION 15

typedef struct Player
{
	char pseudo[MAX_PSEUDO];
	int sockfd;
	int shot;
} Player;

/* globals variables */
Player tabPlayers[MAX_PLAYERS];
volatile sig_atomic_t end_inscriptions = 0;
// int end_inscriptions = 0;


void endServerHandler(int sig)
{
	end_inscriptions = 1;
}

void terminate(Player *tabPlayers, int nbPlayers)
{
	printf("\nJoueurs inscrits : \n");
	for (int i = 0; i < nbPlayers; i++)
	{
		printf("  - %s inscrit\n", tabPlayers[i].pseudo);
	}
	exit(0);
}

// PRE:  ServerPort: a valid port number
// POST: on success bind a socket to 0.0.0.0:port and listen to it
//       return socket file descriptor
//       on failure, displays error cause and quits the program
int initSocketServer(int port)
{
	int sockfd = ssocket();

	/* no socket error */

	sbind(port, sockfd);

	/* no bind error */
	slisten(sockfd, BACKLOG);

	// setsockopt -> to avoid Address Already in Use
	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	/* no listen error */
	return sockfd;
}

int main(int argc, char **argv)
{

}
