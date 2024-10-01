#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "shm.h"
#include <bits/time.h>

/* ****************************** Thread Begin ****************************** */
void set_table_enservice(struct table *t) {sem_post(&t->sem_service);}

void set_table_resa(struct table *t) {sem_post(&t->sem_resa);}

int table_enservice(struct table *t) {
	int val = -1;
	sem_getvalue(&t->sem_service, &val);
	return val;
}

int table_resa(struct table *t) {
	int val = -1;
	sem_getvalue(&t->sem_resa, &val);
	return val;
}

struct r_tab {
	struct restoo * r;
	int index_table;
	int duree_repas;
};

/**
 * @version 1.0
 * 
 * @brief       
 * 
 * @param[out]	[:r_ta] {void *}	the restaurant or a table
 *
 * @return {void *} 
 * 
 * @author chriSmile0
*/
void * exec_table_by_thread(void * r_ta) {
	pthread_exit(NULL);
}

/* ****************************** Thread End ******************************** */


int main(int argc, char *argv[]) {
	//Args//
	if(argc < 3) 
		raler("%s duree_repas [table1,tables2,...]", argv[0]);
	

	is_number(argv[1]);//
	is_number(argv[2]);//la fonction fait elle meme la sortie erreur

	int duree_repas = atoi(argv[1]);

	int min_s = 1;
	int max_s = 21600000;

	int nombr_table = 0;
	int capacite_totale_resto = 0;
	int l_tables[argc-2];

	int t = 2;
	int c_p = 0;
	while(((t < argc) && (((c_p = atoi(argv[t])) < 7)  
		&& ((c_p = atoi(argv[t])) > 0)))) {
		capacite_totale_resto += c_p;
		l_tables[t-2] = c_p;
		t++; 
	}
	
	if(t != (argc)) 
		raler("Un des arguments est plus grand que la capacite max ou plus petit que 1");

	nombr_table = t-2;

	if(t != (argc))
		raler("Un des arguments est plus grand que la capacite max ou plus petit que 1");

	if((duree_repas < min_s) || (duree_repas > max_s)) 
		raler("Temps du repas trop petit/grand");
	
	struct restoo * r =  open_resto(nombr_table,l_tables);
	struct cahier_rapel * c = open_cahier(nombr_table);

	struct r_tab *tab_rtab;
	tab_rtab = calloc(nombr_table, sizeof(struct r_tab));
	pthread_t *tid;
	tid = calloc(nombr_table, sizeof(pthread_t));
	for(int i = 0 ; i < nombr_table ; i++) {
		tab_rtab[i].r = r;
		int c = i;
		tab_rtab[i].index_table = c;
		tab_rtab[i].duree_repas = duree_repas;
		if((errno = pthread_create(&tid[i], NULL, exec_table_by_thread, &tab_rtab[i])) > 0) 
			raler("thread create");
			
	}
	for(int i = 0 ; i < nombr_table ; i++) 
		if(errno = pthread_join(tid[i], NULL) > 0) 
			raler("thread join"); 
	
	
	close_resto();
	close_cahier();
	free(tid);
	free(tab_rtab);
	return 0;
}