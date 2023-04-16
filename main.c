#include "compiler.h"

int main(int argc, char *argv[]) {
	FILE *fp;

	fp = fopen(argv[1], "r");
	struct token_stream *top = init_stream();	// top pointer points to the first token on the stream
	struct token_stream *bot = top;				// bot pointer points to the last token on the stream
	
	/* scan_tokens is passed the bot pointer so that the bot 
	 * pointer is updated to follow the latest token, but 
	 * the top pointer continues to point to the first token */
	if (scan_tokens(fp, &bot)) {
		fprintf(stderr, "Scanning failed on line %d\n", bot->token->line);
		fclose(fp);

		return 1;
	}
	printf("Scanning succeeded\n");
	if (parse_P(&top))
		printf("Successfully parsed\n");
	else {
		fprintf(stderr, "Parsing failed on line %d\n", top->token->line);
		fclose(fp);

		return 1;
	}
	fclose(fp);

	return 0;
}

