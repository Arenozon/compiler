#include <stdlib.h>
#include <string.h>
#include "compiler.h"

// Creates a token stream and initialises the fields to NULL
struct token_stream *init_stream() {
	struct token_stream *ts = malloc(sizeof(*ts));
	ts->token = NULL;
	ts->next = NULL;

	return ts;
}

// Creates a token and initialises its fields to default values (0 or NULL)
static struct token *create_token() {
	struct token *t = malloc(sizeof(struct token));
	t->type = 0;
	t->line = 0;
	t->name = NULL;
	t->value = 0;
	t->token_type = 0;

	return t;
}

// Enter a token into the token stream
static void enter_token(struct token_stream **bot, struct token *token) {
	if ((*bot)->token == NULL) {
		(*bot)->token = token;
		return;
	}
	struct token_stream *new = malloc(sizeof(struct token_stream));
	new->token = token;
	new->next = NULL;
	(*bot)->next = new;
	(*bot) = new;
}

static int isNumber(char c) {
	return (c >= 48 && c <= 57);
}

static int isLetter(char c) {
	return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));
}

void free_token(struct token *t) {
	if (t->name) {
		free(t->name);
	}
	free(t);
}

/* Goes through the source file and converts the characters into tokens. 
 * Populates the token stream with said tokens. */
int scan_tokens(FILE *fp, struct token_stream **ts) {
	char c;
	struct token *t;
	int line = 1;

next_token:
	while(1) {
		t = create_token();
		do {
			t->line = line;
			c = fgetc(fp);
			switch(c) {
				case '\n':
					line++;
					break;
				case '*':
					t->token_type = TOKEN_MULTIPLY;
					enter_token(ts, t);
					goto next_token;
				case '+':
					t->token_type = TOKEN_PLUS;
					enter_token(ts, t);
					goto next_token;
				case '-':
					t->token_type = TOKEN_SUBTRACT;
					enter_token(ts, t);
					goto next_token;
				case '/':
					t->token_type = TOKEN_DIVIDE;
					enter_token(ts, t);
					goto next_token;
				case ';':
					t->token_type = TOKEN_SEMI_COLON;
					enter_token(ts, t);
					goto next_token;
				case '(':
					t->token_type = TOKEN_LPAREN;
					enter_token(ts, t);
					goto next_token;
				case ')':
					t->token_type = TOKEN_RPAREN;
					enter_token(ts, t);
					goto next_token;
				case '{':
					t->token_type = TOKEN_LBRACE;
					enter_token(ts, t);
					goto next_token;
				case '}':
					t->token_type = TOKEN_RBRACE;
					enter_token(ts, t);
					goto next_token;
				case '=':
					if ((c = fgetc(fp)) == '=') {
						t->token_type = TOKEN_EQUAL;
					} else {
						ungetc(c, fp);
						t->token_type = TOKEN_ASSIGN;
					}
					enter_token(ts, t);
					goto next_token;
				case ',':
					t->token_type = TOKEN_COMMA;
					enter_token(ts, t);
					goto next_token;
				case '&':
					if (fgetc(fp) == '&') {
						t->token_type = TOKEN_AND;
						enter_token(ts, t);
						goto next_token;
					} else { 
						t->token_type = ERROR;
						enter_token(ts, t);
						return 1;
					}
				case '|':
					if (fgetc(fp) == '|') {
						t->token_type = TOKEN_OR;
						enter_token(ts, t);
						goto next_token;
					} else {
						t->token_type = ERROR;
						return 1;
					}
				case '!':
					if (fgetc(fp) == '=') {
						t->token_type = TOKEN_NOT_EQUAL;
					} else {
						ungetc(c, fp);
						t->token_type = TOKEN_NOT;
					}
					enter_token(ts, t);
					goto next_token;
				case '>':
					if ((c = fgetc(fp)) == '=') {
						t->token_type = TOKEN_GE;
					} else {
						ungetc(c, fp);
						t->token_type = TOKEN_GT;
					}
					enter_token(ts, t);
					goto next_token;
				case '<':
					if ((c = fgetc(fp)) == '=') {
						t->token_type = TOKEN_LE;
					} else {
						ungetc(c, fp);
						t->token_type = TOKEN_LT;
					}
					enter_token(ts, t);
					goto next_token;
				case '$':
					t->token_type = TOKEN_CALL;
					enter_token(ts, t);
					goto next_token;
				case EOF:
					t->token_type = TOKEN_EOF;
					enter_token(ts, t);
					return 0;
			}

			// For numbers
			if (isNumber(c)) {
				int value = 0;

				do {
					value = value * 10 + (c - '0');
					c = fgetc(fp);
				} while (isNumber(c));
				ungetc(c, fp);
				t->token_type = TOKEN_INT;
				t->value = value;
				enter_token(ts, t);
				goto next_token;
			}

			// For keywords and identifiers
			if (isLetter(c)) {
				char *word = malloc(255);
				int i = 0;

				do {
					word[i] = c;
					i++;
					c = fgetc(fp);
				} while (isLetter(c) || isNumber(c));

				word[i] = '\0';
				ungetc(c, fp);

				if (strcmp(word, "i64") == 0) {
					free(word);
					t->token_type = TOKEN_TYPE;
					t->type = TYPE_I64;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "func") == 0) {
					free(word);
					t->token_type = TOKEN_FUNC;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "if") == 0) {
					free(word);
					t->token_type =TOKEN_IF;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "elif") == 0) {
					free(word);
					t->token_type = TOKEN_ELIF;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "else") == 0) {
					free(word);
					t->token_type = TOKEN_ELSE;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "while") == 0) {
					free(word);
					t->token_type = TOKEN_WHILE;
					enter_token(ts, t);
					goto next_token;
				} else if (strcmp(word, "return") == 0) {
					free(word);
					t->token_type = TOKEN_RET;
					enter_token(ts, t);
					goto next_token;
				} else {
					t->token_type = TOKEN_ID;
					t->name = word;
					enter_token(ts, t);
					goto next_token;
				}
			}
		} while (c == ' ' || c == '\n' || c == '\t');
		
		t->token_type = ERROR;
		enter_token(ts, t);
		return 0;
	}
}

struct token *read_token(struct token_stream **top) {
	struct token *token = (*top)->token;
	(*top) = (*top)->next;

	return token;
}

// Put a token back onto the top of the token stream
void putback_token(struct token_stream **top, struct token *token) {
	struct token_stream *new = malloc(sizeof(struct token_stream));
	struct token_stream *temp = *top;
	new->token = token;
	*top = new;
	(*top)->next = temp;
}

// Print the token stream to console
void print_stream(struct token_stream *ts) {
	struct token_stream *temp = ts;

	while (temp->token->token_type != TOKEN_EOF) {
		switch(temp->token->token_type) {
			case TOKEN_INT:
				printf("Token type: %d\nValue: %d\nLine: %d\n", temp->token->token_type, temp->token->value, temp->token->line);
				break;
			case TOKEN_ID:
				printf("Token type: %d\nName: %s\nLine: %d\n", temp->token->token_type, temp->token->name, temp->token->line);
				break;
			case TOKEN_TYPE:
				printf("Token type: %d\nType: %d\nLine: %d\n", temp->token->token_type, temp->token->type, temp->token->line);
				break;
			default:
				printf("Token type: %d\nLine: %d\n", temp->token->token_type, temp->token->line);
				break;
		}
		temp = temp->next;
	}
	printf("Token type: %d\nLine: %d\n", temp->token->token_type, temp->token->line);
}
