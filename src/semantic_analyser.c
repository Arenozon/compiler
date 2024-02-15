#include <stdlib.h>
#include "compiler.h"
#include "symbol.h"
#include "scope.h"

void expr_resolve(struct expr *e);
void term_p_resolve(struct term_p *t);
void stmt_resolve(struct stmt_s *s);

void decl_resolve(struct decl *d)
{
	if (!d) return;

	symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

	d->symbol = create_symbol(kind, d->type, d->name);

	scope_bind(d->name, d->symbol);
}

void func_resolve()
{
	// Placeholder, still have to implement function symbol table
}

void factor_resolve(struct factor *f)
{
	if (!f) return;

	if (f->expr) {
		expr_resolve(f->expr);
		return;
	}
	if (f->id) {
		struct symbol *s;
		if((s = scope_lookup(f->id)))
			f->symbol = s;
		else {
			fprintf(stderr, "Uninitialised variable used: %s\n", f->id);
			exit(EXIT_FAILURE);
		}
		return;
	}
	if (f->func) 
		func_resolve(f->func);
	if (f->num)
		return;
	else
		fprintf(stderr, "Empty factor\n");
}

void term_resolve(struct term *t)
{
	if (!t) return;

	factor_resolve(t->fac);
	term_p_resolve(t->next);
}

void term_p_resolve(struct term_p *t)
{
	if (!t) return;

	factor_resolve(t->fac);
	term_p_resolve(t->next);
}

void expr_p_resolve(struct expr_p *e)
{
	if (!e) return;

	term_resolve(e->term);
	expr_p_resolve(e->next);
}

void expr_resolve(struct expr *e)
{
	if (!e) return;

	term_resolve(e->term);
	expr_p_resolve(e->next);
}

void assign_resolve(struct assign *a)
{
	if (!a) return;

	expr_resolve(a->expr);
}

void def_resolve(struct def *d)
{
	if (!d) return;

	// Placeholder for function lookup
}

void cond_resolve(struct cond *c)
{
	if (!c) return;

	expr_resolve(c->expr1);
	expr_resolve(c->expr2);
}

void elif_resolve(struct elif_stmt *e)
{
	if (!e) return;

	cond_resolve(e->cond);
	stmt_resolve(e->block);
	elif_resolve(e->next);
}

void if_resolve(struct if_stmt *i)
{
	if (!i) return;

	cond_resolve(i->cond);
	stmt_resolve(i->block);
	elif_resolve(i->elif_stmt);
	stmt_resolve(i->else_block);
}

void loop_resolve(struct loop *l)
{
	if (!l) return;

	cond_resolve(l->cond);
	stmt_resolve(l->block);
}

void ret_resolve(struct ret *r)
{
	if (!r) return;

	expr_resolve(r->expr);
}

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
			if_resolve(s->if_stmt);
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
