# Compiler flags
CC = gcc
CFLAGS = -Wall -Werror -Wextra -O3 -DNDEBUG

# Project files
SRCS = src/main.c src/lexer.c src/parser.c src/ast.c src/symbol.c src/scope.c src/semantic_analyser.c
OBJS = $(SRCS:src/%.c=%.o)
EXE = compiler

# Test files
TESTS = tests/test.txt tests/test2.txt

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(EXE) $(TESTS)
	./$< tests/test2.txt
	./$< tests/test.txt

debug: CFLAGS = -Wall -Werror -Wextra -g -O0 -DDEBUG -fsanitize=address
debug: $(EXE)

%.o: src/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f $(EXE)
