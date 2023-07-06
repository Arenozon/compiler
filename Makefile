compiler: main.o lexer.o parser.o ast.o semantic_analyser.o
	gcc -g -o compiler main.o lexer.o parser.o ast.o semantic_analyser.o
	rm main.o lexer.o parser.o ast.o semantic_analyser.o

test: main.o lexer.o parser.o ast.o semantic_analyser.o tests/test.txt tests/test2.txt
	gcc -g -o compiler main.o lexer.o parser.o ast.o semantic_analyser.o
	rm main.o lexer.o parser.o ast.o semantic_analyser.o
	./compiler tests/test2.txt
	./compiler tests/test.txt

debug: main.o lexer.o parser.o ast.o semantic_analyser.o
	gcc -fsanitize=address -O0 -g -o compiler main.o lexer.o parser.o ast.o semantic_analyser.o
	rm main.o lexer.o parser.o ast.o semantic_analyser.o

main.o: main.c lexer.c parser.c ast.c semantic_analyser.c compiler.h
	gcc -g -c main.c

lexer.o: lexer.c compiler.h
	gcc -g -c lexer.c

parser.o: parser.c ast.c compiler.h
	gcc -g -c parser.c

ast.o: ast.c compiler.h
	gcc -g -c ast.c

semantic_analyser.o: semantic_analyser.c compiler.h
	gcc -g -c semantic_analyser.c

clean:
	rm main.o lexer.o parser.o ast.o semantic_analyser.o
