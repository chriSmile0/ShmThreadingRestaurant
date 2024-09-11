#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "shm.h"


/**
 * @brief Copie d'une table dans une autre
 *        
 * 
 * @param[:source] la table source
 * @param[:dest] la table destination
 * @param[:same_taille] Leur taille
 *
 * @return no return
 * 
*/

void copie_tab(struct table source[],struct table dest[],int same_taille)
{
    for(int i = 0 ; i < same_taille;i++) 
        dest[i] = source[i];
    
}

/**
 * @brief Cherche la table la mieux adapté pour le convive entrant
 *        
 * 
 * @param[:nb_place] Le nombre que le premier convive veut
 * @param[:tab_tables] Le tableau de tables 
 * @param[:taille_tab_t] La taille du tableau de table
 *
 * @return l'indice dans la plus petite table possible
 * 
*/

int plus_petite_table_possible(int nb_place,struct table tab_tables[],
                                int taille_tab_t)
{
    int i = 0;
    int good_size = nb_place;
    int j = 0;
    int tmp_max=0;

    struct table tab_a_trier[taille_tab_t]; 
    copie_tab(tab_tables,tab_a_trier,taille_tab_t);

    for (i = taille_tab_t-2;i >= 0;i--) {
        for ( j = 0;j <= i;j++) {
            if (tab_a_trier[j].capacite >= tab_a_trier[j+1].capacite) {
                tmp_max = tab_a_trier[j+1].capacite;
                tab_a_trier[j+1].capacite = tab_a_trier[j].capacite;
                tab_a_trier[j].capacite= tmp_max;
            }
        }
    }
    
    i = taille_tab_t -1;
    int tim = -1;
    while ((tab_a_trier[i].capacite > good_size) && ( i > 0) 
            && ((sem_getvalue(&tab_a_trier[i].sem_time,&tim) == 0) 
            && (tim != 0))) {
        i--;
    }

    if (tab_a_trier[i].capacite < good_size)
        return -1;

    return tab_a_trier[i].num;
}

/**
 * @brief Inserer le premier convive dans la chaine convive de la table
 *        qui lui correspond le mieux 
 * 
 * @param[:r] le restoo 
 * @param[:nb_place] Le nombre de placer chercher
 * @param[:conv] Le convive a inserer
 *
 * @return La table d'insertion
 * 
*/

int inserer_convive_first(struct restoo * r,char conv[],int nb_place)
{
    int i = 0;
    char * copie_f = malloc(100*sizeof(char));
    memset(copie_f,'\0',100);
    strncpy(copie_f,conv,strlen(conv));
    char * copie = malloc(80*sizeof(char));
    memset(copie,'\0',80);

    int nb_tables_max = r->nb_tables;

    int chercher_table_libre = plus_petite_table_possible(nb_place,r->tables,nb_tables_max);


    int l_b = chercher_table_libre;
    strncpy(copie,r->tables[l_b].convive,strlen(r->tables[l_b].convive));
    while (copie[i] != '\0')
        i++;

    if( i == 0) {
        strncpy(r->tables[l_b].convive,copie_f,strlen(copie_f));
        r->tables[l_b].convive[strlen(copie_f)] = '\0';
        r->tables[l_b].nb_convive_t = nb_place;
    }
    
    free(copie_f);
    return l_b;
   
}

/**
 * @brief Cherche le nom du premier convive 
 *        
 * 
 * @param[:r] le restoo
 * @param[:convive_f] Le convive first que l'on cherche
 *
 * @return L'indice de la table où se trouve convive_f
 * 
*/

int chercher_first_c(char convive_f[],struct restoo * r)
{
    int i = 0;
    char chaine[10];
    int j = 0;
    int indice_fin_first = -1;
    int nb_t =  r->nb_tables;
    int taille_conv = 0;
    int t = 0;
    while( (indice_fin_first != -1) && (t < nb_t)) {
        taille_conv = strlen(r->tables[t].convive);
        i = 0;
        while ( i < taille_conv) {
            if (isalpha(r->tables[t].convive[i]) == 1024) {
                j = i;
                while ((r->tables[t].convive[j] != ' ') && (j < taille_conv)) 
                    j++;

                memset(chaine,'\0',10);
                concat_maison(chaine,r->tables[t].convive,i,j);
                if (strncmp(chaine,convive_f,strlen(convive_f)) == 0) 
                    indice_fin_first = j;
                
            }
            i+= i + j;
        }
        t++;
    }
    return t;
}


int main(int argc,char *argv[])
{
    //ARGS
    if (argc != 3) {
        fprintf(stderr,
        "usage: %s Nom_du_premier_convive nb_place_resa ou Nom_convive Premier_convive\n"
        ,argv[0]);
        exit(EXIT_FAILURE);
    }

    long int len_1 = strlen(argv[1]);
    long int len_2 = strlen(argv[2]);
    if ((len_1 > 10) || (len_2 > 10) || (len_1 == 0 ) || (len_2 == 0)) {
        fprintf(stderr,
        "usage: %s %s %s , pas plus grand que 10 caracter  es et non vide\n",
        argv[0],argv[1],argv[2]);
        exit(EXIT_FAILURE);
    }

    char conv_invit[10];
    char resa_conv[10];
    int arg2_nombre = 0;
    if (isdigit(argv[2][0]) == 2048) 
        if (is_number(argv[2]))
            arg2_nombre = 1;


    printf("arg2_nombre : %d\n",arg2_nombre);
    
    struct restoo * r = access_resto();
    struct cahier_rapel * c = access_cahier();
   
    if (arg2_nombre == 1) {
        int nb_place_resa = atoi(argv[2]);
        snprintf(resa_conv,len_1+1,"%s",argv[1]);
        int placer = inserer_convive_first(r,resa_conv,nb_place_resa);
        if (placer == -1) {
            printf("Desole %s pas de table disponible \n",argv[1]);
        }
        else {
            printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer);
            r->nb_tables_occuper++;
            r->tables[placer].num = placer;
            int index = creer_groupe(c);
        
            int index_g = concat_chaine_in_membres(c,resa_conv,index);
            if (nb_place_resa == 1) 
                sem_post(&r->tables[placer].fin_table);
            
            sem_post(&r->tables[placer].sem_time);
            c->groupes[index_g].nb_membres_gr = nb_place_resa;
            sem_wait(&r->tables[placer].sem_ta);
            sem_wait(&r->tables[placer].sem_time);
            r->nb_tables_occuper--;
        }
    }
    else {
        printf("là \n");
        char resa_conv[10];
        snprintf(conv_invit,len_1+1,"%s",argv[1]);
        snprintf(resa_conv,strlen(argv[2])+1,"%s",argv[2]);
        int placer_invit = chercher_first_c(conv_invit,r);
                        
        if (placer_invit == -1) 
            printf("Desolé %s pas de %s ici \n",argv[1],argv[2]);
        else {
            printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer_invit);
            int x = inserer_in_groupe(conv_invit,c,resa_conv,c->nb_groupe);
            printf("x : %d\n",x);
            int nb_membres_groupe = nb_membres_gr(c->groupes[x].membres_gr);
            printf("coucou");
            if ((nb_membres_groupe + 1) == c->groupes[x].nb_membres_gr) {
                sem_post(&r->tables[placer_invit].fin_table);
                printf("ici \n");
            }



            sem_wait(&r->tables[placer_invit].sem_ta);
            sem_wait(&r->tables[placer_invit].sem_time);
        }
    }
   
    return 0;
}