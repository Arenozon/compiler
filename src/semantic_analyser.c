#include <stdlib.h>
#include "compiler.h"
#include "symbol.h"
#include "scope.h"

void decl_resolve(struct decl *d)
{
	if (!d) return;

	symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

	d->symbol = create_symbol(kind, d->type, d->name);

	scope_bind(d->name, d->symbol);
}

void assign_resolve(struct assign *a)
{
	if (!a) return;


}

void def_resolve(struct def *d);

void cond_resolve(struct if_stmt *c);

void loop_resolve(struct loop *l);

void expr_resolve(struct expr *e);

void ret_resolve(struct ret *r);

void stmt_resolve(struct stmt_s *s)
{
	if (!s) return;

	switch (s->kind) {
		case STMT_DECL:
			decl_resolve(s->decl);
			break;

		case STMT_ASSIGN:
			assign_resolve(s->assign);
			break;

		case STMT_DEF:
			def_resolve(s->def);
			break;

		case STMT_COND:
			cond_resolve(s->if_stmt);
			break;

		case STMT_LOOP:
			loop_resolve(s->loop);
			break;

		case STMT_EXPR:
			expr_resolve(s->expr);
			break;

		case STMT_RET:
			ret_resolve(s->ret);
			break;

		default:
			fprintf(stderr, "stmt_resolve: Unexpected statement kind %d\n", 
				s->kind);
			exit(EXIT_FAILURE);
	}

	if (s->next)
		stmt_resolve(s->next);
}

void prog_resolve(struct prog *p)
{
	if (!p) return;

	scope_enter();
	stmt_resolve(p->stmts);
}
