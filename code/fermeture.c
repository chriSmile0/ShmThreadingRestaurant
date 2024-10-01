#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "shm.h"

int main(int argc, char *argv[]) {
	if(argc != 1)
		raler("%s", argv[0]);
	
	struct restoo * r = access_resto();
	printf("\n********FERMETURE**********\n");
	print_resto(r, stdout);
	sem_post(&r->sem_fin_service_resto);
	return 0;
}