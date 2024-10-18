#include "convive.h"

int smallest_fit_table(int nb_place, struct table tab_tables[],
								int taille_tab_t) {
	int last_fit = -1;
	for(int i = 0 ; i < taille_tab_t ; i++) {
		if(tab_tables[i].convives[0][0] != '\0')
			continue; // resa
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
	}
	return last_fit;
}

int insert_convive_first(struct restoo * r, char conv[], int nb_place)
{
	int nb_tables_max = r->nb_tables;
	int chercher_table_libre = smallest_fit_table(nb_place, r->tables, nb_tables_max);
	if(chercher_table_libre != -1) {
		strcpy(r->tables[chercher_table_libre].convives[0], conv); 
		r->tables[chercher_table_libre].nb_convive_t = nb_place;
	}
	return chercher_table_libre;
}

void insert_convive_second(struct restoo * r, char conv[], int index_table) {
	struct table *t = &(r->tables[index_table]);
	int i = 1;
	while(i < MAX_NB_CHAIRS && t->convives[i][0] != '\0')
		i++;
	
	snprintf(t->convives[i], MAXLEN_NAME+1, "%s", conv);
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

int chercher_first_c(char convive_f[], struct restoo * r)
{
	int nb_t =  r->nb_tables;
	int t = 0;
	while((t < nb_t) && strcmp(r->tables[t].convives[0],convive_f)!=0) 
		t++;
	if(t == nb_t)
		return -1;
	return t;
}

int main(int argc, char *argv[]) {
	if(argc != 3) 
		raler("%s Nom_du_premier_convive nb_place_resa ou Nom_convive Premier_convive", argv[0]);

	long int len_1 = strlen(argv[1]);
	long int len_2 = strlen(argv[2]);
	if((len_1 > 10) || (len_2 > 10) || (len_1 == 0 ) || (len_2 == 0)) 
		raler("%s %s %s , pas plus grand que 10 caracteres et non vide",
				argv[0], argv[1], argv[2]);

	int arg2_nombre = (is_number(argv[2], 0));

	struct restoo * r = access_resto();
	struct cahier_rapel * c = access_cahier();

	if(arg2_nombre) {
		int nb_place_resa = atoi(argv[2]);
		int placer = insert_convive_first(r, argv[1], nb_place_resa);
		if(placer == -1) {
			printf("Desole %s pas de table disponible \n",argv[1]);
		}
		else {
			printf("Bienvenue %s , vous avez la table %d\n", argv[1], placer);
			r->nb_tables_resa++;
			r->tables[placer].num = placer;
			int index = create_group(c, placer);
			strcpy(c->groups[index].members_gr, argv[1]);
			c->groups[index].nb_members_gr = nb_place_resa;
			sem_post(&r->tables[placer].sem_resa);
			sem_wait(&r->tables[placer].chairs[0]);
			r->nb_tables_resa--;
		}
	}
	else {
		int placer_invit = chercher_first_c(argv[2], r);
						
		if(placer_invit == -1) {
			printf("Desolé %s pas de %s ici \n", argv[1], argv[2]);
		}
		else {
			//char conv_invit[10];
			//char resa_conv[10];
			//snprintf(conv_invit, len_1+1, "%s", argv[1]);
			//snprintf(resa_conv,strlen(argv[2])+1, "%s", argv[2]);
			int x = insert_in_group(argv[1], c, argv[2], c->nb_groupe);
			if(x == -1) {
				printf("Table pleine \n");
				return 0;
			}
			printf("Bienvenue %s , vous avez la table %d \n",argv[1], placer_invit);
			insert_convive_second(r, argv[2], placer_invit);
			sem_wait(&r->tables[placer_invit].chairs[c->groups[x].members_present-1]);	
		}
	}
	return 0;
}