#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "../utils_v1.h"
#include "../virement.h"

#define SEM_KEY 667
#define KEY 020
#define PERM 0666
#define SIZETAB 1000

int main(int argc, char const *argv[])
{
    int type = argv[1];

    struct Payment* pay;
    if( type == 1 ) {
        int sizeStruct = sizeof(pay); 

        //SEMAPHORE
        int sem_id = sem_create(SEM_KEY, 1, PERM, 1);

        //SHARED MEMORY
      int shm_id = sshmget(KEY,(sizeof(pay)*SIZETAB), IPC_CREAT | IPC_EXCL | PERM);
      int* z = sshmat(shm_id);
      printf("Shared Memory created ssh_id : %d at adress %d: \n",shm_id,*z);

      EXIT_SUCCESS;
    }

    if (type == 2)
    {
        int shm_id = sshmget(KEY, 0, 0);
        sshmdelete(shm_id);
        printf("Mémoire partagé détruite à : %d\n", shm_id);

        int sem_id = sem_get(SEM_KEY, 1);
        sem_delete(sem_id);
        printf("Semaphore avec l'adress : %d est détruite \n", sem_id);
    }

    // TYPE = 3 -> On réserve de façon exclusive le livre de comptes partagé
    if (type == 3)
    {
        long int tempsRes = strtol(argv[2], NULL, 10);
        int sem_id = sem_get(SEM_KEY, 1);
        sem_down0(sem_id);
        sleep(tempsRes);
        sem_up0(sem_id);
        printf("Fin reservation\n");
    }
}


