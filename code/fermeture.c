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
    sem_post(&r->S_fin);
    return 0;
}