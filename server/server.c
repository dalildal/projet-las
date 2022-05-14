#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../messages.h"
#include "../virement.h"
#include "../utils_v1.h"

#define BACKLOG 5
#define SEM_KEY 667
#define SHM_KEY 020
#define PERM 0666
#define SIZETAB 1000

void endServerHandler(int sig);
int initSocketServer(int port);

/* globals variables */
volatile sig_atomic_t end = 0;

void endServerHandler(int sig)
{
	end = 1;
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

	if (argc != 2)
    {
        printf("Nombre d'arguments incorrect \n");
        exit(0);
    }

	StructMessage msg;
	Virement virement;

	// convert char to int
	int port = strtol(argv[1], NULL, 10);
	int sockfd = initSocketServer(port);
	printf("Le serveur tourne sur le port : %d \n", port);

	// Accès Mémoire Partagé + Semaphore
	int shm_id = sshmget(SHM_KEY, SIZETAB * sizeof(int), 0);
	int *tab = sshmat(shm_id);
	// int sem_id = sem_get(SEM_KEY, 1);

	// Ctrl-C
	ssigaction(SIGINT, endServerHandler);

	while (!end)
	{

		/* client trt */
		int newsockfd = accept(sockfd, NULL, NULL);
		checkNeg(newsockfd, "ERROR accept");

		/* message du client */
		ssize_t retMsg = read(newsockfd, &msg, sizeof(msg));
		read(newsockfd, &virement, sizeof(virement));
		printf("%ld ", retMsg);
		printf("%d et %d \n", virement.num_destinataire, virement.num_expediteur);
		if (newsockfd > 0)
		{
			if (msg.code == VIREMENT)
			{
				if (virement.montant > 0)
				{
					tab[virement.num_destinataire] += virement.montant;
					tab[virement.num_expediteur] -= virement.montant;
					printf("OK : %d et %d \n", tab[virement.num_destinataire], tab[virement.num_expediteur]);
					msg.code = VIREMENT_OK;
				}
				else
				{
					msg.code = VIREMENT_KO;
				}
			}
			swrite(newsockfd, &msg, sizeof(msg));
		}
	}
}
