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


/**
 * @brief Cherche le nombre de groupe du cahier
 * 
 * @param[:c] le cahier
 *
 * @return Le nombre de groupe du cahier
 * 
*/

int nb_groupe(struct cahier_rapel * c)//A modifier elle n'est pas optimale
{                               //Le cas où des tables se liberent et qu'elles sont réattribuer 
   int nb_groupe_max = c->nb_groupe;
   int j = 0;
   for (int i = 0 ; i < nb_groupe_max;i++) {
        if (c->groupes[i].num_gr != 0)
            j++;

   }
   return j;  
}

/**
 * @brief Cherche le nombre de convive au total
 *        Si un convive est absent on ne le contera pas
 *        
 * 
 * @param[:c] le cahier
 * @param[:nb_groupe] Le nombre de groupe
 *
 * @return Le nombre de convives au total
 * 
*/

int nb_convives(struct cahier_rapel * c,int nb_groupe)
{
    int nombre_convive = 0;
    for (int i = 0 ; i < nb_groupe ;i++) {
        int nb_membres_groupe = nb_membres_gr(c->groupes[i].membres_gr);
        int nombre_absent = nb_membres_absent(c->groupes[i].membres_gr);
        nombre_convive += (nb_membres_groupe - nombre_absent);
    }
    return nombre_convive;
}


int nb_convives_installer(struct table t)
{
    int nb_conv = nb_conv_t(t.convive);
    return nb_conv > 0 ? nb_conv : -1 ;
}


/* *************Debut thread ****************** */


int table_occuper(struct table *t)
{  
    int i = 0;
    if ((nb_convives_installer(*t)) == t->nb_convive_t)
        i++;
    
    return i;
}

struct r_tab {
    struct restoo * r;
    int index_table;
    int time_open;
};



/**
 * @brief Fonction utiliser pour qu'une table tourne séparément des autres
 *        tables.
 *        Cette fonction attend donc la fermeture indéfiniment 
 *        Si par chance un client passe la porte alors il va s'installer
 *        Puis une fois installer on le laisse manger pendant un certain temps.
 *        Puis on lui dit de partir , car son repas est terminé
 *        
 * 
 * @param[:void] On mettra en entrer un r_tab
 *
 * @return no return 
 * 
*/


void *exec_table_by_thread(void * r_ta)
{
    struct r_tab * re_ta = r_ta;
    int index_table = re_ta->index_table;
    int time_open = re_ta->time_open;
    struct table * t = &(re_ta->r->tables[index_table]);
    struct timespec clock;
    int t_s = -1;
    int occuper = -1;
    int v = -1;
    int close = -1;
    //int goon = 1;
    while (((sem_getvalue(&t->fin_table,&t_s) == 0) && (t_s != 1))
        && (close != 1)) {

        /*clock_gettime(CLOCK_REALTIME,&clock);
        if (time_open >= 1000) {
            clock.tv_sec+= time_open/1000;
            clock.tv_nsec+= (time_open % 1000)*1000000;
        }
        else {
            clock.tv_nsec+= (time_open * 1000000);
        }
       
        sem_timedwait(&t->fin_table,&clock);
        if(occuper > 0) {
            //sem_getvalue(&t->sem_ta,&v);
            sem_timedwait(&t->fin_table,&clock);
            //sleep(1);
            printf("begin sleep \n");
            //clock_nanosleep(CLOCK_MONOTONIC, 0, &clock, NULL);
            printf("after sleep \n");
            memset(t->convive,'\0',80);
            sem_post(&t->sem_ta);
            //sem_post(&t->fin_table);
            sem_getvalue(&t->sem_ta,&v);

            //sem_post(&t->fin_table);
            //printf("v : %d\n",v);
        }
        
        int S_fin_var = -1;
        int sem_fin = sem_getvalue(&re_ta->r->S_fin,&S_fin_var);
        int fermeture = -1;
        if((sem_fin == 0) && (S_fin_var == 1))
            fermeture = 1;
        
        occuper =  table_occuper(t);
    
        int conv_in_t = -1;
        sem_getvalue(&t->sem_time,&conv_in_t);
        //printf("fermeture : %d \n",fermeture);
        if(((occuper == 0) && (conv_in_t == 0)) && (fermeture == 1)) {
            close = 1;
        }
        else {
            if((occuper == 1) && (fermeture == 1)) {
                printf("occuper \n");
                print_table(t);
                close = 1; // NORMALEMENT NON 
            }
            else {
                if(occuper == 1)
                    print_table(t);
                if(fermeture == 1)
                    close = 1;
            }
        }*/
        clock_gettime(CLOCK_REALTIME,&clock);
        if (time_open >= 1000) {
            clock.tv_sec+= time_open/1000;
            clock.tv_nsec+= (time_open % 1000)*1000000;
        }
        else {
            clock.tv_nsec+= (time_open * 1000000);
        }
       
        sem_timedwait(&t->fin_table,&clock);

        if (occuper > 0) {
            sem_getvalue(&t->sem_ta,&v);
            memset(t->convive,'\0',80);
            sem_post(&t->sem_ta);
            sem_getvalue(&t->sem_ta,&v);
        }

        int S_fin_var = -1;
        int sem_fin = sem_getvalue(&re_ta->r->S_fin,&S_fin_var);
        int fermeture = -1;
        if ((sem_fin == 0) && (S_fin_var == 1))
          
            fermeture = 1;
        
        occuper =  table_occuper(t);
    
        int conv_in_t = -1;
        sem_getvalue(&t->sem_time,&conv_in_t);
        if (((occuper == 0) && (conv_in_t == 0)) && (fermeture == 1)) 
            close = 1;
        
    }
    pthread_exit(NULL);
}

/* *************Fin thread ******************* */


int main(int argc,char *argv[])
{
    //Args//
    if (argc < 3) {
        fprintf(stderr,"usage: %s temps_d'ouverture liste_de_tables \n",argv[0]);
        exit(EXIT_FAILURE);
    }

    is_number(argv[1]);//
    is_number(argv[2]);//la fonction fait elle meme la sortie erreur

    int time_open = atoi(argv[1]);

    int min_s = 1;
    int max_s = 21600000;

    int nombr_table = 0;
    int capacite_totale_resto = 0;
    int l_tables[argc-2];
  
    int t = 2;
    int c_p = 0;
    while ((( t < argc ) && (((c_p = atoi(argv[t])) < 7)  
        && ((c_p = atoi(argv[t])) > 0)))) {
        capacite_totale_resto+=c_p;
        l_tables[t-2] = c_p;
        t++; 
    }
    
    if (t != (argc)) {
        fprintf(stderr,
        "usage: Un des arguments est plus grand que la capacite max ou plus petit que 1\n");
        exit(EXIT_FAILURE);
    }
    nombr_table = t-2;

    if (t != (argc)) {
        fprintf(stderr,
        "usage: Un des arguments est plus grand que la capacite max ou plus petit que 1\n");
        exit(EXIT_FAILURE);
    }

    if ((time_open < min_s) || (time_open > max_s)) {
        fprintf(stderr,"usage: Temps d'ouverture trop petit/grand \n");
        exit(EXIT_FAILURE);
    }
    struct restoo * r =  open_resto(nombr_table,l_tables);
    struct cahier_rapel * c = open_cahier(nombr_table);

    struct r_tab *tab_rtab;
    tab_rtab = calloc(nombr_table , sizeof(struct r_tab));
   
    pthread_t *tid;
    tid = calloc(nombr_table ,sizeof(pthread_t));
    for (int i = 0 ; i < nombr_table;i++) {
        tab_rtab[i].r = r;
        int c = i;
        tab_rtab[i].index_table = c;
        tab_rtab[i].time_open = time_open;
        if ((errno = pthread_create(&tid[i],NULL,exec_table_by_thread,&tab_rtab[i])) > 0) {
            fprintf(stderr,"thread create\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0 ; i < nombr_table;i++) {
        if (errno = pthread_join(tid[i],NULL) > 0) {
            fprintf(stderr,"thread join\n");
            exit(EXIT_FAILURE);
        }
    }

    int nb_c = nb_convives(c,c->nb_groupe);
    int nb_g = nb_groupe(c);
    if (nb_c == 0)
        nb_g = 0;

    //print_resto(r);
    //print_cahier(c);
    close_resto();
    close_cahier();
    printf("%d convives servis dans %d groupes\n",nb_c,nb_g);
    free(tid);
    free(tab_rtab);
    return 0;
}