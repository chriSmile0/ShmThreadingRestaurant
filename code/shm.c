#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#include "shm.h"




struct restoo * open_resto(int nb_tables,int tab_capa[])
{
    struct restoo * rest_au_rang;
    int fd;
    int capa = 0;
    fd = shm_open(RESTO_NAME,O_RDWR |O_CREAT|O_EXCL,0600);
    if (fd == -1) {
        fprintf(stderr,"fichier deja existant ou impossible a creer \n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0 ; i < nb_tables;i++) 
        capa+= tab_capa[i];
    
    size_t taille = RESTO_SIZE(nb_tables);
    ftruncate(fd,taille);  
    rest_au_rang = mmap(NULL,taille,
                    PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (rest_au_rang == MAP_FAILED) {
        fprintf(stderr,"Impossible de projeter en mémoire\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    rest_au_rang->taille = taille;
    rest_au_rang->nb_tables = nb_tables;
    sem_init(&rest_au_rang->S_fin,1,0);

    for (int i = 0 ; i < nb_tables;i++) {
        rest_au_rang->tables[i].num = i;
        rest_au_rang->tables[i].capacite = tab_capa[i];
        printf("rest_au rang capa : i : %d , capa : %d\n",i,rest_au_rang->tables[i].capacite);
        sem_init(&rest_au_rang->tables[i].sem_ta,1,0);
        sem_init(&rest_au_rang->tables[i].sem_time,1,0);
        sem_init(&rest_au_rang->tables[i].fin_table,1,0);  
    }
    return rest_au_rang;
}

struct restoo * access_resto()
{
    int fd,filesize;
    struct stat s;
    struct restoo * re;
    fd = shm_open(RESTO_NAME,O_RDWR,0600);
    if (fd == -1) {
        fprintf(stderr,"fichier inexistant ou impossible a lire\n");
        exit(EXIT_FAILURE);
    }

    fstat(fd,&s);
    filesize = s.st_size;
    ftruncate(fd,filesize);//100 pour le moment
    re = mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if (re == MAP_FAILED) {
        fprintf(stderr,"Impossible de projeter en mémoire\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return (re);
}


struct cahier_rapel * open_cahier(int nb_groupes)
{
    struct cahier_rapel * cahier;
    int fd;
    fd = shm_open(CAHIER_NAME,O_RDWR |O_CREAT|O_EXCL,0600);
    if (fd == -1) {
        fprintf(stderr,"fichier deja existant ou impossible a creer \n");
        exit(EXIT_FAILURE);
    }
    
    ftruncate(fd,CAHIER_SIZE(nb_groupes));   
    size_t size_cahier = CAHIER_SIZE(nb_groupes);
    cahier = mmap(NULL,size_cahier,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (cahier == MAP_FAILED) {
        fprintf(stderr,"Impossible de projeter en mémoire\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    cahier->taille = size_cahier;
    cahier->nb_groupe = nb_groupes;

    for (int i = 0 ; i < nb_groupes;i++) 
        cahier->groupes[i].num_gr = 0;  
    
    return cahier;
}

struct cahier_rapel * access_cahier()
{
    struct cahier_rapel * cahier;
    int fd,filesize;
    struct stat s;
    fd = shm_open(CAHIER_NAME,O_RDWR ,0600);
    if (fd == -1) {
        fprintf(stderr,"fichier deja existant ou impossible a creer \n");
        exit(EXIT_FAILURE);
    }
 
    fstat(fd,&s);
    filesize = s.st_size;
    ftruncate(fd,filesize);//100 pour le moment
    size_t size_cahier = filesize;
    cahier = mmap(NULL,size_cahier,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (cahier == MAP_FAILED) {
        fprintf(stderr,"Impossible de projeter en mémoire\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    cahier->taille = size_cahier;

    return cahier;
}

void effacer_cahier()
{
    shm_unlink(CAHIER_NAME);
}


struct cahier_rapel *  copie_cahier(struct cahier_rapel * ancien)
{

    int ancienne_taille = ancien->nb_groupe;
    int nouveau_nb_groupe = 2*ancienne_taille;
    struct cahier_rapel * nouveau;

    int fd;
    fd = shm_open(CAHIER_NAME,O_RDWR ,0600);
    if (fd == -1) {
        fprintf(stderr,"le cahier n'existe pas\n");
        exit(EXIT_FAILURE);
    }
    
    ftruncate(fd,CAHIER_SIZE(nouveau_nb_groupe));   
    size_t size_cahier = CAHIER_SIZE(nouveau_nb_groupe);
    nouveau = mmap(NULL,size_cahier,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if (nouveau == MAP_FAILED) {
        fprintf(stderr,"Impossible de projeter en mémoire\n");
        exit(EXIT_FAILURE);
    }
    close(fd);
    nouveau->taille = size_cahier;
    int i = 0;
    for ( i = 0 ; i < ancienne_taille;i++) 
        nouveau->groupes[i].num_gr = ancien->groupes[i].num_gr; 
        
    for( i = ancienne_taille; i < nouveau_nb_groupe; i++)
        nouveau->groupes[i].num_gr = 0; 

    nouveau->nb_groupe = nouveau_nb_groupe;

    return nouveau;
    
}

int chaine_vide(char chaine[10])
{
    int i = 0;
    char * copie_c = malloc(11*sizeof(char));
    strncpy(copie_c,chaine,strlen(chaine));
    while (chaine[i] != '\0')
        i++;

    free(copie_c);
    return i;
}

int is_number(char chaine[])
{
    int i = 0;
    int taille_ch = strlen(chaine);
    while ((chaine[i] != '\0') && (isdigit(chaine[i])))
        i++;

    if (i == taille_ch) 
        return 1;

    fprintf(stderr,"usage: %s ne contient pas que des chiffres",chaine);
    exit(EXIT_FAILURE);
    
}

void lire_resto(struct restoo * r,FILE *fd)
{
    int nb_table = r->nb_tables;
    for (int i = 0 ; i < nb_table; i++) {
        struct table * t = &(r->tables[i]);
        int v = 0;
        if (sem_getvalue (&(t->sem_ta), &v) == -1)
            perror ("sem_getvalue");

        fprintf(fd, "table %d :",t->num);
        if(chaine_vide(t->convive) != 0) 
            fprintf(fd, " %s ",t->convive);
        else
            fprintf(fd, "(vide)");
           
        fprintf (fd, "\n");
    }
}

void lire_cahier(struct cahier_rapel * c,FILE *fd)
{
    int nb_groupe = c->nb_groupe;
    int i = 0;
    while ((c->groupes[i].num_gr != 0)  && (i < nb_groupe)){
        struct group * g = &(c->groupes[i]);
        fprintf(fd, "Groupe %d :  %s ",g->num_gr,g->membres_gr);
        fprintf (fd, "\n");
        i++;
    }
}


int creer_groupe(struct cahier_rapel * c)
{
    int nb_g = c->nb_groupe;
    int i = 0;
    while((c->groupes[i].num_gr  != 0)&&( i < nb_g)) 
        i++;

    if (i == nb_g) {
        struct cahier_rapel * ancien_cahier;
        ancien_cahier = c;
        struct cahier_rapel * new_cahier;
        new_cahier  = copie_cahier(ancien_cahier);
        c = new_cahier;
    }
    c->groupes[i].num_gr = i+1;
    return i;
}

void concat_maison(char dest[],char source[],int debut,int fin)
{
    int i = debut;
    int j = 0;
    while ( i < fin) {
        dest[j] = source[i];
        i++;
        j++;
    }
}

int concat_chaine_in_membres(struct cahier_rapel * c,char conv[],int index)
{
    int i = 0;
    char * copie_f = malloc(100*sizeof(char));
    memset(copie_f,'\0',100);
    strncpy(copie_f,conv,strlen(conv));
    char * copie = malloc(80*sizeof(char));
    memset(copie,'\0',80);
    strncpy(copie,c->groupes[index].membres_gr,strlen(c->groupes[index].membres_gr));
    while (copie[i] != '\0')
        i++;

    if( i == 0) {
        strncpy(c->groupes[index].membres_gr,copie_f,strlen(copie_f));
        c->groupes[index].membres_gr[strlen(copie_f)] = '\0';
    }
    else {
        strncat(c->groupes[index].membres_gr,conv,strlen(conv));
        c->groupes[index].membres_gr[strlen(conv)] = '\0';
    }
    free(copie_f);
    return index;
   
}

int chercher_first(char convive_f[],char membres[80])
{
    int i = 0;
    int taille_membres = strlen(membres);
    char chaine[10];
    int j = 0;
    int indice_fin_first = -1;
    while ( i < taille_membres) {
        if (isalpha(membres[i]) == 1024) {
            j = i;
            while ((membres[j] != ' ') && (j < taille_membres)) 
                j++;

            memset(chaine,'\0',10);
            concat_maison(chaine,membres,i,j);
            if (strncmp(chaine,convive_f,strlen(convive_f)) == 0) 
                indice_fin_first = j;
               
        }
        i+= i + j;
    }
    return indice_fin_first;
}

int nb_membres_gr(char membres[80])
{
    int taille_membres = strlen(membres);
    int i = 0;
    int j = 0;
    int nb_membres = 0;
    int taille_mot = 0;
    while (i < taille_membres) {
        if (isalpha(membres[i]) == 1024) {
            j = i;
            taille_mot = 0;
            while ((membres[j] != ' ') && (j < taille_membres)) 
                j++;

            nb_membres++;
            taille_mot = j;
        }
        i++;
        i+= taille_mot;
    }
    return nb_membres; 
}

int nb_conv_t(char convive[80])
{
    int taille_convive = strlen(convive);
    int i = 0;
    int j = 0;
    int nb_convive = 0;
    while (i < taille_convive) {
        if (isalpha(convive[i]) == 1024) {
            j = i;
            while ((convive[j] != ' ') && (j < taille_convive)) 
                j++;

            nb_convive++;
        }
        i++;
        i+= j;
    }
    return nb_convive; 
}


int nb_membres_absent(char membres[80])
{
    int taille_membres = strlen(membres);
    int i = 0;
    int j = 0;
    int taille_mot = 0;
    int nb_absent = 0;
    char chaine[10];
    char absent[] = "absent";
    while ( i < taille_membres) {
        if (isalpha(membres[i]) == 1024) {
            j = i;
            while ((membres[j] != ' ') && (j < taille_membres) && 
                    (membres[j] != '\0')) 
                j++;
            
            memset(chaine,'\0',10);
            concat_maison(chaine,membres,i,j);
            taille_mot = strlen(chaine);
            if (strncmp(chaine,absent,strlen(absent)) == 0)
                nb_absent++;

            i+= taille_mot;
        }
        i++;
    }
    return nb_absent;
}


int inserer_in_groupe(char convive_a[],struct cahier_rapel * c, 
                        char convive_f[], int nb_groupe)
{
    int i = 0;
    int search = -2;
    while ((((search = chercher_first(convive_f,c->groupes[i].membres_gr)) == -1)) 
            && (i < nb_groupe)) 
        i++;

    printf("i : %d\n",i);
    
    if (i == nb_groupe)
        return -1;

    snprintf(c->groupes[i].membres_gr + search,80," %s",convive_a);
    return i;
}

void affichage_police(FILE * fd) 
{
    struct restoo * r = access_resto();
    struct cahier_rapel * c = access_cahier();
    lire_resto(r,fd);
    lire_cahier(c,fd);
}


void close_resto()
{
    shm_unlink(RESTO_NAME);
}

void close_cahier()
{
    shm_unlink(CAHIER_NAME);
}

//***************************** UPDATE 2024 **********************************//
void print_table(struct table *t) {
    printf("\n********** TABLE n°%d **********\n",t->num);
    printf("Capacite de la table : %d\n",t->capacite);
    printf("Convives prévus pour la table : %d\n",t->nb_convive_t);
    int ta;
    sem_getvalue(&t->sem_ta, &ta);
    printf ("%s\n",(ta) ? "En Service": "Pas en Service");
    int tim;
    sem_getvalue(&t->sem_time, &tim);
    printf ("%s\n",(tim) ? "Reservé": "Non Reservé");
    int fin_ta;
    sem_getvalue(&t->fin_table, &fin_ta);
    printf ("%s\n", (ta) ? ((fin_ta) ? "Service terminé" : "Service en cours") : "");
    printf("Convives : %s\n",t->convive);
}


void print_resto(struct restoo *r) {
    printf("\n********** RESTO **********\n");
    printf("Taille segment resto : %ld\n",r->taille);
    printf("Nombre de tables occupées : %d\n",r->nb_tables_occuper);
    int nb_table = r->nb_tables;
    printf("Nombre totale de table du restaurant : %d\n",nb_table);
    int sfin;
    sem_getvalue(&r->S_fin, &sfin);
    printf ("%s\n",(sfin) ? "Fini": "Pas fini");
    for(int i = 0; i < nb_table; i++)
        print_table(&r->tables[i]);
   
}

void print_group(struct group *g) {
    printf("\n********** GROUPE n°%d **********\n",g->num_gr);
    printf("Nombre de membres du groupe : %d\n",g->nb_membres_gr);
    printf("Membres du groupe : %s\n",g->membres_gr);
}

void print_cahier(struct cahier_rapel *c) {
    printf("\n********** CAHIER **********\n");
    printf("Taille segment cahier : %ld\n",c->taille);
    int nb_groupe = c->nb_groupe;
    printf("Nombre de groupe dans le cahier : %d\n",nb_groupe);
    for(int i = 0 ; i < nb_groupe; i++) 
        print_group(&c->groupes[i]);

}