#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "shm.h"

int main(int argc,char *argv[])
{
    //ARGS
    if (argc != 1) {//0 argument en plus du prog
        fprintf(stderr,"usage: %s\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    struct restoo * r = access_resto();

    // 190 -> On ne ferme pas si une table est encore occupé
    printf("\n********FERMETURE**********\n");
    print_resto(r);
    // Attente d'un convive ou repas encore en cours 
    // On lance tous les repas 
    // "" = 210 extension 

    sem_post(&r->sem_fin_service_resto);
    return 0;
}