compiler: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	gcc -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o

test: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o tests/test.txt tests/test2.txt
	gcc -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	./compiler tests/test2.txt
	./compiler tests/test.txt

debug: main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o
	gcc -fsanitize=address -O0 -g -o compiler main.o lexer.o parser.o ast.o symbol.o scope.o semantic_analyser.o

main.o: src/main.c src/lexer.c src/parser.c src/ast.c src/symbol.c src/scope.c src/compiler.h src/scope.h src/semantic_analyser.c
	gcc -g -c src/main.c

lexer.o: src/lexer.c src/compiler.h
	gcc -g -c src/lexer.c

parser.o: src/parser.c src/ast.c src/compiler.h
	gcc -g -c src/parser.c

ast.o: src/ast.c src/compiler.h
	gcc -g -c src/ast.c

symbol.o: src/symbol.c src/compiler.h
	gcc -g -c src/symbol.c

scope.o: src/scope.c src/symbol.h
	gcc -g -c src/scope.c

semantic_analyser.o: src/semantic_analyser.c src/semantic_analyser.h
	gcc -g -c src/semantic_analyser.c

clean:
	rm -f *.o
