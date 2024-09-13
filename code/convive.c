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

struct strings {
    char *tab[10];
    int nb_string;
};

void copie_tab(struct table source[],struct table dest[],int same_taille)
{
    for(int i = 0 ; i < same_taille;i++) 
        dest[i] = source[i];
    
}

/**
 * @brief Search if a token is present on a string
 * @param[:string] the string to analyze
 * @param[:token] the token search in string
 * @return >0 or 0 
*/
int is_present(char * string, char *token) {
    int i = 0;
    char c;
    char first_letter_token = token[0];
    size_t len_token = strlen(token);
    int cpt = 0;
    while((c = *(string+i)) != '\0') {
        if(c == first_letter_token) {
            int j = i + len_token;
            int k = i;
            int h = 0;
            while((k < j) && (string[k]==token[h])) {
                k++;
                h++;
            }
            if(k==j) 
                cpt++;
                 
        }
        i++;
    }
    return cpt;
}

void ready_to_lunch(struct group *g, struct table *t) {
    g->g_complet = 1;
    sem_post(&t->sem_ta);
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
    int tim = -1;
    int last_fit = -1;
    while((sem_getvalue(&tab_tables[i].sem_time,&tim) == 0) && (tim != 1)
        && (i < taille_tab_t)) {
            //SEARCH PERFECT TABLE
        int capa_table = tab_tables[i].capacite;
        if(capa_table == nb_place) 
            return i;

        if(capa_table > nb_place) {
            if(last_fit != -1) { 
                if(capa_table < tab_tables[last_fit].capacite)
                    last_fit = i;
            
            }
            else {
                last_fit = i;
            }
        }
        i++;
    }
    return last_fit;
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

    if(i == 0) {
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
    int nb_t =  r->nb_tables;
    int t = 0;
    while((!is_present(r->tables[t].convive,convive_f)) && (t < nb_t)) 
        t++;
    if(t == nb_t)
        return -1;
    return t;
}

/**
 * @param[:original_string] char *string = "HELLO" dont work -> str[]="blabla",str[100]="blabla" works
 * @param[:pos] the position of the next first character 
*/

void advance_string(char *original_string, int pos) {
    char *ptr = original_string;
    ptr+=pos;
    memcpy(original_string,ptr,strlen(ptr)+pos);
}

char * substr(char *string, int dept, int end) {
    int size = end-dept+1;
    char * dest = (char*)malloc(sizeof(char)*size);
    int i = dept; 
    char c;
    while((i < end) && ((c=*(string+i)) != '\0')) {
        *dest = c;
        dest++;
        i++;
    }
    *dest = '\0';
    return dest-(size-1);
}

char * strchr_(char * string, char search, int advance_src) {
    int i = 0;
    char c;
    while((c = *(string+i)) != '\0' && (c != search)) 
        i++;
    if(c == search) {
        char * rt = substr(string,0,i);
        if(advance_src) 
            advance_string(string,i+1);
        return rt;
    }
    else {
        return string;
    } 
}

char * strtok_(char * string, char *token, int advance_src) {
    int i = 0;
    char c;
    char first_letter_token = token[0];
    size_t len_token = strlen(token);
    while((c = *(string+i)) != '\0') {
        if(c == first_letter_token) {
            int j = i + len_token;
            int k = i;
            int h = 0;
            while((k < j) && (string[k]==token[h])) {
                k++;
                h++;
            }
            if(k==j) {
                char * rt = substr(string,0,i);
                if(advance_src) 
                    advance_string(string,j);
                return rt;
            }  
        }
        i++;
    }
    return string; 
}

/**
 * @brief Cut string in tab of string
 * @param[:convive] Initialize string
 * @param[:separator] character separator
 * @return struct strings contains all strings we separate by separator but without
 *          the separator character
*/
struct strings line_in_strings(char string[], char separator) {
    struct strings s;
    int i = 0; 
    while((i < 6)) {
        char * rt = strchr_(string,separator,1);
        s.tab[i] = rt;
        i++;
        if(strcmp(string,rt)==0)
            break;
    }
    s.nb_string = i;
    return s;
}


int main(int argc,char *argv[])
{
    (void) argc;
    (void) argv;
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
        printf("nb_place resa : %d\n",nb_place_resa);
        snprintf(resa_conv,len_1+1,"%s",argv[1]);
        int placer = inserer_convive_first(r,resa_conv,nb_place_resa);
        printf("placer : %d\n",placer);
        if (placer == -1) {
            printf("Desole %s pas de table disponible \n",argv[1]);
        }
        else {
            printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer);
            r->nb_tables_occuper++;
            r->tables[placer].num = placer;
            int index = creer_groupe(c,placer);
        
            int index_g = concat_chaine_in_membres(c,resa_conv,index);
            (void) index_g;
            //if(nb_place_resa == 1) {
                //sem_post(&r->tables[placer].fin_table);
                //sem_post(&r->tables[placer].sem_ta);
                sem_post(&r->tables[placer].sem_time);
                c->groupes[index].nb_membres_gr = nb_place_resa;
                if(nb_place_resa == 1) {
                    //sem_post(&r->tables[placer].sem_ta);
                    ready_to_lunch(&c->groupes[index],&r->tables[placer]);
                }
                printf("index_g : %d\n",index);
                printf("c groupes resa : %d\n",c->groupes[index].nb_membres_gr);
                sem_wait(&r->tables[placer].fin_table);
                for(int i = 0; i < nb_place_resa-1;i++) 
                    sem_post(&r->tables[placer].sem_stand);
                
                //sem_post(&r->tables[placer].fin_table);
                printf("levé résa \n");
                sem_wait(&r->tables[placer].sem_time);
                //r->nb_tables_occuper--;
            //}
            
            //sem_post(&r->tables[placer].sem_time);
           
            //sem_post(&r->tables[placer].sem_ta);
            //TRAITEMENT 


            // FIN ICI 
            //sem_wait(&r->tables[placer].sem_time);
            r->nb_tables_occuper--;
            
        }
    }
    else {
        char resa_conv[10];
        snprintf(conv_invit,len_1+1,"%s",argv[1]);
        snprintf(resa_conv,strlen(argv[2])+1,"%s",argv[2]);
        int placer_invit = chercher_first_c(resa_conv,r);
                        
        if (placer_invit == -1) 
            printf("Desolé %s pas de %s ici \n",argv[1],argv[2]);
        else {
            printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer_invit);
            int x = inserer_in_groupe(conv_invit,c,resa_conv,c->nb_groupe);
            printf("x : %d\n",x);
            if(x == -1) {
                printf("Table pleine \n");
                return 0;
            }
            int nb_membres_groupe = nb_membres_gr(c->groupes[x].membres_gr);
            printf("nb_membres : %d\n",nb_membres_groupe);
            if(nb_membres_groupe == c->groupes[x].nb_membres_gr) {
                //sem_post(&r->tables[placer_invit].sem_ta);
                ready_to_lunch(&c->groupes[x],&r->tables[placer_invit]);
                //sem_wait(&r->tables[placer_invit].fin_table);
                sem_wait(&r->tables[placer_invit].sem_stand);
                //sem_post(&r->tables[placer_invit].fin_table);*/
                printf("ici \n");
            }



            /*sem_wait(&r->tables[placer_invit].sem_ta);
            sem_wait(&r->tables[placer_invit].sem_time);*/
        }
    }
    /*char string[80] = "COUCOU JEAN ICI JOE LE TAXI";
    //printf("%s\n",strtok_(string,"JOE",0));
    char param[10] = "JEAN";
    printf("%s is in %s ? -> %s\n",param,string,(is_present(string,param) ? "YES" : "NO"));*/
    return 0;
}