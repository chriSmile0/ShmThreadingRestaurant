#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


#include "shm.h"

struct restoo * open_resto(int nb_tables, int tab_capa[])
{
	struct restoo * r;
	int fd;
	int capa = 0;
	fd = shm_open(RESTO_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
	if(fd == -1) 
		raler("fichier deja existant ou impossible a creer");
	
	for(int i = 0 ; i < nb_tables ; i++) 
		capa += tab_capa[i];
	
	size_t taille = RESTO_SIZE(nb_tables);
	ftruncate(fd,taille);  
	r = mmap(NULL, taille, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(r == MAP_FAILED) 
		raler("Impossible de projeter en mémoire");
	
	close(fd);
	r->taille = taille;
	r->nb_tables = nb_tables;
	sem_init(&r->sem_fin_service_resto, 1, 0);
	sem_init(&r->sem_fin_resto, 1, 0);

	for(int i = 0 ; i < nb_tables ; i++) {
		r->tables[i].num = i;
		r->tables[i].capacite = tab_capa[i];
		printf("rest_au rang capa : i : %d , capa : %d\n", i, r->tables[i].capacite);
		sem_init(&r->tables[i].sem_service, 1, 0);
		sem_init(&r->tables[i].sem_resa, 1, 0);
		sem_init(&r->tables[i].sem_fin_repas, 1, 0);  
		sem_init(&r->tables[i].sem_fin_repas_convive, 1, 0);
		for(int j = 0 ; j < tab_capa[i] ; j++)
			sem_init(&r->tables[i].chairs[j], 1, 0);
	}
	return r;
}

struct restoo * access_resto()
{
	int fd,filesize;
	struct stat s;
	struct restoo * r;
	fd = shm_open(RESTO_NAME, O_RDWR, 0600);
	if(fd == -1) 
		raler("fichier inexistant ou impossible a lire");

	fstat(fd,&s);
	filesize = s.st_size;
	ftruncate(fd, filesize);//100 pour le moment
	r = mmap(NULL, filesize, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0);
	if(r == MAP_FAILED)
		raler("Impossible de projeter en mémoire");

	close(fd);
	return (r);
}

void reset_table(struct table *t) {
	sem_post(&t->sem_fin_repas);
	sem_wait(&t->sem_resa);
	memset(t->convive,'\0',80);
	t->nb_convive_t = 0;
}


struct cahier_rapel * open_cahier()
{
	struct cahier_rapel * cahier;
	int fd;
	fd = shm_open(CAHIER_NAME, O_RDWR | O_CREAT | O_EXCL, 0600);
	if(fd == -1) 
		raler("fichier deja existant ou impossible a creer");
	
	ftruncate(fd, CAHIER_SIZE(0));   
	size_t size_cahier = CAHIER_SIZE(0);
	cahier = mmap(NULL, size_cahier ,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(cahier == MAP_FAILED) 
		raler("Impossible de projeter en mémoire");

	close(fd);
	cahier->taille = size_cahier;
	cahier->nb_groupe = 0;
	return cahier;
}

struct cahier_rapel * access_cahier()
{
	struct cahier_rapel * cahier;
	int fd, filesize;
	struct stat s;
	fd = shm_open(CAHIER_NAME, O_RDWR ,0600);
	if(fd == -1) 
		raler("fichier deja existant ou impossible a creer");
	

	fstat(fd, &s);
	filesize = s.st_size;
	ftruncate(fd, filesize);//100 pour le moment
	size_t size_cahier = filesize;
	cahier = mmap(NULL, size_cahier, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(cahier == MAP_FAILED) 
		raler("Impossible de projeter en mémoire");
	
	close(fd);
	cahier->taille = size_cahier;

	return cahier;
}

void effacer_cahier() {shm_unlink(CAHIER_NAME);}


struct cahier_rapel *  copy_cahier(struct cahier_rapel * older) {

	int oldest_size = older->nb_groupe;
	int new_nb_groupe = oldest_size+1;
	struct cahier_rapel * new;

	int fd;
	fd = shm_open(CAHIER_NAME, O_RDWR, 0600);
	if(fd == -1)
		raler("the cahier dont exist");
	
	ftruncate(fd, CAHIER_SIZE(new_nb_groupe));   
	size_t size_cahier = CAHIER_SIZE(new_nb_groupe);
	new = mmap(NULL, size_cahier ,PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(new == MAP_FAILED) 
		raler("Impossible to project in memory"); 
	
	close(fd);
	new->taille = size_cahier;
	int i = 0;
	for(i = 0 ; i < oldest_size ; i++) 
		new->groupes[i].num_gr = older->groupes[i].num_gr; 
		
	for(i = oldest_size ; i < new_nb_groupe ; i++)
		new->groupes[i].num_gr = 0; 

	new->nb_groupe = new_nb_groupe;

	return new;
	
}

int empty_str(char str[10])
{
	int i = 0;
	char * copy_s = malloc(11*sizeof(char));
	strncpy(copy_s, str, strlen(str));
	while(str[i] != '\0')
		i++;

	free(copy_s);
	return i;
}

int is_number(char str[])
{
	int i = 0;
	int size_str = strlen(str);
	while((str[i] != '\0') && (isdigit(str[i])))
		i++;

	if(i == size_str) 
		return 1;

	raler("usage: %s ne contient pas que des chiffres", str);
}

int create_group(struct cahier_rapel * c, int num_table)
{
	int nb_g = c->nb_groupe;
	int i = 0;
	while((c->groupes[i].num_gr != 0) && (i < nb_g)) 
		i++;

	if(i == nb_g) {
		struct cahier_rapel * older_cahier;
		older_cahier = c;
		struct cahier_rapel * new_cahier;
		new_cahier  = copy_cahier(older_cahier);
		c = new_cahier;
	}
	c->groupes[i].num_gr = i+1;
	c->groupes[i].num_table = num_table;
	c->groupes[i].g_complet = 0;
	c->groupes[i].membres_present = 1;
	sem_init(&c->groupes[i].sem_protect_mempre, 1, 1);
	return i;
}

void insert_str_homemade(char dest[], char source[], int begin, int end)
{
	int i = begin;
	int j = 0;
	while(i < end) {
		dest[j] = source[i];
		i++;
		j++;
	}
}

int concat_str_in_membres(struct cahier_rapel * c, char conv[], int index)
{
	int i = 0;
	char * copy_f = malloc(100*sizeof(char));
	memset(copy_f, '\0', 100);
	strncpy(copy_f, conv, strlen(conv));
	char * copy = malloc(80*sizeof(char));
	memset(copy, '\0', 80);
	strncpy(copy, c->groupes[index].membres_gr, strlen(c->groupes[index].membres_gr));
	while(copy[i] != '\0')
		i++;
	printf("i : %d\n", i);
	if(i == 0) {
		strncpy(c->groupes[index].membres_gr, copy_f, strlen(copy_f));
		c->groupes[index].membres_gr[strlen(copy_f)] = '\0';
	}
	else {
		strncat(c->groupes[index].membres_gr,conv, strlen(conv));
		c->groupes[index].membres_gr[strlen(conv)] = '\0';
	}
	free(copy_f);
	return index;

}

int search_first(char convive_f[], char membres[80])
{
	//int i = 0;
	/*int taille_membres = strlen(membres);
	printf("taille membres %d\n",taille_membres);
	char chaine[10];
	int j = 0;
	int indice_fin_first = -1;
	while ( i < taille_membres) {
		if(isalpha(membres[i]) == 1024) {
			j = i;
			while ((membres[j] != ' ') && (j < taille_membres)) 
				j++;

			memset(chaine,'\0',10);
			concat_maison(chaine,membres,i,j);
			if(strncmp(chaine,convive_f,strlen(convive_f)) == 0) 
				indice_fin_first = j;
			
		}
		i+= i + j;
	}*/
	strcat(membres, convive_f);
	return strlen(membres);
}

int nb_membres_gr(char membres[80])
{
	int taille_membres = strlen(membres);
	int i = 0;
	int j = 0;
	int nb_membres = 0;
	int taille_mot = 0;
	while(i < taille_membres) {
		if(isalpha(membres[i]) == 1024) {
			j = i;
			taille_mot = 0;
			while((membres[j] != ' ') && (j < taille_membres)) 
				j++;

			nb_membres++;
			taille_mot = j;
		}
		i++;
		i+= taille_mot;
	}
	return nb_membres; 
}

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

int nb_conv_t(char convive[80])
{
	int taille_convive = strlen(convive);
	int i = 0;
	/*int j = 0;
	int nb_convive = 0;*/
	/*char * copy_f = malloc(80*sizeof(char));
	memset(copy_f, '\0', 80);
	strncpy(copy_f, convive, strlen(convive));
	while(strcmp(copy_f,strchr_(copy_f,' ',1))!=0)
		i++;*/
	/*while(i < taille_convive) {
		if(isalpha(convive[i]) == 1024) {
			j = i;
			while((convive[j] != ' ') && (j < taille_convive)) 
				j++;

			nb_convive++;
		}
		i++;
		i+= j;
	}*/
	int j = 0;
	if(taille_convive > 0)
		j = 1;
	while(i < taille_convive) {
		if(convive[i] == ' ') 
			j++;
		i++;
	}
	return j;
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
	while(i < taille_membres) {
		if(isalpha(membres[i]) == 1024) {
			j = i;
			while((membres[j] != ' ') && (j < taille_membres) && 
					(membres[j] != '\0')) 
				j++;
			
			memset(chaine, '\0', 10);
			insert_str_homemade(chaine,membres, i, j);
			taille_mot = strlen(chaine);
			if(strncmp(chaine, absent, strlen(absent)) == 0)
				nb_absent++;

			i+= taille_mot;
		}
		i++;
	}
	return nb_absent;
}


int insert_in_group(char convive_w[], struct cahier_rapel * c, 
						char convive_f[], int nb_groupe)
{
	int i = 0;
	int present = -1;
	while((!(present = is_present(c->groupes[i].membres_gr,convive_f))) 
		&& (i < nb_groupe))
		i++;

	printf("i : %d\n",i);

	if(i == nb_groupe)
		return -1;
	else 
		if(c->groupes[i].g_complet == 1)
			return -1;

	snprintf(c->groupes[i].membres_gr + strlen(c->groupes[i].membres_gr), 80, 
				" %s", convive_w);
	c->groupes[i].membres_present++;
	if(c->groupes[i].nb_membres_gr == c->groupes[i].membres_present)
		c->groupes[i].g_complet = 1;
	return i;
}

void affichage_police(FILE * fd) 
{
	struct restoo * r = access_resto();
	struct cahier_rapel * c = access_cahier();
	(void) fd;
	//printf("\n***POLICE***\n");

	print_resto(r, fd, 1);
	print_cahier(c, fd, 1);
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
void print_table(struct table *t, FILE *f, int police) {
	if(police) 
		fprintf(f, "\nTable %d %s" , t->num, (!t->nb_convive_t) ? "(vide)" : "");
	else 
		fprintf(f, "\n********** TABLE n°%d**********\n", t->num);
	
	fprintf(f, "Capacite de la table : %d\n", t->capacite);
	fprintf(f, "Convives prévus pour la table : %d\n", t->nb_convive_t);
	int service;
	sem_getvalue(&t->sem_service, &service);
	fprintf(f, "%s\n", (service==1) ? "En Service" : "Pas en Service");
	int resa;
	sem_getvalue(&t->sem_resa, &resa);
	fprintf(f, "%s\n",(resa==1) ? "Reservé" : "Non Reservé");
	int fin_repas;
	sem_getvalue(&t->sem_fin_repas, &fin_repas);
	fprintf(f, "%s\n", (service==1) ? ((fin_repas) ? "Service terminé" : "Service en cours") : "");
	fprintf(f, "Convives : %s\n", t->convive);
}


void print_resto(struct restoo *r, FILE *f, int police) {
	fprintf(f, "\n********** RESTO **********\n");
	fprintf(f, "Taille segment resto : %ld\n", r->taille);
	fprintf(f, "Nombre de tables occupées : %d\n", r->nb_tables_occuper);
	int nb_table = r->nb_tables;
	fprintf(f, "Nombre totale de table du restaurant : %d\n", nb_table);
	int sfin_service;
	sem_getvalue(&r->sem_fin_service_resto, &sfin_service);
	fprintf(f, "%s\n", (sfin_service) ? "Fin du service" : "Service Pas fini");
	int sfin_resto;
	sem_getvalue(&r->sem_fin_resto, &sfin_resto);
	fprintf(f, "%s\n", (sfin_resto) ? "Fin du restaurant" : "Resto Pas fini");
	for(int i = 0 ; i < nb_table ; i++)
		print_table(&r->tables[i], f, police);

}

void print_group(struct group *g, FILE *f, int police) {
	if(police) {
		fprintf(f, "\n********** Groupe %d %s**********\n", g->num_gr, g->membres_gr);
	}
	else {
		fprintf(f, "\n********** GROUPE n°%d **********\n", g->num_gr);
		fprintf(f, "Membres du groupe : %s\n", g->membres_gr);
	}
	fprintf(f, "Nombre de membres du groupe : %d\n", g->nb_membres_gr);
	fprintf(f, "Nombre de membres du groupe présent : %d\n", g->membres_present);
	fprintf(f, "Numéro de la table du diner : %d\n", g->num_table);
}

void print_cahier(struct cahier_rapel *c, FILE *f, int police) {
	(void) police;
	fprintf(f, "\n********** CAHIER **********\n");
	fprintf(f, "Taille segment cahier : %ld\n", c->taille);
	int nb_groupe = c->nb_groupe;
	fprintf(f, "Nombre de groupe dans le cahier : %d\n", nb_groupe);
	for(int i = 0 ; i < nb_groupe ; i++) 
		print_group(&c->groupes[i], f, police);

}

/**
 * @brief Search ifa token is present on a string
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