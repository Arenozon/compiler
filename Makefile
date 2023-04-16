compiler: main.o lexer.o parser.o ast.o
	gcc -g -o compiler main.o lexer.o parser.o ast.o
	rm main.o lexer.o parser.o ast.o

main.o: main.c lexer.c parser.c ast.c compiler.h
	gcc -g -c main.c

lexer.o: lexer.c compiler.h
	gcc -g -c lexer.c

parser.o: parser.c ast.c compiler.h
	gcc -g -c parser.c

ast.o: ast.c compiler.h
	gcc -g -c ast.c

clean:
	rm main.o lexer.o parser.o ast.o

debug:
	gcc -fsanitize=address -O0 -g -o compiler main.o lexer.o parser.o ast.o
	rm main.o lexer.o parser.o ast.o
