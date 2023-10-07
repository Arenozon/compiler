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
	struct symbol **symbols;
	int size;
	int count;
};
