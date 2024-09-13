#ifndef SHM_H
#define SHM_H

#include <stdio.h> // For fprintf, perror
#include <stdlib.h> // For exit
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <sys/time.h>

/** Représentation d'une table dans le resto. */
struct table {
    /** Indice de la table suivant dans le chaînage */
    int num;
    /** Capacite max que peut acceuillir la table */
    int capacite;
    /* nb_convive prevu a la table par le premier convive*/
    int nb_convive_t;
    /** Sémaphore représentant si elle est cours de service */
    sem_t sem_ta;
    /** Semaphore pour signifier si quelqu'un a reserver cette table */
    sem_t sem_time;
    /** Fin du service de la table*/
    sem_t fin_table;
    /** Fin du repas communiquer aux invités de celui qui a résa*/
    sem_t sem_stand;
    /** Liste de convive qui seront de taille capacite*/
    char convive[80];
};

struct group {
    //Num du groupe
    int num_gr;
    //Nombre de membres//
    int nb_membres_gr;
    //Numéro de la table
    int num_table;
    //Groupe au complet ?
    int g_complet;
    char membres_gr[80];//Max 6 noms par groupes séparer par un espace
};

/* Representation du cahier de rappel*/

struct cahier_rapel {
    size_t taille;
    int nb_groupe;
    struct group groupes[];
};


/** Représentation du resto complet. */
struct restoo {
    /** Taille du segment : Utile pour munmap */
    size_t taille;
    /** Donne le nombre de tables occuper*/
    int nb_tables_occuper;
    /** Donne le nombre de tables */
    int nb_tables;
    sem_t S_fin;
    struct table tables [];
};


#define RESTO_NAME "/resto"
#define CAHIER_NAME "/cahier"


#define CAHIER_SIZE(nb_groupes) sizeof(struct cahier_rapel) +  nb_groupes*sizeof(struct group)

#define RESTO_SIZE(nb_tables) sizeof(struct restoo) + nb_tables*sizeof(struct table)


/**
 * @brief Creation d'un segment partagée contenant un restaurant 
 * 
 * @param[:nb_tables] Le nombre de table 
 * @param[:tab_capa] Le tableau qui contient toutes les capacités des tables
 *
 * @return L'adresse du nouveau segment , projeté en mémoire
 * 
*/

struct restoo * open_resto(int nb_tables,int tab_capa[]);

/** 
 * @brief Projection en mémoire du segment de mémoire partagée.
 *        Le segment de nom #RESTO_NAME est projeté en mémoire, et devient
 *        accessible en lecture/écriture via le pointeur fourni en valeur de
 *        retour. Le segment est partagé avec tous les autres processus qui
 *        le projettent également en mémoire.
 * 
 * @return  L'adresse du segment existant, projeté en mémoire
*/


struct restoo * access_resto(void);

/**
 * @brief Creation d'un segment partagée contenant un restaurant 
 * 
 * @param[:nb_groupes] Le nombre de groupes (qui peut evoluer)
 *
 * @return L'adresse du nouveau segment , projeté en mémoire
 * 
*/

struct cahier_rapel * open_cahier(int nb_groupes);

/** 
 * @brief Projection en mémoire du segment de mémoire partagée.
 *        Le segment de nom #CAHIER_NAME est projeté en mémoire, et devient
 *        accessible en lecture/écriture via le pointeur fourni en valeur de
 *        retour. Le segment est partagé avec tous les autres processus qui
 *        le projettent également en mémoire.
 * 
 * @return  L'adresse du segment existant, projeté en mémoire
*/

struct cahier_rapel * access_cahier();


/** 
 * @brief Suppression du cahier
 *        
 * @return no return
*/

void close_cahier();

/** 
 * @brief Creer un nouveau cahier avec la taille doubler par
 *        rapport a l'ancien. Comme au départ je met un nombre
 *        égal au nombre de tables, si jamais il y'a plus de groupes
 *        que de tables on agrandit le cahier en créant un nouveau 
 *
 * @param[:ancien] L'ancien cahier 
 *      
 * @return Le nouveau cahier
*/

struct cahier_rapel *  copie_cahier(struct cahier_rapel * ancien);

/** 
 * @brief Cherche a savoir si une chaine de caracteres ne contient que des
 *        chiffres
 * 
 * @param[:chaine] La chaine a analyser
 *        
 * @return retourne 1 si bon , sinon exit(EXIT_FAILURE)
*/

int is_number(char chaine[]);

/** 
 * @brief Cherche a savoir si une chaine est vide
 * 
 * @param[:chaine] La chaine a analyser
 *        
 * @return retourne 0 si vide, sinon > 0
*/

int chaine_vide(char chaine[]);

/** 
 * @brief Initialisation des champs convive des différentes tables
 * 
 * @param[:r] Le restaurant 
 *        
 * @return no return
*/

void init_restoo(struct restoo * r);

/** 
 * @brief Lit le restaurant 
 * 
 * @param[:r] Le restaurant
 * @param[:fd] Le descripteur de fichier sur lequel ecrire
 *        
 * @return no return
*/

void lire_resto(struct restoo * r, FILE *fd);

/** 
 * @brief Lit le cahier
 * 
 * @param[:c] Le cahier
 * @param[:fd] Le descripteur de fichier sur lequel ecrire
 *        
 * @return no return
*/

void lire_cahier(struct cahier_rapel * c, FILE *fd);

/** 
 * @brief Vide la table , et remet son sem_ta a 0 ,la rend libre pour un autre
 *        groupe
 * 
 * @param[:t] la table
 *
 * @return retourne 1 si terminer
*/

int debarasser_table(struct table t);

/** 
 * @brief Creation d'un groupe ( a finir)
 * 
 * @param[:c] le cahier
 * @param[:num_table] le numéro de la table que/qu'à occuper le groupe
 *
 * @return retourne 1 si bonne execution
*/

int creer_groupe(struct cahier_rapel * c, int num_table);

/** 
 * @brief Chercher si le le convive entrer en parametre est deja dans les 
 *        membres du groupe passer en parametre via sa chaine de caracteres
 *        membres
 * 
 * @param[:convive_f] le convive a chercher
 * @param[:membres] Les membres du groupe
 *
 * @return retourne l'indice de la dernière lettre du convive entrer en 
 *         parametre si réussite sinon -1
*/

int chercher_first(char convive_f[],char membres[80]);

/** 
 * @brief Compte le nombre de membres dans la chaine passer en parametre
 * 
 * @param[:membres] Les membres du groupe
 *
 * @return retourne le nombre de membre
*/

int nb_membres_gr(char membres[80]);

/**
 * @brief Compte le nombre de convives dans la chaine convive
 * 
 * @param[:convive] Les convives de la table
 *
 * @return retourne le nombre de convive assis autour de la table
*/

int nb_conv_t(char convive[80]);

/** 
 * @brief Compte le nombre de membres absent dans la chaine passer en parametre
 *        Ils sont symboliser par la chaine "absent" donc quand le restaurant
 *        dit que tu commence a manger même si il manque des personnes du groupe
 *        alors il faut passer le semaphore de la table au bon niveau 
 *        pour que la restaurant sois en règle après.
 * 
 * @param[:membres] Les membres du groupe
 *
 * @return retourne le nombre d'absent du groupe, sois 0 ou plus
*/

int nb_membres_absent(char membres[80]);

/** 
 * @brief Cherche a inserer dans un groupe un membre passer en parametre
 *        Si ce groupe est vide on copie sinon on concatene
 * 
 * @param[:c] Le cahier
 * @param[:conv] Le convive a inserer
 *
 * @return L'indice du groupe dans lequel on a inserer ce convive
*/

int concat_chaine_in_membres(struct cahier_rapel * c,char conv[], int index);

/** 
 * @brief Fonction de concatenation qui concatene une chaine à une autre
 *        en partant de debut jusqu'à fin de la source
 * 
 * @param[:dest] la destination
 * @param[:source] la source 
 * @param[:debut] indice de debut de la source
 * @param[:fin] indice de fin de la source
 *
 * @return L'indice du groupe dans lequel on a inserer ce convive
*/


void concat_maison(char dest[],char source[],int debut,int fin);

/** 
 * @brief Cherche a inserer dans un groupe un membre passer en parametre
 *        Si le convive chercher est bien là c'est bon sinon on refoule
 * 
 * @param[:convive_a] Le convive attendu
 * @param[:c] Le cahier
 * @param[:convive_f] Le premier convive avec lequel le convive_a doit manger
 * @param[:nb_groupe] Le nombre de groupe
 *
 * @return L'indice du groupe dans lequel on a inserer ce convive
*/

int inserer_in_groupe(char convive_a[],struct cahier_rapel * c, char convive_f[], int nb_groupe);

/** 
 * @brief Va afficher utiliser lire_resto et lire_cahier pour faire l'état
 *        des lieux
 * 
 * @param[:fd] Le descripteur de sortie pour l'affichage

 * @return no return 
*/

void affichage_police(FILE * fd); 

/** 
 * @brief Suppression du restaurant
 *        
 * @return no return
*/

void close_resto(void);



//***************************** UPDATE 2024 **********************************//
/** 
 * @brief 
 *
 * @param[:t] la table
 * 
 *
 * @return 
*/
void print_table(struct table *t);



/** 
 * @brief 
 *
 * @param[:r] le resto
 * 
 *
 * @return 
*/
void print_resto(struct restoo *r);

/** 
 * @brief 
 *
 * @param[:g] le groupe
 * 
 *
 * @return 
*/
void print_group(struct group *g);

/** 
 * @brief 
 *
 * @param[:c] le cahier
 * 
 *
 * @return 
*/
void print_cahier(struct cahier_rapel *c);


#endif /* __SHM_H__ */