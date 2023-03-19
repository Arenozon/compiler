#include <stdio.h>
#include <stdlib.h>

enum flag{NUM, ADD, SUB};

struct token {
	enum flag flg;
	int val;
};

void lexer(FILE *fp, struct token *tok);

int main(int argc, char *argv[]) {
	FILE *fp;
	struct token tok[255];

	fp = fopen("lexer_test.txt", "r");
	lexer(fp, tok);
	for (int i = 0; i < 15; i++) {
		if (tok[i].flg == NUM)
			printf("%d\n", tok[i].val);
		else if (tok[i].flg == ADD || tok[i].flg == SUB)
			printf("%c\n", tok[i].val);
	}

	return 0;
}

void lexer(FILE *fp, struct token *tok) {
	char buffer[255];

	buffer[0] = fgetc(fp);
	for (int i = 0, j = 0; buffer[i] != EOF; i++) {
		if (buffer[i] >= 48 && buffer[i] <= 57) {
			tok[j].flg = NUM;
			tok[j++].val = buffer[i] - 48;
		} else if (buffer[i] == '+') {
			tok[j].flg = ADD;
			tok[j++].val = buffer[i];
		} else if (buffer[i] == '-') {
			tok[j].flg = SUB;
			tok[j++].val = buffer[i];
		}
		buffer[i + 1] = fgetc(fp);
	}
}
