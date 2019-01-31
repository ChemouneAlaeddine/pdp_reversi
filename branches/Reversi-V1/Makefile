OBJ = obj/
BIN = bin/
SRC = src/
INCLUDE = include/
CC=gcc
CFLAGS= -g -Wall -std=c99

$(BIN)game: $(OBJ)board.o $(OBJ)game.o $(OBJ)player.o
	$(CC) $^ -o $@

$(OBJ)%.o:$(SRC)%.c
	$(CC) $(CFLAGS) -I $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ)*.o

cleanall:
	rm -f $(OBJ)*.o $(BIN)*
