#Compiler flags
CC = gcc

#debug: CFLAGS += -DDEBUG -g -O0 -Wall

compiler: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	$(CC) -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o

test: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o tests/test.txt tests/test2.txt
	$(CC) -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	./compiler tests/test2.txt
	./compiler tests/test.txt

debug: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	$(CC) -fsanitize=address -DDEBUG -O0 -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o

main.o: src/main.c src/lexer.c src/parser.c src/ast.c src/symbol.c src/scope.c src/compiler.h src/scope.h src/semantic_analyser.c src/ast.h
	$(CC) -g -c src/main.c

lexer.o: src/lexer.c src/compiler.h
	$(CC) -g -c src/lexer.c

parser.o: src/parser.c src/ast.c src/compiler.h
	$(CC) -g -c src/parser.c

ast.o: src/ast.c src/compiler.h
	$(CC) -g -c src/ast.c

symbol.o: src/symbol.c src/compiler.h
	$(CC) -g -c src/symbol.c

scope.o: src/scope.c src/symbol.h
	$(CC) -g -c src/scope.c

semantic_analyser.o: src/semantic_analyser.c src/semantic_analyser.h
	$(CC) -g -c src/semantic_analyser.c

clean:
	rm -f *.o
