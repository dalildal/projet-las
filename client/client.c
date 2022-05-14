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
Virement tronquerChaine(char *cmd);
void addVirementRecurrent(char *cmd);
void execVirementRec(Virement vir);

// Variable
int pipefd[2];
char *adr;
int port;
int num_exp;
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

    // Assignation des args a nos variables
    adr = argv[1];
    port = atoi(argv[2]);
    num_exp = atoi(argv[3]);
    delay = atoi(argv[4]);

    // Création des fils
    int ppidMinute = fork_and_run1(minuterie, &delay);
    int ppidVirementRecurrent = fork_and_run0(virementRec);

    // ferme le pipe de lecture
    int retour = close(pipefd[0]);
    checkNeg(retour, "close error");
    char cmd[BUFFER_SIZE];

    /** TERMINAL **/
    while (true)
    {
        sread(0, cmd, BUFFER_SIZE);

        if (cmd[0] == '+')
        {
            char *chaine = "Envoie d'un virement simple \n";
            size_t sz = strlen(chaine);
            nwrite(0, chaine, sz);
            virementSimple(cmd);
        }
        else if (cmd[0] == '*')
        {
            char *chaine = "Envoie d'un virement récurrent \n";
            size_t sz = strlen(chaine);
            nwrite(0, chaine, sz);
            addVirementRecurrent(cmd);
        }

        else if (cmd[0] == 'q')
        {
            skill(ppidVirementRecurrent, SIGQUIT);
            skill(ppidMinute, SIGQUIT);
            skill(getpid(), SIGQUIT);
            exit(0);
        }
    }

    printf("%d minuterie %d", num_exp, delay);

    return 0;
}

// Premier fils
void minuterie(void *delay)
{
    int *duration = delay;
    int durationInt = *duration;

    /*Fermeture de la pipe en lecture*/
    sclose(pipefd[0]);

    while (true)
    {
        sleep(durationInt);
        int buffer[MAX_SIZE_PIPE];
        // 1 pour un battement
        buffer[0] = 1;
        nwrite(pipefd[1], &buffer, MAX_SIZE_PIPE * sizeof(int));
    }
}

// Tronque la commande en parametre et ajoute un virement à la pipe
void addVirementRecurrent(char *cmd)
{
    Virement virement;
    virement = tronquerChaine(cmd);
    int montant = virement.montant;

    int buffer[MAX_SIZE_PIPE];
    buffer[0] = 0;
    buffer[2] = virement.num_destinataire;
    buffer[3] = virement.num_expediteur;
    buffer[4] = montant;

    nwrite(pipefd[1], &buffer, MAX_SIZE_PIPE * sizeof(int));
}

// Tronque la commande en parametre et envoie le virement au serveur
void virementSimple(char *cmd)
{
    Virement virement;
    StructMessage msg;
    msg.code = VIREMENT;

    cmd[strlen(cmd) - 1] = '\0';
    int sockfd = initSocketClient(adr, port);

    virement = tronquerChaine(cmd);

    swrite(sockfd, &msg, sizeof(msg));
    swrite(sockfd, &virement, sizeof(virement));

    /* wait server response */
    sread(sockfd, &msg, sizeof(msg));

    if (msg.code == VIREMENT_OK)
    {
        printf("Réponse du serveur : Virement effectué\n");
    }
    else
    {
        printf("Réponse du serveur : Virement à échoué\n");
    }
    sclose(sockfd);
}

// Deuxieme fils
void virementRec()
{
    // Fermeture du pipe en écriture
    sclose(pipefd[1]);
    Virement tab[MAX_SIZE_RECURRENT];

    int tailleL = 0;
    int nbRecurrence = 0;
    while (nbRecurrence <= MAX_SIZE_RECURRENT)
    {
        int buffer[BUFFER_SIZE];
        sread(pipefd[0], buffer, BUFFER_SIZE * sizeof(int));
        // Si on recoit un 0 de la pipe
        if (buffer[0] == 0)
        {
            Virement vir;
            vir.montant = buffer[4];
            vir.num_expediteur = buffer[3];
            vir.num_destinataire = buffer[2];
            tab[tailleL] = vir;
            tailleL++;
            nbRecurrence++;
        }
        else
        {
            if (nbRecurrence > 0)
            {
                // On a recu un battement
                for (int i = 0; i < tailleL; i++)
                {
                    execVirementRec(tab[i]);
                }
                nbRecurrence++;
            }
        }
    }
}
// Envoie un par un les viremetns récurrent au serveur
void execVirementRec(Virement vir)
{
    StructMessage msg;

    int sockfd = initSocketClient(adr, port);

    msg.code = VIREMENT;
    swrite(sockfd, &msg, sizeof(msg));
    swrite(sockfd, &vir, sizeof(vir));

    // wait server response
    sread(sockfd, &msg, sizeof(msg));

    if (msg.code == VIREMENT_OK)
    {
        printf("Réponse du serveur : Virement effectué\n");
    }
    else
    {
        printf("Réponse du serveur : Virement à échoué\n");
    }
    sclose(sockfd);
}

// Permet de tronquer la chaine et la convertir en Virement
Virement tronquerChaine(char *cmd)
{
    // On tronque la chaine
    char *signe;
    if ((signe = strtok(cmd, "\t \r")) == NULL)
    {
        fprintf(stderr, "???\n");
    }
    char *n2;
    if ((n2 = strtok(NULL, " ")) == NULL)
    {
        fprintf(stderr, "???\n");
    }
    char *montant;
    if ((montant = strtok(NULL, " ")) == NULL)
    {
        fprintf(stderr, "???\n");
    }

    Virement virement;
    virement.montant = atoi(montant);
    virement.num_destinataire = atoi(n2);
    virement.num_expediteur = num_exp;

    return virement;
}
