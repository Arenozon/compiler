void scope_enter();
void scope_bind(char *name, struct symbol *symbol);
int scope_level();
struct symbol *scope_lookup(char *name);
