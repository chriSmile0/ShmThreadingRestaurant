#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "shm.h"

int main(int argc,char *argv[])
{
    if (argc != 1) {//0 argument en plus du prog
        fprintf(stderr,"usage: %s\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    affichage_police(stdout);
    return 0;
}