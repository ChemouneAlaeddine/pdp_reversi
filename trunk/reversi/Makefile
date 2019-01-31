OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test

CFLAGS =  -Wall -Wextra -std=c99 -g -lm
CPPFLAGS = -I$(INCLUDE_DIR)
LDFLAGS =

OBJS = $(OBJ_DIR)/board.o \
       $(OBJ_DIR)/game.o  \
       $(OBJ_DIR)/player.o

EXEC=$(BIN_DIR)/reversi
TOOLS_EXEC=$(BIN_DIR)/reversi_tools

.PHONY:	all test clean cleanall

all : $(EXEC)

tools : $(TOOLS_EXEC)

$(EXEC): $(OBJS) $(OBJ_DIR)/main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TOOLS_EXEC): $(OBJS) $(OBJ_DIR)/main_tools.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(TEST_DIR)/gen/*

cleanall: clean
	rm -f $(EXEC) $(TOOLS_EXEC)
