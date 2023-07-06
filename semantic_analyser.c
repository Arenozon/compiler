#include <stdlib.h>
#include <string.h>
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

struct symbol *create_symbol(symbol_t kind, type_t type, char *name) {
	struct symbol *s = malloc(sizeof(*s));

	s->kind = kind;
	s->type = type;
	s->name = name;

	return s;
}

unsigned int stringHash(char *string)
{
	unsigned int h = 0;
	
	for (int i = 0; i < strlen(string); i++)
	{
		h = 31 * h + string[i];
	}

	return h;
}
