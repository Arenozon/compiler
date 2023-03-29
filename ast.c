#include <stdlib.h>
#include "compiler.h"

struct prog *create_prog(struct stmt_s *stmts) {
	struct prog *prog = malloc(sizeof(*prog));

	prog->stmts = stmts;

	return prog;
}

struct stmt_s *create_stmt(stmt_t kind, union stmt_u *stmt, 
			struct stmt_s *next) {
	struct stmt_s *s = malloc(sizeof(*s));
	
	s->kind = kind;
	s->stmt = stmt;
	s->next = next;

	return s;
}

struct decl *create_decl(char *name, type_t type) {
	struct decl *d = malloc(sizeof(*d));

	d->name = name;
	d->type = type;

	return d;
}

struct assign *create_assign(char *name, struct expr *expr) {
	struct assign *a = malloc(sizeof(*a));

	a->name = name;
	a->expr = expr;

	return a;
}

struct def *create_def(type_t type, char *name, 
		       struct param *params, 
		       struct stmt_s *block) {
	struct def *d = malloc(sizeof(*d));

	d->type = type;
	d->name = name;
	d->params = params;
	d->block = block;

	return d;
}

struct loop *create_loop(struct cond *cond, 
			 struct stmt_s *block) {
	struct loop *l = malloc(sizeof(*l));

	l->cond = cond;
	l->block = block;

	return l;
}

struct if_stmt *create_if(struct cond *cond, 
			  struct stmt_s *block, 
			  struct elif_stmt *elif_stmt, 
			  struct stmt_s *else_block) {
	struct if_stmt *i = malloc(sizeof(*i));

	i->cond = cond;
	i->block = block;
	i->elif_stmt = elif_stmt;
	i->else_block = else_block;

	return i;
}

struct expr *create_expr(struct term *term, struct expr_p *next) {
	struct expr *e = malloc(sizeof(*e));

	e->term = term;
	e->next = next;

	return e;
}

struct expr_p *create_expr_p(oper_t op, struct term *term, 
			     struct expr_p *next) {
	struct expr_p *e = malloc(sizeof(*e));

	e->op = op;
	e->term = term;
	e->next = next;

	return e;
}

struct param *create_param(type_t type, char *name, 
						   struct param *next) {
	struct param *p = malloc(sizeof(*p));

	p->type = type;
	p->id = malloc(sizeof(*name));
	*(p->id) = *name;
	p->next = next;

	return p;
}

struct arg *create_arg(char *name, struct arg *next) {
	struct arg *a = malloc(sizeof(*a));

	a->id = malloc(sizeof(*name));
	*(a->id) = *name;
	a->next = next;

	return a;
}

struct term_p *create_term_p(oper_t op, union factor *fac, struct term_p *next) {
	struct term_p *tp = malloc(sizeof(*tp));

	tp->op = op;
	tp->fac = fac;
	tp->next = next;

	return tp;
}

struct ret *create_ret(struct expr *expr) {
	struct ret *r = malloc(sizeof(*r));

	r->expr = expr;

	return r;
}
