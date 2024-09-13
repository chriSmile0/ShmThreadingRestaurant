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
    /*int j = 0;
    int tmp_max=0;*/

    /*struct table tab_a_trier[taille_tab_t]; 
    copie_tab(tab_tables,tab_a_trier,taille_tab_t);

    for (i = taille_tab_t-2;i >= 0;i--) {
        for ( j = 0;j <= i;j++) {
            if (tab_a_trier[j].capacite >= tab_a_trier[j+1].capacite) {
                tmp_max = tab_a_trier[j+1].capacite;
                tab_a_trier[j+1].capacite = tab_a_trier[j].capacite;
                tab_a_trier[j].capacite = tmp_max;
            }
        }
    }
    
    i = taille_tab_t -1;
    int tim = -1;
    while ((tab_a_trier[i].capacite > good_size) && ( i > 0) 
            && ((sem_getvalue(&tab_a_trier[i].sem_time,&tim) == 0) 
            && (tim != 0))) {
        i--;
    }*/
    int tim = -1;
    //int found_perfect = -1;
    int last_fit = -1;
    //int last_fit_nb_place = 0;
    while((sem_getvalue(&tab_tables[i].sem_ta,&tim) == 0) && (tim != 1)
        && (i < taille_tab_t)) {
            //SEARCH PERFECT TABLE
        int capa_table = tab_tables[i].capacite;
        printf("in loop \n");
        if(capa_table == nb_place) 
            return i;

        if(capa_table > nb_place) {
            printf("last fit %d\n",last_fit);
            if(last_fit != -1) { 
                printf("here \n");
                if(capa_table < tab_tables[last_fit].capacite)
                    last_fit = i;
            
            }
            else {
                last_fit = i;
            }
        }
        i++;
    }
    printf("tim : %d\n",tim);
    if(last_fit != -1)
        return last_fit;
    if(tab_tables[i].capacite < good_size)
        return -1;
    return i;
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

    printf("chercher table libre : %d\n",chercher_table_libre);
    int l_b = chercher_table_libre;
    strncpy(copie,r->tables[l_b].convive,strlen(r->tables[l_b].convive));
    while (copie[i] != '\0')
        i++;
    printf("i : %d\n",i);
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

/**
 * @param[:original_string] char *string = "HELLO" dont work -> str[]="blabla",str[100]="blabla" works
 * @param[:pos] the position of the next first character 
*/

void advance_string(char *original_string, int pos) {
    /*size_t size = strlen(original_string);
    char *string_malloc = (char*)(malloc(sizeof(char)*size+1));
    snprintf(string_malloc,size+1,"%s",original_string);
    char *ptr = string_malloc;
    ptr+=2;
    //strcat(ptr,"III");
    printf("ptr advance : %s\n",ptr);*/
    //printf("str advance : %s\n",string_malloc);
    //original_string = ptr;
    char *ptr = original_string;
    ptr+=pos;
    memcpy(original_string,ptr,strlen(ptr)+pos);
    //memset(original_string,'\0',10);
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

char * editString(char * str) {
    strcat(str,"LOL");
    return NULL;
}

/**
 * @brief Cut string in tab of string
 * @param[:convive] Initialize string
 * @param[:separator] character separator
 * @return struct strings contains all strings we separate by separator but without
 *          the separator character
*/
struct strings line_in_strings(char convive[], char separator) {
    struct strings s;
    int i = 0; 
    while((i < 6)) {
        char * rt = strchr_(convive,separator,1);
        s.tab[i] = rt;
        i++;
        if(strcmp(convive,rt)==0)
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
    /*if (argc != 3) {
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
            int index = creer_groupe(c);
        
            int index_g = concat_chaine_in_membres(c,resa_conv,index);
            (void) index_g;
            if(nb_place_resa == 1) {
                //sem_post(&r->tables[placer].fin_table);
                sem_post(&r->tables[placer].sem_ta);
                c->groupes[index].nb_membres_gr = nb_place_resa;
                printf("index_g : %d\n",index);
                printf("c groupes resa : %d\n",c->groupes[index].nb_membres_gr);
                sem_wait(&r->tables[placer].fin_table);
                //r->nb_tables_occuper--;
            }
            
            //sem_post(&r->tables[placer].sem_time);
           
            //sem_post(&r->tables[placer].sem_ta);
            //TRAITEMENT 


            // FIN ICI 
            //sem_wait(&r->tables[placer].sem_time);
            r->nb_tables_occuper--;
            
        }
    }
    else {
        printf("là \n");
        char resa_conv[10];
        snprintf(conv_invit,len_1+1,"%s",argv[1]);
        snprintf(resa_conv,strlen(argv[2])+1,"%s",argv[2]);
        printf("resa_conv : %s\n",resa_conv);
        int placer_invit = chercher_first_c(resa_conv,r);
                        
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
    }*/
    char convive[80] = "HELLO COUCOU JEAN";
    struct strings r = line_in_strings(convive,' ');
    for(int i = 0; i < r.nb_string ;i++) {
        printf("s : %s\n",r.tab[i]);
    }
    /*printf("%s\n",r.tab[0]);
    printf("%s\n",r.tab[r.nb_string-1]);*/
    //printf("%s\n",substr("HELLO",1,4));
    /*char string[] = "HELLO COUCOU"; 
    printf("%s\n",strchr_(string,' ',1));
    printf("advance true = %s\n",string);*/

    /*
        // TO KEEP 
        char *string_malloc = (char*)(malloc(sizeof(char)*10));
        int size = 0;
        snprintf(string_malloc,(size=(strlen("HELLO")))+1,"%s","HELLO");
        advance_string(string_malloc,3);
        printf("advance : %s\n",string_malloc);
    */
    
   
    return 0;
}