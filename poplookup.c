/*
 * CSE30 FA21 HW6
 * CSE30 username: cs30fa21cXX (TODO: Fill in)
 */

#include "poplookup.h"

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * main
 *
 * Arguments: argc, argv
 *
 * Operation: Main driver for the program, calls other funttions to:
 *	          parse the options, allocate the hash table, load the table, print out the table stats
 *            and make print population stats of the desired city/state
 * Returns:   EXIT_SUCCESS if all ok, EXIT_FAILURE otherwise
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
int main(int argc, char *argv[]) {
	node** table;
	unsigned long size = TABLE_SIZE;
	// name of csv file
	char* filename;
	int info = 0;
	char* city;
	char* state;
	// indicates if the hash is to be done by city or state
	int hash_by_city = 1;

	if (!parse_opts(argc, argv, &filename, &size, &info, &city, &state, &hash_by_city)) {
		return EXIT_FAILURE;
	}

	if ((table = calloc(size, sizeof(node*))) == NULL) {
		fprintf(stderr, "%s: Unable to allocate space for hash table\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (load_table(table, size, filename, hash_by_city)) {
		return EXIT_FAILURE;
	}

	if (info) print_info(table, size);

	if (hash_by_city) {
		print_population(table, city, size, hash_by_city);
	} else {
		print_population(table, state, size, hash_by_city);
	}

	delete_table(table, size);

	return EXIT_SUCCESS;
}

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * parse_opts
 *
 * Arguments: argc, argv, pointer to filename, pointer to hash table size, pointer to info flag,
 *            pointer to city name, pointer to state name, pointer to hash_by_city
 *
 * Operation: parses argv for ALL the allowable flags
 *            -i sets info to 1 (0 otherwise)
 *	          -t set the hash table size after converting the arguement to an unsigned long
 *	              >= MIN_TABLE_SIZE
 *            -c copies the city name to city
 *            -s copies the state name to state
 *            copies the name of the file to filename
 * returns:   1 if success and 0 otherwise
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
int parse_opts(
	int argc,
	char* argv[],
	char** filename,
	unsigned long* size,
	int* info,
	char** city,
	char** state,
	int *hash_by_city
) {
	int opt;
	char *endptr;
	int fail = 0;
	extern int errno;

	opterr = 0;
	*filename = NULL;
	*city = NULL;
	*state = NULL;
	while ((opt = getopt(argc, argv, "it:c:s:")) != -1) {
		switch (opt) {
			case 'i':
				*info = 1;
				break;
			case 't':
				errno = 0;
				if (((*size = strtoul(optarg, &endptr, 10)) < MIN_TABLE_SIZE)
					|| (errno != 0)
					|| (*endptr != '\0')
				) {
					fprintf(
						stderr,
						"%s: -t value must be equal or larger than %d\n",
						argv[0],
						MIN_TABLE_SIZE
					);
					fail = 1;
				}
				break;
			case 'c':
				if (*hash_by_city) {
					*city = optarg;
					*hash_by_city = 1;
				} else {
					fprintf(stderr, "%s: Cannot query both a city and a state\n", argv[0]);
					fail = 1;
				}
				break;
			case 's':
				if (*hash_by_city) {
					*state = optarg;
					*hash_by_city = 0;
				} else {
					fprintf(stderr, "%s: Cannot query both a city and a state\n", argv[0]);
					fail = 1;
				}
				break;
			case '?':
				fprintf(stderr, "%s: unknown option -%c\n", argv[0], optopt);
				fail = 1;
				break;
			default:
				fail = 1;
				break;
		}
	}

	*filename = argv[optind];
	if (*filename == NULL) {
		fprintf(stderr, "%s: filename is required\n", argv[0]);
		fail = 1;
	}
	if (*city == NULL && *state == NULL) {
		fprintf(stderr, "%s: -c city or -s state is required\n", argv[0]);
		fail = 1;
	}
	if (fail) {
		fprintf(stderr, "Usage: %s [-i] [-t tablesize] [-c city]/[-s state] filename\n", argv[0]);
	}

	return !fail;
}

/*
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * hash
 *
 * Arguments: a null terminated string
 *
 * Operation: calculates a hash value for the string
 *
 * returns:   the hash value
 * !!! DO NOT EDIT THIS FUNCTION !!!
 */
unsigned long hash(char *str) {
	unsigned long hash = 0;
	unsigned int c;
	while ((c = (unsigned char) *str++) != '\0') {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}

/*
 * node_lookup
 *
 * Arguments: linked list chain pointer head, name of city, name of state, population
 */
node* node_lookup(node* front, char* city, char* state, int pop) {
	// TODO: delete body below and implement function
	// suppresses warnings before implementation
	node* temp = front;
	while(temp != NULL){
	    if((strcmp(city,temp->city) == 0) && (strcmp(state,temp->state)==0)&&(pop == temp->pop)){
	       return temp;
	    }
	    temp = temp->next;
	}
	return NULL;
}

/*
 * add_front
 *
 * Arguments: linked list pointer head, city, state, population
 */
node* add_front(node* front, char* city, char* state, int pop) {
	// TODO: delete body below and implement function
	// suppresses warnings before implementation
	node* temp = malloc(sizeof(node));
	if(temp == NULL){
	  return NULL;
	}
	temp->city = malloc(strlen(city)+1);
	temp->state = malloc(strlen(state)+1);
	strcpy(temp->city,city);
	strcpy(temp->state,state);
	temp->pop = pop;
	temp->next = front;
	front = temp;
	return front;
}

/*
 * print_population
 *
 * Arguments: pointer to hash table, str string, hash table size, hash by city or state
 */
void print_population(node** table, char* str, unsigned long size, int hash_by_city) {
	// TODO: delete body below and implement function
	/*
	 * Use these format strings as given.
	 *
	 * printf("Unable to find any %s by the name of %s.\n", <arg1>, <arg2>);
	 * printf("Minimum: %d\tMaximum: %d\tAverage: %d\n", <arg1>, <arg2>, <arg3>);
	 */
	// TODO: delete suppression below and implement function
	// suppresses warnings before implementation
	int i = (int)hash(str)%size;
	node *temp = table[i];
	int avg = 0;
	int count = 0;
	int min = 0;
	int max = 0;
	while(temp != NULL){
	   if(hash_by_city == 0){
	     if(strcmp(str,temp->state) == 0){
	       avg = avg + temp->pop;
	       ++count;
	       if((count == 1)&&(min == 0)){
	         min = temp->pop;
	       }
	       if(min > temp->pop){
	         min = temp->pop;
	       }
	       if(max < temp->pop){
	         max = temp->pop;
	       }
	     }
	   }
	   else{
	      if(strcmp(str,temp->city) == 0){
	        avg = avg + temp->pop;
		++count;
		if((count == 1) && (min == 0)){
		  min = temp->pop;
		}
                if(min > temp->pop){
		  min = temp->pop;
		}
		if(max < temp->pop){
		  max = temp->pop;
		}
	      }
	   }
	   temp = temp->next;
	}
	if(count == 0){
	  if(hash_by_city == 0){
	    printf("Unable to find any states by the name of %s.\n",str);
	  }
	  else{
	    printf("Unable to find any cities by the name of %s.\n",str);
	  }
	  return;
	}
	else{
	  avg = avg/count;
	  printf("Minimum: %d\tMaximum: %d\tAverage: %d\n",min,max,avg);
	  return;
	}
}

/*
 * load_table
 *
 * Arguments: pointer to hash table, hash table size, file name, hash by city or state
 */
int load_table(node** table, unsigned long size, char* filename, int hash_by_city) {
	// TODO: delete body below and implement function
	/*
	 * Use these format strings as given
	 *
	 * perror("load_table filename open");
	 * perror("load_table malloc");
	 * fprintf(stderr, "load_table duplicate entry: %s %s %d\n", <arg1>, <arg2>, <arg3>);
	 * fprintf(stderr, "load_table could not add %s\n", <arg>);
	 */
	// TODO: delete suppression below and implement function
	// suppresses warnings before implementation
	FILE *f;
	if((f = fopen(filename,"r"))== NULL){
	  perror("load_table filename open");
	  return 1;
	}
	char *strpt;
	char *buffer = malloc(sizeof(char)*LINE_SIZE);
	if(buffer == NULL){
	  perror("load_table malloc");
	  return 1;
	}
	while((strpt = fgets(buffer,LINE_SIZE,f))!= NULL){
	    char *delimiter = ",\n";
	    char *city = strtok(buffer,delimiter);
	   char *state = strtok(NULL,delimiter);
	   char *pops = strtok(NULL,delimiter);
	   node *front;
	   int num;
	   if(hash_by_city == 0){
	     num = (int)hash(state) % size;
	    front = table[num];
	   } 
	   else{
	    num = (int)hash(city) % size;
            front = table[num];
	   }
	   int pop = atoi(pops);
	   node *ptr = node_lookup(front,city,state,pop);
	   if(ptr != NULL){
	     fprintf(stderr,"load_table duplicate entry: %s %s %d\n",ptr->city,ptr->state,ptr->pop);
	     continue;
	   }
	   table[num] = add_front(front,city,state,pop);
	   if(table[num] == NULL){
             if(hash_by_city == 0){
	       fprintf(stderr,"load_table could not add %s\n",state);
             } 
	     else{
	         fprintf(stderr,"load_table could not add %s\n",city);

	     }
	     continue;
	   }

	}
	return 0;
}

/*
 * print_info for debug
 *
 * Arguments: pointer to a hash table, number of elements
 */
void print_info(node **table, unsigned long size) {
	// TODO: delete body below and implement function
	int max = 0;
	int min = 0;
	int count = 0;
	int empty = 0;
	node *temp;
	for(int i = 0; i < (int)size; ++i){
	   temp = table[i];
	   int counter = 0;
	   while(temp != NULL){
	      ++count;
	      ++counter;
	      temp = temp->next;
	   }
	   temp = table[i];
	   if((temp != NULL) && (min == 0)){
	     min = counter;
	   }
	   if((max < counter) && (counter != 0)){
	      max = counter;
	   }
	   if((min > counter) && (counter != 0)){
	      min = counter;
	   }
	   if(counter == 0){
	     ++empty;
	   }
	}
        printf("Table size: %lu\n", size);
	 printf("Total entries: %lu\n", count);
	 printf("Longest chain: %lu\n", max);
	 printf("Shortest chain: %lu\n", min);
	 printf("Empty buckets: %lu\n", empty);
	// TODO: delete suppression below and implement function
	// suppresses warnings before implementation
}

/*
 * delete_table
 *
 * Arguments: pointer to hash table, hash table array size
 */
void delete_table(node **table, unsigned long size) {
	// TODO: delete suppression below and implement function
	// suppresses warnings before implementation
	for(int i = 0; i < (int)size; ++i){
	   node *temp;
	   while(table[i] != NULL){
	       temp = table[i];
	       table[i] = table[i]->next;
	       free(temp->state);
	       temp->state = NULL;
	       free(temp->city);
	       temp->city = NULL;
	       free(temp);
	       temp = NULL;
	   }
	}
	free(table);
	
}
