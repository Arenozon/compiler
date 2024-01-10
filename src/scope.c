#include <stdlib.h>
#include <string.h>
#include "symbol.h"

struct stack
{
	struct HashTable *table;
	struct stack *next;
};

struct stack *stack;

void scope_enter()
{
	struct stack *new_entry = malloc(sizeof(struct stack));
	new_entry->table = create_table(100);	// TODO: Adjust table size
	new_entry->next = stack;
	stack = new_entry;
}

void scope_exit()
{
	if (!stack) {
		fprintf(stderr, "Tried to exit a scope, but there is no scope\n");
		exit(EXIT_FAILURE);
	}
	if (stack->table)
		free_table(stack->table);
	struct stack *temp = stack;
	stack = stack->next;
	free(temp);
}

int scope_level()
{
	int level = 0;
	struct stack *current = stack;
	
	if (!current)
		return level;

	level++;
	while (current->next) {
		level++;
		current = current->next;
	}

	return level;
}

// Why is the name included here? Shouldn't the name already be in symbol?
void scope_bind(const char *name, struct symbol *sym)
{
	strcpy(sym->name, name);
	insert_symbol(stack->table, sym);
}

struct symbol *scope_lookup(const char *name)
{
	struct stack *current = stack;
	struct symbol *result;

	if (!current) {
		fprintf(stderr, "Tried to perform a scope lookup, \
			but there is no scope\n");
		exit(EXIT_FAILURE);
	}
		
	while (current) {
		result = ht_search(current->table, name);
		if (result)
			return result;
		current = current->next;
	}

	return NULL;
}

struct symbol *scope_lookup_current(const char *name)
{
	if (!stack) {
		fprintf(stderr, "Tried to perform a scope lookup, \
			but there is no scope\n");
		exit(EXIT_FAILURE);
	}

	return ht_search(stack->table, name);
}
