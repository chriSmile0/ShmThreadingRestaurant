#include "convive.h"

void copy_tab(struct table source[],struct table dest[],int same_taille)
{
	for(int i = 0 ; i < same_taille;i++) 
		dest[i] = source[i];
}

void up_present(struct group *g) {
	//sem_wait(&g->sem_protect_mempre);
	g->membres_present++;
	//sem_post(&g->sem_protect_mempre);
}

void ready_to_lunch(struct group *g, struct table *t) {
	g->g_complet = 1;
	sem_post(&t->sem_service); // en service
}

int plus_petite_table_possible(int nb_place,struct table tab_tables[],
								int taille_tab_t)
{
	int i = 0;
	int resa = -1;
	int last_fit = -1;
	while((sem_getvalue(&tab_tables[i].sem_resa,&resa) == 0)
		&& (i < taille_tab_t)) {
			//SEARCH PERFECT TABLE
		//printf("tim in : %d\n",tim);
		if(resa == 1) {
			i++;
			continue;
		}
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
	//printf("tim out : %d\n",tim);
	return last_fit;
}

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
	//printf("l_b : %d\n",l_b);
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

void inserer_convive_second(struct restoo * r, char conv[], int index_table) {
	snprintf(r->tables[index_table].convive+strlen(r->tables[index_table].convive),80,"%s%s"," ",conv);
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

/*
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
}*/
/*
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
}*/

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
	if (argc != 3) 
		raler("%s Nom_du_premier_convive nb_place_resa ou Nom_convive Premier_convive",argv[0]);

	long int len_1 = strlen(argv[1]);
	long int len_2 = strlen(argv[2]);
	if((len_1 > 10) || (len_2 > 10) || (len_1 == 0 ) || (len_2 == 0)) 
		raler("%s %s %s , pas plus grand que 10 caracteres et non vide",argv[0],argv[1],argv[2]);

	char resa_conv[10];
	int arg2_nombre = 0;
	if(isdigit(argv[2][0]) == 2048) 
		if(is_number(argv[2]))
			arg2_nombre = 1;


	printf("arg2_nombre : %d\n",arg2_nombre);
	
	struct restoo * r = access_resto();
	struct cahier_rapel * c = access_cahier();

	if(arg2_nombre == 1) {
		int nb_place_resa = atoi(argv[2]);
		snprintf(resa_conv,len_1+1,"%s",argv[1]);
		int placer = inserer_convive_first(r,resa_conv,nb_place_resa);
		if(placer == -1) {
			printf("Desole %s pas de table disponible \n",argv[1]);
		}
		else {
			printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer);
			r->nb_tables_occuper++;
			r->tables[placer].num = placer;
			int index = create_group(c,placer);
		
			int index_g = concat_str_in_membres(c,resa_conv,index);
			(void) index_g;
			c->groupes[index].nb_membres_gr = nb_place_resa;
			sem_post(&r->tables[placer].sem_resa);
			//sem_wait(&r->tables[placer].sem_fin_repas_convive);
			sem_wait(&r->tables[placer].chairs[0]);
			r->nb_tables_occuper--;
		}
	}
	else {
		char conv_invit[10];
		char resa_conv[10];
		snprintf(conv_invit,len_1+1,"%s",argv[1]);
		snprintf(resa_conv,strlen(argv[2])+1,"%s",argv[2]);
		int placer_invit = chercher_first_c(resa_conv,r);
		printf("placer invit %d\n", placer_invit);
						
		if(placer_invit == -1) {
			printf("Desolé %s pas de %s ici \n",argv[1],argv[2]);
		}
		else {
			int x = insert_in_group(conv_invit,c,resa_conv,c->nb_groupe);
			printf("x : %d\n",x);
			if(x == -1) {
				printf("Table pleine \n");
				return 0;
			}
			printf("Bienvenue %s , vous avez la table %d\n",argv[1],placer_invit);
			inserer_convive_second(r,conv_invit,placer_invit);
			/*printf("|%s|\n",r->tables[placer_invit].convive);
			printf("nb_membres : %d\n",nb_membres_groupe);
			printf("before present ? : %d\n",c->groupes[x].membres_present);
			//up_present(&c->groupes[x]);
			printf("present ? : %d\n",c->groupes[x].membres_present);
			printf("attendus ? : %d\n",c->groupes[x].nb_membres_gr);*/
			if(c->groupes[x].g_complet) {
				printf("go lunch \n");
				sem_wait(&r->tables[placer_invit].chairs[c->groupes[x].membres_present-1]);
			}
			else {
				sem_wait(&r->tables[placer_invit].chairs[c->groupes[x].membres_present-1]);
			}
				
		}
	}
	return 0;
}