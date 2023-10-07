#include <stdlib.h>
#include <string.h>
#include "semantic_analyser.h"

// This is an implementation of a hash function I found on Wikipedia, allegedly 31 was used by K & R
// Takes in a string and converts it to an unsigned int, with the hopes that most strings will produce unique hashes
static unsigned int stringHash(char *string, int size)
{
	unsigned int h = 0;
	
	for (int i = 0; i < strlen(string); i++)
		h = (31 * h + string[i]) % size;

	return h;
}

struct symbol *create_symbol(symbol_t kind, type_t type, char *name) {
	struct symbol *s = malloc(sizeof(*s));

	s->kind = kind;
	s->type = type;
	s->name = name;

	return s;
}

struct HashTable *create_table(int size)
{
	struct HashTable *table = malloc(sizeof(*table));
	table->size = size;
	table->count = 0;
	table->symbols = calloc(table->size, sizeof(struct symbol*));

	return table;
}

// Placeholder collision handler
static void handle_collision(struct HashTable *table, struct symbol *symbol)
{

}

void insert_symbol(struct HashTable *table, struct symbol *symbol)
{
	int index = stringHash(symbol->name, table->size);
	struct symbol *current_symbol = table->symbols[index];

	if (current_symbol == NULL) 
	{
		if (table->count == table->size)
		{
			printf("Insert Error: Hash Table is full\n");
			
			return;
		}

		table->symbols[index] = symbol;
		table->count++;
	}
	else if (strcmp(current_symbol->name, symbol->name) == 0) 
	{
		table->symbols[index] = symbol;

		return;
	}
	else {
		handle_collision(table, symbol);

		return;
	}
}


struct symbol *ht_search(struct HashTable *table, char *name)
{
	int index = stringHash(name, table->size);
	struct symbol *symbol = table->symbols[index];

	if (symbol != NULL && strcmp(name, symbol->name) == 0)
		return symbol;

	return NULL;
}

// Frees a symbol
void free_symbol(struct symbol *symbol)
{
	free(symbol->name);
	free(symbol);
}

// Frees a hash table
void free_table(struct HashTable *table)
{
	for (int i = 0; i < table->size; i++)
	{
		if (table->symbols[i] != NULL)
			free_symbol(table->symbols[i]);
	}

	free(table->symbols);
	free(table);
}

// Prints the contents of a hash table
void print_table(struct HashTable *table)
{
	printf("\nHash table\n--------------------\n");

	for (int i = 0; i < table->size; i++)
	{
		if (table->symbols[i])
			printf("Hash: %d\tName: %s\tType: %d\n", i, table->symbols[i]->name, table->symbols[i]->type);
	}

	printf("-------------------\n");
}

