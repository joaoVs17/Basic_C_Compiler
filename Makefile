CC = gcc
CFLAGS = -Wall -Wextra -g3

TARGET = main

SRC = \
	src/main.c \
	src/parser/parser.c \
	src/lexer/lexer.c \
	src/ast/ast.c \
	src/stack/stack.c \
	src/production/production.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)
	$(MAKE) clean

debug: $(TARGET)
	gdb ./$(TARGET)
	$(MAKE) clean

val: $(TARGET)
	valgrind \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

re: clean all
