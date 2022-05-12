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
#define BUFFER_SIZE 255
#define MAX_SIZE_RECURRENT 100
#define MAX_SIZE_PIPE 10

int initSocketClient(char ServerIP[16], int Serverport);
void minuterie(void *delay);
void virementRec();

// Variable
int pipefd[2];

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
    if (argc != 5)
    {
        printf("Nombre d'arguments incorrect \n");
        exit(0);
    }
    printf("test 1 \n");
    // creation de la pipe
    int ret = spipe(pipefd);
    checkNeg(ret, "pipe error");

    StructMessage msg;
    Virement virement;

    // msg.messageText[ret - 1] = '\0';
    char *adr = argv[1];
    int port = atoi(argv[2]);
    int num = atoi(argv[3]);
    int delay = atoi(argv[4]);

    virement.num_destinataire = 4;
    virement.num_expediteur = 5;
    virement.montant = 45;

    int ppidMinute = fork_and_run1(minuterie, &delay);
    int ppidVirementRecurrent = fork_and_run0(virementRec);

    // ferme le pipe de lecture
    int retour = close(pipefd[0]);
    checkNeg(retour, "close error");
    char cmd[BUFFER_SIZE];

    while (true)
    {
        sread(0, cmd, BUFFER_SIZE);
        printf("%d", cmd[0]);
        if (cmd[0] == '+')
        {
        }
        else if (cmd[0] == '*')
        {
        }

        else if (cmd[0] == 'q')
        {
            skill(ppidVirementRecurrent, SIGQUIT);
            skill(ppidMinute, SIGQUIT);
            skill(getpid(), SIGQUIT);
            exit(0);
        }
    }

    int sockfd = initSocketClient(adr, port);
    swrite(sockfd, &msg, sizeof(msg));
    swrite(sockfd, &virement, sizeof(virement));

    printf("%d minuterie %d", num, delay);

    sclose(sockfd);
    return 0;
}

void minuterie(void *delay)
{
    printf("ICI 3 \n");

    /*Cast de void à int*/
    int *duration = delay;
    int durationInt = *duration;
    /*Fermeture de la pipe en lecture*/
    sclose(pipefd[0]);
    printf("ICI \n");

    while (true)
    {
        printf("ICI 2 \n");

        sleep(durationInt);
        char buffer[MAX_SIZE_PIPE];
        /*b pour battement*/
        buffer[0] = 'b';
        nwrite(pipefd[1], &buffer, MAX_SIZE_PIPE * sizeof(char));
    }
}

void virementRec()
{
    printf("Récurrent\n");
}
