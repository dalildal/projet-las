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
void virementSimple(char *cmd);

// Variable
int pipefd[2];
char *adr;
int port;
int num;
int delay;

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
    printf("**** PROGRAMME DE VIREMENT BANQUAIRE **** \n");
    printf("\n");
    printf("+ -> Virement Simple  \n");
    printf("* -> Virement Récurrent  \n");
    printf("q -> Quitter  \n");
    printf("\n");

    // creation de la pipe
    int ret = spipe(pipefd);
    checkNeg(ret, "pipe error");

    // StructMessage msg;
    // Virement virement;

    // msg.messageText[ret - 1] = '\0';
    adr = argv[1];
    port = atoi(argv[2]);
    num = atoi(argv[3]);
    delay = atoi(argv[4]);

    int ppidMinute = fork_and_run1(minuterie, &delay);
    int ppidVirementRecurrent = fork_and_run0(virementRec);

    // ferme le pipe de lecture
    int retour = close(pipefd[0]);
    checkNeg(retour, "close error");
    char cmd[BUFFER_SIZE];

    while (true)
    {
        sread(0, cmd, BUFFER_SIZE);

        if (cmd[0] == '+')
        {
            char *chaine = "J'envoie un virement au serveur\n";
            size_t sz = strlen(chaine);
            nwrite(0, chaine, sz);
            virementSimple(cmd);
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

    printf("%d minuterie %d", num, delay);

    return 0;
}

void minuterie(void *delay)
{

    /*Cast de void à int*/
    int *duration = delay;
    int durationInt = *duration;
    /*Fermeture de la pipe en lecture*/
    sclose(pipefd[0]);

    while (true)
    {

        sleep(durationInt);
        char buffer[MAX_SIZE_PIPE];
        /*b pour battement*/
        buffer[0] = 'b';
        nwrite(pipefd[1], &buffer, MAX_SIZE_PIPE * sizeof(char));
    }
}

void virementSimple(char *cmd)
{
    cmd[strlen(cmd) - 1] = '\0';
    int sockfd = initSocketClient(adr, port);

    char *signe;
    if ((signe = strtok(cmd, "\t \r")) == NULL)
    {
        fprintf(stderr, "???\n");
        return;
    }
    char *montant;
    if ((montant = strtok(NULL, " ")) == NULL)
    {
        fprintf(stderr, "???\n");
        return;
    }
    char *montant;
    if ((montant = strtok(NULL, " ")) == NULL)
    {
        fprintf(stderr, "???\n");
        return;
    }

    printf("%s\n", montant);
    sclose(sockfd);
}

void virementRec()
{
    // printf("Récurrent\n");
}
