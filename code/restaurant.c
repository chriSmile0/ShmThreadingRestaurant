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

int nb_groupe(struct cahier_rapel * c) {return c->nb_groupe;}

int nb_convives(struct cahier_rapel * c, int nb_groupe)
{
    int nombre_convive = 0;
    for(int i = 0 ; i < nb_groupe ;i++) 
        nombre_convive += c->groupes[i].membres_present;
    
    return nombre_convive;
}

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
	int lunch_time;
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
	struct r_tab * re_ta = r_ta;
    int index_table = re_ta->index_table;
    int lunch_time = re_ta->lunch_time;
    struct table * t = &(re_ta->r->tables[index_table]);
	print_table(t, stdout, 0);
	(void) lunch_time;
	int open = 1;
	int open_is_programing = 0;
	while(open) {
		int S_resa_var = -1;
		int sem_resa = sem_getvalue(&t->sem_resa,&S_resa_var);
		if((!sem_resa) && (S_resa_var)) {
			int nb_convives_waited = t->nb_convive_t;
			int nb_conv_t_var = nb_conv_t(t->convive);
			if((nb_convives_waited == nb_conv_t_var) || (open_is_programing)) {
				printf("***all convives are here***\n");
				sem_post(&t->sem_service);
				usleep(lunch_time*1000);
				/*int S_fin_repas_convive = -1;
				int sem_repas_convive = sem_getvalue(&t->sem_fin_repas_convive,&S_fin_repas_convive);*/
				for(int i = 0; i < nb_convives_waited ; i++) 
					sem_post(&t->chairs[i]);
				// SEM ON EACH SEAT
				sem_wait(&t->sem_service);
				reset_table(t);
				if(open_is_programing)
					open = !open;
			}
		}
		int S_end_var = -1;
        int sem_end = sem_getvalue(&re_ta->r->sem_fin_service_resto,&S_end_var);
		if((!sem_end) && (S_end_var == 1)) {
			open = !open;
			if(S_resa_var == 1) {
				open_is_programing = 1;
				open = !open;
			}
		}
	}
	pthread_exit(NULL);
}

/* ****************************** Thread End ******************************** */


int main(int argc, char *argv[]) {
	//Args//
	if(argc < 3) 
		raler("%s lunch_time [table1,tables2,...]", argv[0]);
	

	is_number(argv[1]);//
	is_number(argv[2]);//la fonction fait elle meme la sortie erreur

	int lunch_time = atoi(argv[1]);

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

	if((lunch_time < min_s) || (lunch_time > max_s)) 
		raler("Temps du repas trop petit/grand");
	
	struct restoo * r =  open_resto(nombr_table,l_tables);
	struct cahier_rapel * c = open_cahier();

	struct r_tab *tab_rtab;
	tab_rtab = calloc(nombr_table, sizeof(struct r_tab));
	pthread_t *tid;
	tid = calloc(nombr_table, sizeof(pthread_t));
	for(int i = 0 ; i < nombr_table ; i++) {
		tab_rtab[i].r = r;
		int c = i;
		tab_rtab[i].index_table = c;
		tab_rtab[i].lunch_time = lunch_time;
		if((errno = pthread_create(&tid[i], NULL, exec_table_by_thread, &tab_rtab[i])) > 0) 
			raler("thread create");
			
	}
	for(int i = 0 ; i < nombr_table ; i++) 
		if(errno = pthread_join(tid[i], NULL) > 0) 
			raler("thread join"); 
	
	print_resto(r, stdout, 0);
	print_cahier(c, stdout, 0);
	int nb_g = nb_groupe(c);
	int nb_c = nb_convives(c,nb_g);
	close_resto();
	close_cahier();
	printf("%d convives servis dans %d groupes\n",nb_c,nb_g);
	free(tid);
	free(tab_rtab);
	return 0;
}