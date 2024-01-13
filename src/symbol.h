#ifndef SYMBOL_H
#define SYMBOL_H

#include "compiler.h"

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

struct symbol {
	symbol_t kind;
	type_t type;
	char *name;
	int which;
};

struct HashTable {
	struct LinkedList **symbols;
	int size;
	int count;
};

struct symbol *create_symbol(symbol_t kind, type_t type, char *name);
struct HashTable *create_table(int size);
void free_table(struct HashTable *table);
void insert_symbol(struct HashTable *table, struct symbol *symbol);
struct symbol *ht_search(struct HashTable *table, const char *name);

#endif
