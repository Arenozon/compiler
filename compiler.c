#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	TOKEN_EOF,
	ERROR
} token_t;

token_t scan_token(FILE *fp);
int isNumber(char c);
int isLetter(char c);
int parse_P();
int parse_statements();
int parse_statement();
int parse_loop();
int parse_conditional();
int parse_condition();
int parse_logic();
int parse_assignment();
int parse_declaration();
int parse_definition();
int parse_parameter();
int parse_parameters();
int parse_block();
int parse_E();
int parse_T();
int parse_E_prime();
int parse_F();
int parse_T_prime();

FILE *fp;
token_t putback = -1;

int main(int argc, char *argv[]) {
	fp = fopen(argv[1], "r");
	if (parse_P())
		printf("Successfully parsed\n");
	else
		printf("Parsing failed\n");

	return 0;
}

token_t scan_token(FILE *fp) {
	int c;
	token_t temp;

	if (putback != -1) {
		temp = putback;
		putback = -1;
		return temp;
	}
	do {
		c = fgetc(fp);
		switch(c) {
			case '*':
				return TOKEN_MULTIPLY;
			case '+':
				return TOKEN_PLUS;
			case '-':
				return TOKEN_SUBTRACT;
			case '/':
				return TOKEN_DIVIDE;
			case ';':
				return TOKEN_SEMI_COLON;
			case '(':
				return TOKEN_LPAREN;
			case ')':
				return TOKEN_RPAREN;
			case '{':
				return TOKEN_LBRACE;
			case '}':
				return TOKEN_RBRACE;
			case '=':
				if ((c = fgetc(fp)) == '=')
					return TOKEN_EQUAL;
				else {
					ungetc(c, fp);
					return TOKEN_ASSIGN;
				}
			case ',':
				return TOKEN_COMMA;
			case '&':
				if (fgetc(fp) == '&')
					return TOKEN_AND;
				else
					return ERROR;
			case '|':
				if (fgetc(fp) == '|')
					return TOKEN_OR;
				else
					return ERROR;
			case '!':
				if (fgetc(fp) == '=')
					return TOKEN_NOT_EQUAL;
				else {
					ungetc(c, fp);
					return TOKEN_NOT;
				}
			case '>':
				if ((c = fgetc(fp)) == '=')
					return TOKEN_GE;
				else {
					ungetc(c, fp);
					return TOKEN_GT;
				}
			case '<':
				if ((c = fgetc(fp)) == '=')
					return TOKEN_LE;
				else {
					ungetc(c, fp);
					return TOKEN_LT;
				}
			case EOF:
				return TOKEN_EOF;
		}
		if (isNumber(c)) {
			do {
				c = fgetc(fp);
			} while (isNumber(c));
			ungetc(c, fp);
			return TOKEN_INT;
		}
		if (isLetter(c)) {
			char *word = malloc(255);
			int i = 0;
			do {
				word[i] = c;
				i++;
				c = fgetc(fp);
			} while (isLetter(c) || isNumber(c));
			ungetc(c, fp);
			word[i] = '\0';
			if (strcmp(word, "i64") == 0) {
				free(word);
				return TOKEN_TYPE;
			} else if (strcmp(word, "func") == 0) {
				free(word);
				return TOKEN_FUNC;
			} else if (strcmp(word, "if") == 0) {
				free(word);
				return TOKEN_IF;
			} else if (strcmp(word, "elif") == 0) {
				free(word);
				return TOKEN_ELIF;
			} else if (strcmp(word, "else") == 0) {
				free(word);
				return TOKEN_ELSE;
			} else if (strcmp(word, "while") == 0) {
				free(word);
				return TOKEN_WHILE;
			}
			free(word);
			return TOKEN_ID;
		}
	} while (c == ' ' || c == '\n' || c == '\t');
	
	return ERROR;
}

int isNumber(char c) {
	return (c >= 48 && c <= 57);
}

int isLetter(char c) {
	return ((c >= 65 && c <= 90) || (c >= 97 && c <= 122));
}

void putback_token(token_t t) {
	if (putback == -1)
		putback = t;
	else{
		printf("Two tokens put back on the token stream\n");
		exit(EXIT_FAILURE);
	}
}

int expect_token(token_t t) {
	token_t new_token;

	if ((new_token = scan_token(fp)) == t)
		return 1;
	else {
		putback_token(new_token);
		return 0;
	}
}

int parse_P() {
	return parse_statements() && expect_token(TOKEN_EOF);
}

int parse_statements() {
	token_t t;
	int parsed;

	if ((parsed = parse_statement())) {
		while (((t = scan_token(fp)) != TOKEN_EOF) && (t != TOKEN_RBRACE) && parsed) {
			putback_token(t);
			parsed = parse_statement();
		}
		putback_token(t);

		return parsed;
	}

	return 0;
}

int parse_statement() {
	token_t t = scan_token(fp);

	if (t == TOKEN_TYPE) {
		putback_token(t);

		return parse_declaration();
	} else if (t == TOKEN_ID) {
		putback_token(t);

		return parse_assignment();
	} else if (t == TOKEN_FUNC) {
		putback_token(t);

		return parse_definition();
	} else if (t == TOKEN_IF) {
		putback_token(t);

		return parse_conditional();
	} else if (t == TOKEN_WHILE) {
		putback_token(t);

		return parse_loop();
	} else {
		fprintf(stderr, "parse error in parse_statement: unexpected token %d\n", t);
		
		return 0;
	}
}

int parse_loop() {
	return expect_token(TOKEN_WHILE) && expect_token(TOKEN_LPAREN) && parse_condition() & expect_token(TOKEN_RPAREN) && parse_block();
}

int parse_conditional() {
	token_t t;
	int parsed;

	if ((parsed = (expect_token(TOKEN_IF) && expect_token(TOKEN_LPAREN) && parse_condition() && expect_token(TOKEN_RPAREN)
	&& parse_block()))) {
		while ((t = scan_token(fp)) == TOKEN_ELIF && parsed) {
			parsed = (expect_token(TOKEN_LPAREN) && parse_condition() 
				&& expect_token(TOKEN_RPAREN)) && parse_block();
		}
		putback_token(t);
		if ((t = scan_token(fp)) == TOKEN_ELSE && parsed) {
			parsed = parse_block();
		} else 
			putback_token(t);

		return parsed;
	}

	return 0;
}

int parse_condition() {
	return parse_E() && parse_logic() && parse_E();
}

int parse_logic() {
	token_t t;

	if ((t = scan_token(fp)) == TOKEN_AND || t == TOKEN_OR || t == TOKEN_EQUAL || t == TOKEN_NOT_EQUAL || t == TOKEN_GT 
		|| t == TOKEN_LT || t == TOKEN_GE || t == TOKEN_LE) {
		return 1;
	}

	return 0;
}

int parse_declaration() {
	return expect_token(TOKEN_TYPE) && expect_token(TOKEN_ID) && expect_token(TOKEN_SEMI_COLON);
}

int parse_assignment() {
	return expect_token(TOKEN_ID) && expect_token(TOKEN_ASSIGN) && parse_E() && expect_token(TOKEN_SEMI_COLON);
}

int parse_definition() {
	return expect_token(TOKEN_FUNC) && expect_token(TOKEN_ID) && expect_token(TOKEN_LPAREN) && parse_parameters() 
	&& expect_token(TOKEN_RPAREN) && parse_block();
}

int parse_parameter() {
	return expect_token(TOKEN_TYPE) && expect_token(TOKEN_ID);
}

int parse_parameters() {
	token_t t;
	int parsed;

	if ((t = scan_token(fp)) == TOKEN_TYPE) {
		putback_token(t);
		if ((parsed = parse_parameter())) {
			while ((t = scan_token(fp)) == TOKEN_COMMA && parsed) {
				parsed = parse_parameter();
			}
			putback_token(t);

			return parsed;
		}

		return 0;
	} else {
		putback_token(t);

		return 1;
	}
}

int parse_block() {
	return expect_token(TOKEN_LBRACE) && parse_statements() && expect_token(TOKEN_RBRACE);
}

int parse_E() {
	return parse_T() && parse_E_prime();
}

int parse_T() {
	return parse_F() && parse_T_prime();
}

int parse_E_prime() {
	token_t t = scan_token(fp);
	if (t == TOKEN_PLUS || t == TOKEN_SUBTRACT) {
		return parse_T() && parse_E_prime();
	} else {
		putback_token(t);
		return 1;
	}
}

int parse_F() {
	token_t t = scan_token(fp);
	if (t == TOKEN_LPAREN) {
		return parse_E() && expect_token(TOKEN_RPAREN);
	} else if (t == TOKEN_INT || t == TOKEN_ID) {
		return 1;
	} else {
		printf("parse error: unexpected token %d\n", t);
		return 0;
	}
}

int parse_T_prime() {
	token_t t = scan_token(fp);
	if (t == TOKEN_MULTIPLY || t == TOKEN_DIVIDE) {
		return parse_F() && parse_T_prime();
	} else {
		putback_token(t);
		return 1;
	}
}
