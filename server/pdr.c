#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "../utils_v1.h"
#include "../virement.h"

#define SEM_KEY 667
#define SHM_KEY 020
#define PERM 0666
#define SIZETAB 1000

int main(int argc, char const *argv[])
{
    int num_compte = strtol(argv[1], NULL, 10);;
    int montant = strtol(argv[2], NULL, 10);;

    int shm_id = sshmget(SHM_KEY, SIZETAB * sizeof(int), 0);
    int *tab = sshmat(shm_id);
    int sem_id = sem_get(SEM_KEY, 1);

    for (int i = 0; i < 10; i++)
    {
       printf("%i %d\n", i, tab[i]);
    }

    if( montant > 0) {
        sem_down0(sem_id);
        tab[num_compte] += montant;
        printf("--> %d\n",tab[num_compte]);
        sem_up0(sem_id);
    } else{
        sem_down0(sem_id);
        tab[num_compte] += montant;
        printf("--> %d\n",tab[num_compte]);
        sem_up0(sem_id);
    }

    for (int i = 0; i < 10; i++)
    {
       printf("%i %d\n", i, tab[i]);
    }
    
    sshmdt(tab);

    exit(EXIT_SUCCESS);
}
