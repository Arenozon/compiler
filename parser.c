#include <stdlib.h>
#include <string.h>
#include "compiler.h"

static int parse_statements(struct token_stream *stream, struct stmt_s *stmts);
static int parse_statement(struct token_stream *stream, struct stmt_s *stmt);
static int parse_return(struct token_stream *stream, struct ret *ret);
static int parse_loop(struct token_stream *stream, struct loop *loop);
static int parse_conditional(struct token_stream *stream, struct if_stmt *if_stmt);
static int parse_condition(struct token_stream *stream, struct cond *cond);
static int parse_logic(struct token_stream *stream, struct cond *cond);
static int parse_assignment(struct token_stream *stream, 
							struct assign *assign);
static int parse_declaration(struct token_stream *ts, struct decl *decl);
static int parse_definition(struct token_stream *stream, struct def *def);
static int parse_parameter(struct token_stream *stream, struct param *param);
static int parse_parameters(struct token_stream *stream, struct param *params);
static int parse_block(struct token_stream *stream, struct stmt_s *stmt);
static int parse_E(struct token_stream *stream, struct expr *expr);
static int parse_T(struct token_stream *stream, struct term *term);
static int parse_E_prime(struct token_stream *stream, struct expr_p *expr);
static int parse_F(struct token_stream *stream, union factor *fac);
static int parse_call(struct token_stream *stream, struct call *call);
static int parse_arguments(struct token_stream *stream, struct arg *args);
static int parse_T_prime(struct token_stream *stream, struct term_p *term);

static int expect_optional_token(struct token_stream *ts, token_t expected_type) {
	struct token *new_token = read_token(ts);

	if (new_token->token_type == expected_type) {
		free(new_token);
		return 1;
	}
	else {
		putback_token(ts, new_token);
		return 0;
	}
}

static int expect_mandatory_token(struct token_stream *ts, token_t expected_type) {
	struct token *new_token = read_token(ts);

	if (new_token->token_type == expected_type) {
		free(new_token);
		return 1;
	}
	else {
		fprintf(stderr, "Parse error: unexpected token on line %d\n", 
				new_token->line);
		putback_token(ts, new_token);
		return 0;
	}
}

int parse_P(struct token_stream *ts) {
	struct prog *prog = create_prog(malloc(sizeof(struct stmt_s*)));
	
	return parse_statements(ts, prog->stmts) && expect_mandatory_token(ts, TOKEN_EOF);
}

static int parse_statements(struct token_stream *ts, struct stmt_s *curr) {
	token_t t_type;
	int parsed;
	struct token *t;

	if ((parsed = parse_statement(ts, curr))) {
		t = read_token(ts);
		t_type = t->token_type;
		while ((t_type != TOKEN_EOF) && (t_type != TOKEN_RBRACE) && parsed) {
			putback_token(ts, t);
			curr->next = malloc(sizeof(struct stmt_s*));
			curr = curr->next;
			parsed = parse_statement(ts, curr);
			t = read_token(ts);
			t_type = t->token_type;
		}
		putback_token(ts, t);

		return parsed;
	}

	return 0;
}

int parse_statement(struct token_stream *ts, struct stmt_s *stmt) {
	struct token *t = read_token(ts);
	token_t t_type = t->token_type;

	if (t_type == TOKEN_TYPE) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_DECL, malloc(sizeof(struct decl*)), NULL);

		return parse_declaration(ts, stmt->stmt->decl);
	} else if (t_type == TOKEN_ID) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_ASSIGN, malloc(sizeof(struct assign*)), NULL);

		return parse_assignment(ts, stmt->stmt->assign);
	} else if (t_type == TOKEN_FUNC) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_DEF, malloc(sizeof(struct def*)), NULL);

		return parse_definition(ts, stmt->stmt->def);
	} else if (t_type == TOKEN_IF) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_COND, malloc(sizeof(struct if_stmt*)), NULL);

		return parse_conditional(ts, stmt->stmt->if_stmt);
	} else if (t_type == TOKEN_WHILE) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_LOOP, malloc(sizeof(struct loop*)), NULL);

		return parse_loop(ts, stmt->stmt->loop);
	} else if (t_type == TOKEN_LPAREN || t_type == TOKEN_ID
	|| t_type == TOKEN_INT || t_type == TOKEN_CALL) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_EXPR, malloc(sizeof(struct expr*)), NULL);

		if (!parse_E(ts, stmt->stmt->expr))
			return 0;
		if (!expect_mandatory_token(ts, TOKEN_SEMI_COLON)) {
			fprintf(stderr, "Parse error in parse_statement\n");

			return 0;
		}

		return 1;
	} else if (t_type == TOKEN_RET) {
		putback_token(ts, t);
		stmt = create_stmt(STMT_RET, malloc(sizeof(struct ret*)), NULL);

		return parse_return(ts, stmt->stmt->ret);
	} else {
		fprintf(stderr, "Parse error in parse_statement: unexpected token on line %d\n", t->line);
		free(t);
		
		return 0;
	}
}

static int parse_return(struct token_stream *ts, struct ret *ret) {
	ret = create_ret(NULL);

	if (!expect_mandatory_token(ts, TOKEN_RET)) {
		fprintf(stderr, "Parse error in parse_return: expected \'return\'\n");
		
		return 0;
	}
	if (!parse_E(ts, ret->expr)) {
		return 0;
	}
	if (!expect_mandatory_token(ts, TOKEN_SEMI_COLON)) {
		fprintf(stderr, "Parse error in parse_return: expected a semi colon\n");

		return 0;
	}

	return 1;
}

static int parse_loop(struct token_stream *ts, struct loop *loop) {
	loop = create_loop(NULL, NULL);
	if (!expect_mandatory_token(ts, TOKEN_WHILE)) {
		fprintf(stderr, "Parse error in parse_loop: expected \'while\'\n");

		return 0;
	}
	if (!expect_mandatory_token(ts, TOKEN_LPAREN)) {
		fprintf(stderr, "Parse error in parse_loop: expected \'(\'\n");

		return 0;
	}
	if (!parse_condition(ts, loop->cond))
		return 0;
	if (!expect_mandatory_token(ts, TOKEN_RPAREN)) {
		fprintf(stderr, "Parse error in parse_loop: expected \')\'\n");

		return 0;
	}

	return parse_block(ts, loop->block);
}

static int parse_conditional(struct token_stream *ts, struct if_stmt *if_stmt) {
	struct token *t;
	token_t t_type;
	int parsed;
	struct elif_stmt *curr;

	if ((parsed = (expect_mandatory_token(ts, TOKEN_IF) && expect_mandatory_token(ts, TOKEN_LPAREN) && parse_condition(ts, if_stmt->cond) && expect_mandatory_token(ts, TOKEN_RPAREN) && parse_block(ts, if_stmt->block)))) {
		curr = if_stmt->elif_stmt;
		curr = malloc(sizeof(*curr));
		while ((t_type = ((t = read_token(ts))->token_type)) == TOKEN_ELIF && parsed) {
			parsed = (expect_mandatory_token(ts, TOKEN_LPAREN) && parse_condition(ts, curr->cond) 
				&& expect_mandatory_token(ts, TOKEN_RPAREN)) && parse_block(ts, curr->block);
			curr = curr->next;
			curr = malloc(sizeof(*curr));
			free(t);
		}
		if (t_type == TOKEN_ELSE && parsed) {
			parsed = parse_block(ts, if_stmt->else_block);
			free(t);
		} else {
			free(t);

			putback_token(ts, t);
		}

		return parsed;
	}

	return 0;
}

static int parse_condition(struct token_stream *ts, struct cond *cond) {
	return parse_E(ts, cond->expr1) && parse_logic(ts, cond) && parse_E(ts, cond->expr2);
}

static int parse_logic(struct token_stream *ts, struct cond *cond) {
	struct token *t = read_token(ts);
	token_t t_type = t->token_type;

	switch(t_type) {
		case TOKEN_AND:
			cond->op = OP_AND;
			break;
		case TOKEN_OR:
			cond->op = OP_OR;
			break;
		case TOKEN_EQUAL:
			cond->op = OP_EQUAL;
			break;
		case TOKEN_NOT_EQUAL:
			cond->op = OP_NOT_EQUAL;
			break;
		case TOKEN_GT:
			cond->op = OP_GT;
			break;
		case TOKEN_LT:
			cond->op = OP_LT;
			break;
		case TOKEN_GE:
			cond->op = OP_GE;
			break;
		case TOKEN_LE:
			cond->op = OP_LE;
			break;
		default:
			return 0;
	}
	free(t);

	return 1;
}

static int parse_declaration(struct token_stream *ts, struct decl *decl) {
	char *name;
	type_t type;

	struct token *t = read_token(ts);
	if (t->token_type == TOKEN_TYPE) {
		type = t->type;
		free(t);
	} else {
		free(t);

		return 0;
	}

	t = read_token(ts);
	if (t->token_type == TOKEN_ID) {
		name = malloc(sizeof(*(t->name)));
		*name = *(t->name);
		free(t);
	} else {
		free(t);

		return 0;
	}

	decl = create_decl(name, type);
	free(name);

	return expect_mandatory_token(ts, TOKEN_SEMI_COLON);
}

int parse_assignment(struct token_stream *ts, struct assign *assign) {
	char *name;

	struct token *t = read_token(ts);
	if (t->token_type == TOKEN_ID) {
		name = malloc(sizeof(*(t->name)));
		*name = *(t->name);
	} else {
		free(t);

		return 0;
	}

	assign = create_assign(name, malloc(sizeof(struct expr*)));
	free(name);
	free(t);
	
	return expect_mandatory_token(ts, TOKEN_ASSIGN) && parse_E(ts, assign->expr) && expect_mandatory_token(ts, TOKEN_SEMI_COLON);
}

int parse_definition(struct token_stream *ts, struct def *def) {
	type_t type;
	struct token *t;
	char *name;

	if (!expect_mandatory_token(ts, TOKEN_FUNC))
		return 0;

	t = read_token(ts);
	if (t->token_type == TOKEN_TYPE) {
		type = t->type;
		free(t);
	} else {
		free(t);

		return 0;
	}

	t = read_token(ts);
	if (t->token_type == TOKEN_ID) {
		name = malloc(sizeof(*(t->name)));
		*name = *(t->name);
		free(t);
	} else {
		free(t);

		return 0;
	}

	def = create_def(type, name, malloc(sizeof(struct param*)), malloc(sizeof(struct stmt_s*)));
	
	return expect_mandatory_token(ts, TOKEN_LPAREN) && parse_parameters(ts, def->params) 
	&& expect_mandatory_token(ts, TOKEN_RPAREN) && parse_block(ts, def->block);
}

int parse_parameter(struct token_stream *ts, struct param *param) {
	char *name;
	type_t type;
	struct token *t = read_token(ts);
	
	if (t->token_type == TOKEN_TYPE) {
		type = t->type;
		free(t);
	} else {
		free(t);

		return 0;
	}

	t = read_token(ts);
	if (t->token_type == TOKEN_ID) {
		name = malloc(sizeof(*(t->name)));
		*name = *(t->name);
		free(t);
	} else {
		free(t);

		return 0;
	}

	param = create_param(type, name, NULL);
	free(name);

	return 1;
}

int parse_parameters(struct token_stream *ts, struct param *params) {
	int parsed;
	struct param *curr;
	struct token *t = read_token(ts);

	if (t->token_type == TOKEN_TYPE) {
		putback_token(ts, t);
		if ((parsed = parse_parameter(ts, params))) {
			t = read_token(ts);
			curr = params->next;
			curr = malloc(sizeof(*curr));
			while (t->token_type == TOKEN_COMMA && parsed) {
				free(t);
				parsed = parse_parameter(ts, curr);
				t = read_token(ts);
				curr = curr->next;
				curr = malloc(sizeof(*curr));
			}
			putback_token(ts, t);

			return parsed;
		}

		return 0;
	} else {
		putback_token(ts, t);

		return 1;
	}
}

int parse_block(struct token_stream *ts, struct stmt_s *block) {
	return expect_mandatory_token(ts, TOKEN_LBRACE) && parse_statements(ts, block) && expect_mandatory_token(ts, TOKEN_RBRACE);
}

int parse_E(struct token_stream *ts, struct expr *expr) {
	return parse_T(ts, expr->term) && parse_E_prime(ts, expr->next);
}

int parse_T(struct token_stream *ts, struct term *term) {
	return parse_F(ts, term->fac) && parse_T_prime(ts, term->next);
}

int parse_E_prime(struct token_stream *ts, struct expr_p *expr) {
	struct token *t = read_token(ts);

	switch(t->token_type) {
		case TOKEN_PLUS:
			expr->op = OP_ADD;
			return parse_T(ts, expr->term) && parse_E_prime(ts, expr->next);
		case TOKEN_SUBTRACT:
			expr->op = OP_SUB;
			return parse_T(ts, expr->term) && parse_E_prime(ts, expr->next);
		default:
			putback_token(ts, t);
			
			return 0;
		
	}
}

int parse_F(struct token_stream *ts, union factor *fac) {
	struct token *t = read_token(ts);

	switch(t->token_type) {
		case TOKEN_LPAREN:
			free(t);

			return parse_E(ts, fac->expr) && expect_mandatory_token(ts, TOKEN_RPAREN);
		case TOKEN_INT:
			fac->num = t->value;
			free(t);
			
			return 1;
		case TOKEN_ID:
			fac->id = malloc(sizeof(*(t->name)));
			*(fac->id) = *(t->name);
			free(t);
			
			return 1;
		case TOKEN_CALL:
			fac->func = malloc(sizeof(*(fac->func)));
			putback_token(ts, t);

			return parse_call(ts, fac->func);
		default:
			fprintf(stderr, "Parse error: unexpected token on line %d\n", t->line);
			free(t);
			
			return 0;
	}
}

int parse_call(struct token_stream *ts, struct call *func) {
	char *name;
	struct token *t;

	if (!expect_mandatory_token(ts, TOKEN_CALL)) {
		fprintf(stderr, "Parse error in parse_call\n");
		free(t);

		return 0;
	}

	t = read_token(ts);
	if (t->token_type == TOKEN_ID) {
		name = malloc(sizeof(t->name));
		*name = *(t->name);
		free(t);
	} else {
		fprintf(stderr, "Parse error: unexpected token on line %d\n", t->line);
		free(t);

		return 0;
	}

	return expect_mandatory_token(ts, TOKEN_LPAREN) && parse_arguments(ts, func->args) && expect_mandatory_token(ts, TOKEN_RPAREN);
}

int parse_arguments(struct token_stream *ts, struct arg *args) {
	int parsed;
	char *name;
	struct arg *curr;
	struct token *t = read_token(ts);

	parsed = (t->token_type == TOKEN_ID);
	if (parsed) {
		name = malloc(sizeof(*(t->name)));
		*name = *(t->name);
		args = create_arg(name, NULL);
		free(name);
		t = read_token(ts);
		curr = args->next;
		while (t->token_type == TOKEN_COMMA && parsed) {
			t = read_token(ts);
			if (t->token_type == TOKEN_ID) {
				name = malloc(sizeof(*(t->name)));
				*name = *(t->name);
				curr = create_arg(name, NULL);
				curr = curr->next;
				free(name);
				free(t);
				parsed = 1;
			} else {
				putback_token(ts, t);

				return 0;
			}
		}
		putback_token(ts, t);

		return parsed;
	}
	putback_token(ts, t);

	return 1;
}

int parse_T_prime(struct token_stream *ts, struct term_p *term) {
	struct token *t = read_token(ts);

	switch(t->token_type) {
		case TOKEN_MULTIPLY:
			term = create_term_p(OP_MULT, NULL, NULL);
			free(t);
			return parse_F(ts, term->fac) && parse_T_prime(ts, term->next);
		case TOKEN_DIVIDE:
			term = create_term_p(OP_DIV, NULL, NULL);
			free(t);
			return parse_F(ts, term->fac) && parse_T_prime(ts, term->next);
		default:
			putback_token(ts, t);

			return 1;
	}
}
