#ifndef CONVIVE_H
#define CONVIVE_H

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "shm.h"

struct strings {
	char *tab[10];
	int nb_string;
};

/**
 * @version 1.0 
 * 
 * @brief	Copy of a table in another
 *        
 * @param[in]	[:source]		{struct table[]}	tab of table source
 * @param[out]	[:dest] 		{struct table[]} 	tab of table destination
 * @param[in]	[:same_taille] 	{int}				number of copy
 *
 * @return void	
 * 
 * @author chriSmile0
 * 
*/
void copy_tab(struct table source[], struct table dest[], int same_taille);

/**
 * @version 1.0
 * 
 * @brief	A member of group is present at the table.
 * 
 * @param[out]	[:g]	{struct group *}	the pointer on a group
 *
 * @return void
 * 
 * @author chriSmile0
*/
void up_present(struct group *g);

/**
 * @version 1.0
 * 
 * @brief 	the group sit at the table is ready to lunch
 * 
 * @param[out]	[:g]	{struct group *}	the group
 * @param[out]	[:t] 	{struct table *}	the table 
 *
 * @return void 
 * 
 * @author chriSmile0
*/
void ready_to_lunch(struct group *g, struct table *t);

/**
 * @version 1.0
 * 
 * @brief	Best fit table for a group
 *        
 * 
 * @param[in]	[:nb_place]		{int }			First member indicate number of people
 * @param[in]	[:tab_tables] 	{struct table}	Tab of tables
 * @param[in]	[:size_tab_t] 	{int}			size of tab of tables
 *
 * @return {int}	index of the smallest table 
 * 
 * @author chriSmile0
*/
int smallest_table(int nb_place, struct table tab_tables[],
					int size_tab_t);

/**
 * @version 1.0
 * 
 * @brief 	Insert the first member of the group in the table convives string
 * 
 * @param[out]	[:r]			{struct restoo *}	The pointer of the restaurant
 * @param[in]	[:conv] 		{char []}			The convive to insert
 * @param[in]	[:nb_place] 	{int}				Number of place we want to insert
 *
 * @return	{int}	the index of the table where we are insert the conv
 * 
 * @author chriSmile0
*/
int insert_convive_first(struct restoo * r, char conv[], int nb_place);

void inserer_convive_second(struct restoo * r, char conv[], int index_table);

/**
 * @version 1.0
 * 
 * @brief 	Search the name of the first member in the restaurant
 *        
 * @param[in]	[:convive_f]	{char []}	convive_f	the first convive
 * @param[out]	[:r]			{struct restoo *}		the pointer on the restaurant
 *
 * @return	{int}	the index of the table we found the convive_f
 * 
 * @author chriSmile0
*/
int search_first_c(char convive_f[], struct restoo * r);

/**
 * @version 1.0
 * 
 * @brief 	Advance the string in param in the position enter in pos argument. 
 * 
 * @param[out]	[:original_string]	{char*} char *string = "HELLO" dont work -> str[]="blabla",str[100]="blabla" works
 * @param[in]	[:pos] 			{int}	the position of the next first character
 * 
 * @return 	{void} 
 * 
 * @author chriSmile0
*/
void advance_string(char *original_string, int pos);

/**
 * @version 1.0
 * 
 * @brief 	My substr version
 * 
 * @param[in]	[:string]  {char *}	The string to cut 
 * @param[in]	[:dept]    {int}		The dept of the part to extract
 * @param[in]	[:end]     {int}T		he end of the part to extract
 * 
 * @return the part of the string {dept,end}
 * 
 * @author chriSmile0
*/
char * substr(char *string, int dept, int end);

/**
 * @version 1.0
 * 
 * @brief 	My strchr version
 * 
 * @param[in/out]	[:string]  		{char *}	the string to extract a part
 * @param[in]		[:search]		{char}		the character to want to check 
 * 									and use like a separator
 * 
 * @param[in]		[:advance_src]	{int}		a boolean if we want to advance 
 * 									the original string or not
 * 									(true) -> for loop usage
 * 
 * @return	{char *} -> the part of the string between the start of the 
 *                      string and the position of the character we search 
 * 
 * @author chriSmile0
*/
char * strchr_(char * string, char search, int advance_src);

/**
 * @version 1.0
 * 
 * @brief 	My strtok version
 * 
 * @param[in/out]	[:string] 		the string to extract a part
 * @param[in]		[:token]		the token to want to check and use like a separator
 * @param[in]		[:advance_src]	a boolean if we want to advance the original string or not
 * 									(true) -> for loop usage
 * @return  explicitreturn -> the part of the string between the start of the 
 *                          	string and the position of the token we search 
 * 
 * @author chriSmile0
*/
char * strtok_(char * string, char *token, int advance_src);

/**
 * @version 1.0
 * 
 * @brief 	Cut string in tab of string
 * 
 * @param[in]	[:convive] Initialize string
 * @param[in]	[:separator] character separator
 * 
 * @return {struct strings}	contains all strings we separate by separator but 
 * 							without the separator character
 * 
 * @author chriSmile0
*/
struct strings line_in_strings(char string[], char separator);


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
#endif /* CONVIVE_H */