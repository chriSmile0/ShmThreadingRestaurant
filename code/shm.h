#include "check.h"
#include "values.h"
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
	sem_t sem_service;
	/** Semaphore pour signifier si quelqu'un a reserver cette table */
	sem_t sem_resa;
	/** Fin du service de la table*/
	sem_t sem_fin_repas;
	/** Fin du repas communiquer aux invités de celui qui a résa*/
	sem_t sem_fin_repas_convive;
	/** SEM ON EACH CHAIR */
	sem_t chairs[MAX_NB_CHAIRS];
	/** Liste de convive qui seront de taille capacite*/
	char convives[MAX_NB_CHAIRS][MAXLEN_NAME+1];
};

struct group {
	//Num du groupe
	int num_gr;
	//Nombre de membres//
	int nb_members_gr;
	//Numéro de la table
	int num_table;
	//Groupe au complet ?
	int g_full;
	//Present dans le groupe 
	int members_present;
	//Protection of up membres_present 
	sem_t sem_protect_mempre;
	char members_gr[MAX_NB_CHAIRS*MAXLEN_NAME];//Max 6 noms par groupes séparer par un espace
};

/* Representation du cahier de rappel*/

struct cahier_rapel {
	size_t taille;
	int nb_groupe;
	struct group groups[];
};


/** Représentation du resto complet. */
struct restoo {
	/** Taille du segment : Utile pour munmap */
	size_t taille;
	/** Donne le nombre de tables resa*/
	int nb_tables_resa;
	/** Donne le nombre de tables */
	int nb_tables;
	/** Fin du service dans le resto */
	sem_t sem_fin_service_resto;
	/** Fin du resto  */
	sem_t sem_fin_resto;
	struct table tables [];
};

#define CAHIER_SIZE(nb_groupes) sizeof(struct cahier_rapel) +  nb_groupes*sizeof(struct group)

#define RESTO_SIZE(nb_tables) sizeof(struct restoo) + nb_tables*sizeof(struct table)


/**
 * @version 1.0 
 * 
 * @brief	Creation of the sharing memory contain the restaurant
 * 
 * @param[in]	[:nb_tables]	{int} 		Number of table 
 * @param[in]	[:tab_capa] 	{int []}	All tables
 *
 * @return {struct restoo *}	the adress of the new segment project in memory
 * 
 * @author chriSmile0
*/
struct restoo * open_resto(int nb_tables,int tab_capa[]);

/** 
 * @version 1.0
 * 
 * @brief	
 * 			Project in memory of the segment in the sharing memory.
 * 			The segment of name #RESTO_NAME is project in memory and become
 * 			in access in read/write with the pointer in the return value
 * 			The segment is share with the all others process who project
 * 			this segment in memory.
 * 
 * @param[:/] {void}
 * 
 * @return 	{struct restoo *}	Adress of the existing sharing segment.
 * 
 * @author chriSmile0
*/
struct restoo * access_resto(void);

/** 
 * @version 1.0
 * 
 * @brief	Reset the table in parameter
 * 
 * @param[out]	[:t]	{struct table *}	the table t to reset	
 * 
 * @return 	{void} 
 * 
 * @author chriSmile0
*/
void reset_table(struct table *t);

/**
 * @version 1.0 
 * 
 * @brief	Creation of the sharing memory contain the restaurant
 * 
 * @param[:/]
 *
 * @return {struct cahier_rapel *}	the adress of the new segment project in memory
 * 
 * @author chriSmile0
*/
struct cahier_rapel * open_cahier(void);

/** 
 * @version 1.0
 * 
 * @brief 	Project in memory of the segment in the sharing memory.
 * 			The segment of name #CAHIER_NAME is project in memory and become
 * 			in access in read/write with the pointer in the return value
 * 			The segment is share with the all others process who project
 * 			this segment in memory.
 * 
 * @param[:/] {void}
 * 
 * @return 	{struct cahier_rapel *}	Adress of the existing sharing segment.
 * 
 * @author chriSmile0
*/

struct cahier_rapel * access_cahier(void);


/**
 * @version 1.0
 *  
 * @brief	Remove cahier
 *        
 * @param[:/] {void}
 * 
 * @return {void}
 * 
 * @author chriSmile0
*/
void close_cahier(void);

/** 
 * @version 1.0
 * 
 * @brief	Create a new cahier with the double size compared with the older
 * 			Because at the start we init this with the same number of table 
 * 			and it"s possible to have many groups than a number of a
 * 			table.
 * 			[NEW VERSION OF THIS FUNCTION IS NECESSARY!!]
 *
 * @param[in]	[:older] {struct cahier_rapel *}	Older cahier 
 *      
 * @return {struct cahier_rapel *}	the new cahier
 * 
 * @author chriSmile0
*/
struct cahier_rapel *  copy_cahier(struct cahier_rapel * older);

/** 
 * @version 1.0
 * 
 * @brief 	String is empty ?
 * 
 * @param[in]	[:str]	{char [10]} String to analyze
 *        
 * @return {int}	>0 not empty, 0 is empty
 * 
 * @author chriSmile0
*/
int empty_str(char str[10]);

/** 
 * @version 1.0
 * 
 * @brief	isdigit string version
 * 
 * @param[in]	[:str]			{char []}	String to analyze
 * @param[in]	[:raise_error] 	{int}		Boolean if we want to raise error	
 *        
 * @return {int}	1 if is ok , else exit(EXIT_FAILURE) or 0 is raise_error=0
 * 
 * @author chriSmile0
*/
int is_number(char str[], int raise_error);

/** 
 * @version 1.0
 * 
 * @brief 	Group creation
 * 
 * @param[out]	[:c] {struct cahier_rapel *}	the cahier
 * @param[in]	[:num_table] {int}				the number of the table, of the group we has lunch
 *
 * @return {int}	1 if ok.
 *
 * @author chriSmile0
*/
int create_group(struct cahier_rapel * c, int num_table);

/**
 * @version 1.0
 * 
 * @brief	Count the numbers of convives in convive string
 * 
 * @param[in]	[:convive] {char [80]}	The convives of the table
 *
 * @return	{int}	the number of sit convives at the table
 * 
 * @author chriSmile0
*/
int nb_conv_t(char convives[MAX_NB_CHAIRS][MAXLEN_NAME+1]);

/** 
 * @version 1.0
 * 
 * @brief	Count the number of not present people of a group
 * 
 * @param[in]	[:membres]	{char [80]}	Members of the group
 *
 * @return	{int}	the number of not present members = 0 or more
 * 
 * @author chriSmile0
*/
int nb_members_not_present(char membres[80]);

/** 
 * @version 1.0
 * 
 * @brief	Insert a conv in a group
 * 
 * @param[out]	[:c]		{struct cahier_rapel *} Le cahier
 * @param[in]	[:conv] 	{char []}				the convive to isnert
 * @param[in]	[:index]	{int}					the index of the group in the cahier
 *
 * @return 	{int}	index of the group in the cahier where the insertion it was done
 * 
 * @author chriSmile0
*/
int concat_str_in_membres(struct cahier_rapel * c, char conv[], int index);

/** 
 * @version 1.0
 * 
 * @brief	My insert_str version
 * 
 * @param[out]	[:dest] 	{char []} 	the destination
 * @param[in]	[:source] 	{char []}	the source 
 * @param[in]	[:begin] 	{int}		index of the begin of the source
 * @param[in]	[:end] 		{int}		index of the end ot the source
 *
 * @return	{void}	the destination with source 	
 * 
 * @author chriSmile0
*/
void insert_str_homemade(char dest[], char source[], int begin, int end);

/** 
 * @version 1.0
 * 
 * @brief	Search the first member of a group in all groups 
 * 			If the member is not found in the globality of cahier 
 * 			The convive is pray to leave the restaurant.
 * 
 * @param[in]	[:convive_w]	{char []} 				The convive we want to join a group
 * @param[out]	[:c] 			{struct cahier_rapel *}	The cahier
 * @param[in]	[:convive_f] 	{char []}				The first convive
 * @param[out]	[:nb_groupe] 	{int}					The number of group in the cahier
 *
 * @return {int}	index of group we insertion it was done or -1 if not found.
 * 
 * @author chriSmile0
*/
int insert_in_group(char convive_w[], struct cahier_rapel * c, 
						char convive_f[], int nb_groupe);

/** 
 * @version 1.0
 * 
 * @brief 	Use print_resto and print_cahier 
 * 
 * @param[in]	[:fd]	{FILE *}	the file description for the output of these print.
 *
 * @return {void}	
 * 
 * @author chriSmile0
*/
void affichage_police(FILE * fd); 

/** 
 * @version 1.0
 * 
 * @brief	Remove restaurant
 * 
 * @param[:/] {void} 
 * 
 * @return {void}
 * 
 * @author chriSmile0
*/
void close_resto(void);


//***************************** UPDATE 2024 **********************************//
/** 
 * @version 1.0
 * 
 * @brief	Print the table in the FILE f 
 *
 * @param[in]	[:t]	{struct table *}	the table
 * @param[in]	[:f] 	{FILE *}			the file descriptor to print the content
 
 * @return {void}
 * 
 * @author chriSmile0
*/
void print_table(struct table *t, FILE *f, int police);

/** 
 * @version 1.0
 * 
 * @brief	Print the resto in the FILE f 
 *
 * @param[in]	[:r]	{struct restoo *}	the resto
 * @param[in]	[:f]	{FILE *}			the file descriptor to print the content
 * 
 * @return {void}
 * 
 * @author chriSmile0
*/
void print_resto(struct restoo *r, FILE *f, int police);

/** 
 * @version 1.0
 * 
 * @brief 	Print the group in the FILE f
 *
 * @param[in]	[:g] {struct group *}	the group
 * @param[in]	[:f] {FILE *}			the file descriptor to print the content
 *
 * @return {void}
 * 
 * @author chriSmile0
*/
void print_group(struct group *g, FILE *f, int police);

/** 
 * @version 1.0
 * 
 * @brief	Print the cahier in the FILE f
 * 
 * @param[in]	[:c] {struct cahier_rapel *} 	the cahier
 * @param[in]	[:f] {FILE *}					the file descriptor to print the content
 * 
 * @return {void} 
 * 
 * @author chriSmile0
*/
void print_cahier(struct cahier_rapel *c, FILE *f, int police);

/**
 * @version 1.0
 * 
 * @brief 	Search if a token is present on a string
 * 
 * @param[in]	[:string]	{char *}	the string to analyze
 * @param[in]	[:token]	{char *}	the token search in string
 * 
 * @return {int}	>0 or 0
 * 
 * @author chriSmile0 
*/
int is_present(char * string, char *token);

/**
 * @version x.y
 *
 * @brief	...
 * 
 * @param[in]	[:p1]					{char/int/...}	Description
 * @param[in]	[:long_parameter] 		{char/int/...}	Description
 * @param[out]	[:modified_parameter	{char *|int *}	Description
 * 
 * 
 * @return	{int/void/char}	Description
 *
 * @author chriSmile0
*/
#endif /* __SHM_H__ */