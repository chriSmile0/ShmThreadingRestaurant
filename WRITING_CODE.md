# [Chris_C_Strandard]

## Indents
```
"[c]": {
		"editor.tabSize": 4,
		"editor.insertSpaces": false,
		"editor.detectIndentation":false
}
```

## Comments 
```
/**
 * @version x.y
 *
 * @brief	...
 * 
 * @param[:p1]	{char/int/etc}	Description 
 *
 * @return	{int/void/char}		Description
 *
 * @author X
*/
```
### Example 
```
/**
 * @version 1.0
 *
 * @brief	Print the x value in the standard output
 *
 * @param[:x]	{int}	the value to print
 *
 * @return {void} print the value
 * 
 * @author chriSmile0
*/
void print_value(int x);
```

## Macros 
- Write macros with comments if it's necessary 
- [FILENAME] 
	- `macros.h` for regroup the all macros<br> 
	- `checks.h` for all check macros 
	- `variables.h` for all variable macros  

## Variables/Functions names 
- snake_case 
### Single var declaration 
- `ìnt val_sibling = 2;`
### Multiples var declaration 
- `ìnt val_sibling, val_sibling2 = 2;`

## Function parameters
- `foo(void)`
- `foo(int param1)`
- `foo(int param1, int param2)`

## Conditions
### Over 80 characters condition
-	```
	if((abcdefghijklmnopqrstuvwxyz > 100000000) 
		&& (lorem_ipsum_notare_lepsum < 100000))
		return 12;
	```
- 	not 
	```
	if((abcdefghijklmnopqrstuvwxyz > 100000000) &&
		(lorem_ipsum_notare_lepsum < 100000))
		return 12;
	```
-	or not 
	```
	if((abcdefghijklmnopqrstuvwxyz > 100000000) &&   (lorem_ipsum_notare_lepsum < 100000))
		return 12;
	```
-	or not 
	```
	if((abcdefghijklmnopqrstuvwxyz > 100000000) 
		&& (lorem_ipsum_notare_lepsum < 100000))
			return 12;
	```

### Braces 
#### if only braces
- 	```
	if(a > b) {
		...
	}
	```
-	not
	```
	if(ab > b)
	{

	}
	```
#### if else braces
-	```
	if(a > b) {
		...
	}
	else {
		...
	}
	```
-	not 
	```
	if(a > b) {
		...
	} else {
		...
	}
	```

### Loop Conditions

#### While 

- `while(p1 < p2)` not `while ( p1 < p2 )`
- `while((var_x = (p1 < p2))) {...}` not `while((var_x = (p1 < p2)) == 1) {...}`
- `while(!(var_x = (p1 < p2))) {...}` not `while((var_x = (p1 < p2)) > 0) {...}`
-   ```
	while(str[i] != `\0` && (i < len_str)) {
		...;
		i++;
	}
	```

#### Do - While 
##### Single statement
-	```
	do
    	x--;
	while(x > 0);
	```
- 	not 
	```
	do 
	{
		x--;
	} while(x > 0);
	```
- 	or not 
	```
	do 
	{
		x--;
	} 
	while(x > 0);
	```
##### Multiples statement
- 	```
	do {
		x--;
		y++;
	} 
	while(x > 0);
	```
-	not 
	```
	do 
	{
		x--;
		y++;
	} while(x > 0);
	```

#### For 
- `for(int i = 0 ; i < 42 ; i++)`
- `for(; k < 42 ; k++)`

### Branch Conditions 

#### IF-ELSEIF-ELSE
##### Simple condition to ternary 
- `return((p1 > 5) ? ((p1 > 8) ? p1+40 : p1+4) : p1-3);`
-	not 
	```
	if(p1 > 5) {
		if(p1 > 8) 
			return p1+40; 
		else 
			return p1+4;
	}
	else {
		return p1-3;
	}
	```
##### Not useless braces
-	```
	if(a > b)
		printf("a > b\n");
	else 
		printf("a <= b\n");
	```
- 	not 	
	```
	if(a > b) {
		printf("a > b\n");
	}
	else {
		printf("a <= b\n");
	}
	```
##### Same level of braces
-	```
	if(a > b) {
		printf("a > b\n");
		a++;
	}
	else {
		printf("a <= b\n");
	}
	```
- 	not 
	```
	if(a > b) {
		printf("a > b\n");
		a++;
	}
	else 
		printf("a <= b\n");
	```
##### Embemding
-	```
	if(p1 > 5) {
		if(p1 > 8) 
			printf("value > 8 -> 9\n");
	}	
	else {
		printf("p1 > 5 and < 8 not important\n");
	}
	
	```
#### Switch 
[**DEFAULT_is_require**]
##### Return statement in case (no break necessary)
- 	```
	switch(c) {
		case 1 : return 5;
		case 2 : return 6;
		default:
			return 0;
	}
	```
##### No return in case with one statement per case
- 	```
	switch(c) {
		case 1 : c+=4;
			break;
		case 2 : c+=5;
			break;
		default: c = 0;
			break;
	}
	```
##### No return in case with many statement per case 
- 	```
	switch(c) {
		case 1 : 
			c+=4;
			y--;
			break;
		case 2 : 
			c+=5;
			y++;
			break;
		default: 
			c = 0;
			y = 0;
			break;
	}
	```

## Ternary Condition 
#### Boolean 
- `return (p1);` not `return ((p1) ? 1 : 0);`
- `return (p1 == p2);` not `return p1 == p2;`

#### Affectation 
- `int val = ((p1 == 3) ? p1+2 : p1-2);`
- `int val = ((p1 > 5) ? ((p1 > 8) ? p1+40 : p1+4) : p1-3);`

## Function usage 
- `fprintf(stdout, "%s: %d\n", "st", 1);` not `fprintf(stdout,"%s:%d\n","st",1);`