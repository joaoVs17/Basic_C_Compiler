# Nome do compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -Wextra -std=c11

# Nome do executável
TARGET = programa

# Arquivo fonte
SRC = src/lexer.c

# Regra padrão
all: $(TARGET)


$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)
	
val: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Limpar arquivos gerados
clean:
	rm -f $(TARGET)