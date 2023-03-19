#include <stdio.h>
#include <stdlib.h>
	
typedef enum {
	TOKEN_MULTIPLY,
	TOKEN_PLUS,
	TOKEN_SUBTRACT,
	TOKEN_DIVIDE,
	TOKEN_INT,
	TOKEN_ID,
	LEFT_CURLY_BRACKET,
	RIGHT_CURLY_BRACKET,
	SEMI_COLON,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_EOF,
	ERROR
} token_t;

token_t scan_token(FILE *fp);
int isNumber(char c);
int isLetter(char c);
int parse_P();
int parse_E();
int parse_T();
int parse_E_prime();
int parse_F();
int parse_T_prime();

FILE *fp;
token_t putback = -1;

int main(int argc, char *argv[]) {
	//token_t token;

	fp = fopen(argv[1], "r");
	/*while ((token = scan_token(fp)) != TOKEN_EOF) {
		if (token != ERROR)
			printf("%d\n", token);
		else {
			printf("Error");
			return 1;
		}	
	}*/
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
			case '{':
				return LEFT_CURLY_BRACKET;
			case '}':
				return RIGHT_CURLY_BRACKET;
			case ';':
				return SEMI_COLON;
			case '(':
				return TOKEN_LPAREN;
			case ')':
				return TOKEN_RPAREN;
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
			do {
				c = fgetc(fp);
			} while (isLetter(c) || isNumber(c));
			ungetc(c, fp);
			return TOKEN_ID;
		}
	} while (c == ' ' || c == '\n');
	
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
	return parse_E() && expect_token(TOKEN_EOF);
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
