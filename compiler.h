#include <stdio.h>

typedef enum {
	TOKEN_MULTIPLY,
	TOKEN_PLUS,
	TOKEN_SUBTRACT,
	TOKEN_DIVIDE,
	TOKEN_INT,
	TOKEN_ID,
	TOKEN_TYPE,
	TOKEN_FUNC,
	TOKEN_SEMI_COLON,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_ASSIGN,
	TOKEN_COMMA,
	TOKEN_RET,
	TOKEN_IF,
	TOKEN_ELIF,
	TOKEN_ELSE,
	TOKEN_WHILE,
	TOKEN_LOGIC,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_EQUAL,
	TOKEN_NOT_EQUAL,
	TOKEN_GT,
	TOKEN_LT,
	TOKEN_GE,
	TOKEN_LE,
	TOKEN_NOT,
	TOKEN_CALL,
	TOKEN_EOF,
	ERROR
} token_t;

typedef enum {
	TYPE_I64
} type_t;

struct token {
	token_t token_type;
	int line;
	char *name;
	int value;
	type_t type;
};

struct token_stream {
	struct token *token;
	struct token_stream *next;
};

typedef enum {
	STMT_DECL,
	STMT_ASSIGN,
	STMT_DEF,
	STMT_COND,
	STMT_LOOP,
	STMT_EXPR,
	STMT_RET
} stmt_t;

typedef enum {
	OP_ADD,
	OP_SUB,
	OP_MULT,
	OP_DIV,
	OP_AND,
	OP_OR,
	OP_EQUAL,
	OP_NOT_EQUAL,
	OP_GT,
	OP_LT,
	OP_GE,
	OP_LE
} oper_t;

union stmt_u {
	struct decl *decl;
	struct assign *assign;
	struct def *def;
	struct if_stmt *if_stmt;
	struct loop *loop;
	struct expr *expr;
	struct ret *ret;
};

struct prog {
	struct stmt_s *stmts;
};

struct stmt_s {
	stmt_t kind;
	union stmt_u *stmt;
	struct stmt_s *next;
};

struct decl {
	char *name;
	type_t type;
};

struct assign {
	char *name;
	struct expr *expr;
};

struct def {
	type_t type;
	char *name;
	struct param *params;
	struct stmt_s *block;
};

struct loop {
	struct cond *cond;
	struct stmt_s *block;
};

struct if_stmt {
	struct cond *cond;
	struct stmt_s *block;
	struct elif_stmt *elif_stmt;
	struct stmt_s *else_block;
};

struct elif_stmt {
	struct cond *cond;
	struct stmt_s *block;
	struct elif_stmt *next;
};

struct ret {
	struct expr *expr;
};

struct cond {
	struct expr *expr1;
	oper_t op;
	struct expr *expr2;
};

struct param {
	type_t type;
	char *id;
	struct param *next;
};

struct expr {
	struct term *term;
	struct expr_p *next;
};

struct expr_p {
	oper_t op;
	struct term *term;
	struct expr_p *next;
};

union factor {
	struct expr *expr;
	char *id;
	int num;
	struct call *func;
};

struct term {
	union factor *fac;
	struct term_p *next;
};

struct term_p {
	oper_t op;
	union factor *fac;
	struct term_p *next;
};

struct call {
	char *name;
	struct arg *args;
};

struct arg {
	char *id;
	struct arg *next;
};

struct token_stream *init_stream();
struct token *read_token(struct token_stream *top);
void putback_token(struct token_stream *top, 
		   struct token *token);
int scan_tokens(FILE *fp, struct token_stream *ts);
int parse_P(struct token_stream *stream);

struct prog *create_prog(struct stmt_s *stmts);
struct stmt_s *create_stmt(stmt_t kind, union stmt_u *stmt, 
			   struct stmt_s *next);
struct decl *create_decl(char *name, type_t type);
struct assign *create_assign(char *name, struct expr *expr);
struct def *create_def(type_t type, char *name, 
		       struct param *params, 
		       struct stmt_s *block);
struct loop *create_loop(struct cond *cond, 
			 struct stmt_s *block);
struct if_stmt *create_if(struct cond *cond, 
			  struct stmt_s *block, 
			  struct elif_stmt *elif_stmt, 
			  struct stmt_s *else_block);
struct expr *create_expr(struct term *term, struct expr_p *next);
struct expr_p *create_expr_p(oper_t op, struct term *term, 
			     struct expr_p *next);
struct param *create_param(type_t type, char *name, struct param *next);
struct arg *create_arg(char *name, struct arg *next);
struct term_p *create_term_p(oper_t op, union factor *fac, struct term_p *next);
struct ret *create_ret(struct expr *expr);
