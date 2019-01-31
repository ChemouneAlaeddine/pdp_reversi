#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include "player.h"

/** usage function if unable to open the file **/
static void usage_open_file(char *f, char *s) {
  printf("%s: Can't find file '%s'\n", f, s);
  exit(EXIT_FAILURE);
}

/** usage function if the loaded file do not have the correct save format **/
static void usage_file_format(char *f, char *s) {
  printf("%s: Wrong file format : %s\n", f, s);
  exit(EXIT_FAILURE);
}

/** usage function if we are unable to save a game**/
static void usage_save(char *f) {
  printf("%s: Unable to save file\n", f);
  exit(EXIT_FAILURE);
}

static void usage_load_ai(char *f, char *e, char *str) {
  printf("%s: %s: %s\n", f, e, str);
  exit(EXIT_FAILURE);
}

void load_ai(char *filename, char *usage,
             int (**ai) (board, bool, int (*)(board, bool, int *), int *,
                         bool), int (**heuristic) (board, bool, int *),
             int *parameters) {

  for (int i = 0; i < 10; i++) {
    parameters[i] = 0;
  }
  if (filename == NULL) {
    usage_open_file(usage, "null");
  }
  FILE *file;
  char str[50];
  file = fopen(filename, "r");
  if (file == NULL) {
    usage_open_file(usage, filename);
  }

  if (fgets(str, 50, file) != NULL) {
    if (strcmp(str, "ai_random\n") == 0) {
      *ai = ai_random;
      return;
    }
    else if (strcmp(str, "ai_basic\n") == 0) {
      *ai = ai_basic;
      return;
    }
    else if (strcmp(str, "ai_heuristic\n") == 0) {
      *ai = ai_heuristic;
      return;
    }
    else if (strcmp(str, "ai_monte_carlo\n") == 0) {
      *ai = ai_monte_carlo;
      return;
    }
    else if (strcmp(str, "ai_minimax\n") == 0) {
      *ai = ai_minimax;
    }
    else if (strcmp(str, "ai_negamax_alpha_beta\n") == 0) {
      *ai = ai_negamax_alpha_beta;
    }
    else if (strcmp(str, "ai_negamax_alpha_beta_sym\n") == 0) {
      *ai = ai_negamax_alpha_beta_sym;
    }
    else {
      usage_load_ai(usage, "wrong AI in file", str);
    }
  }
  else {
    usage_file_format(usage, "Empty file");
  }

  if (fgets(str, 50, file) != NULL) {
    parameters[0] = atoi(str);
  }
  else {
    usage_file_format(usage, "Depth parameters of the ai is missing");
  }

  if (fgets(str, 50, file) != NULL) {
    parameters[1] = atoi(str);
  }
  else {
    usage_file_format(usage, "Time limit parameters of the ai is missing");
  }

  int number_prm;
  if (fgets(str, 50, file) != NULL) {
    if (strcmp(str, "h_logic\n") == 0) {
      *heuristic = h_logic;
      number_prm = 4;
    }
    else if (strcmp(str, "h_logic2\n") == 0) {
      *heuristic = h_logic2;
      number_prm = 5;
    }
    else {
      usage_load_ai(usage, "wrong heuristic in file", str);
    }
  }

  for (int i = 2; i < number_prm + 2; i++) {
    if (fgets(str, 50, file) != NULL) {
      parameters[i] = atoi(str);
    }
    else {
      usage_file_format(usage, "Need more parameters for this heuristic. "
                        "See the README for details");
    }
  }
}

int load_file(char *filename, char *usage, board * game_board,
              bool * current_color) {
  //printf("%s", filename);
  game_board->white = 0;
  game_board->black = 0;
  int dim = 0;                  //Dimension of the game's grid

  if (filename == NULL) {       //Check if there is atleast 1 arguement
    usage_open_file(usage, "null");
  }
  int file;
  if ((file = open(filename, O_RDONLY)) < 0) {  //Try to open the last argument as a file
    usage_open_file(usage, filename);
  }
  else {
    ssize_t readSize;           //Used to detect EndOfFile
    char c;                     //Char buffer
    int sizeY = 0;              //Count the Y axis' size
    bool foundCurrentColor = false;     //Check if we found the next player to play
    while ((readSize = read(file, &c, sizeof(char))) != 0) {
      while (!foundCurrentColor) {
        c = toupper(c);
        //This loop try to find the next player to play, exit the load program if it fails
        bool com = false;       //Tell if we are in comment state for this line
        while (c != '\n') {

          if (c == '#' || com == true) {
            com = true;
          }
          else if ((c == X || c == O) && foundCurrentColor == false) {
            if (c == X) {
              *current_color = BLACK;
            }
            else if (c == O) {
              *current_color = WHITE;
            }
            foundCurrentColor = true;
          }
          else if (c != ' ' && (int) c != 9) {
            usage_file_format(usage, "Unexpected char in file");
          }
          if ((readSize = read(file, &c, sizeof(char))) == 0) { //Read next letter and check if not at EndOfFile
            usage_file_format(usage, "EOF detected too soon");
          }
        }
        if ((readSize = read(file, &c, sizeof(char))) == 0) {   //Read next letter and check if not at EndOfFile
          usage_file_format(usage, "EOF detected too soon");
        }
      }
      int sizeX = 0;            //Count the X axis' size
      bool com = false;         //Tell if we are in comment state for this line
      while (c != '\n') {
        c = toupper(c);
        if (c == '#' || com == true) {
          com = true;
        }
        else if ((c == X) || (c == O) || (c == EMPTY)) {
          if (c == X) {
            add_pawn(&game_board->black, sizeX + sizeY * dim);
          }
          else if (c == O) {
            add_pawn(&game_board->white, sizeX + sizeY * dim);
          }
          sizeX++;
        }
        else if (c != ' ' && (int) c != 9) {
          usage_file_format(usage, "Unexpected char in file");
        }
        if ((readSize = read(file, &c, sizeof(char))) == 0) {   //Read next letter and force exit if EndOfFile
          c = '\n';
        }
      }


      if (sizeX > 0) {          //increment sizeY if previous line wasn't empty or null
        sizeY++;
        if (dim == 0) {
          dim = sizeX;
        }
        if (dim != sizeX || sizeX % 2 != 0) {   //Check if all size are the same and aren't odd
          usage_file_format(usage, "Wrong size of the board");
        }
      }
    }
    if (sizeY != dim) {
      usage_file_format(usage, "EOF detected too soon");
    }
  }
//printf("All good ;)\n");
  close(file);
  game_board->size = dim;
  return dim;
}

int save_file(board game_board, bool current_color, int size, char *filename) {
  int file;
  if ((file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0) {  //TODO Could Change the name for current time, to be able to save several games
    usage_save(filename);
  }
  char c;
  if (current_color == BLACK)
    c = X;
  else
    c = O;
  write(file, &c, sizeof(char));
  c = '\n';
  write(file, &c, sizeof(char));
  __uint128_t tmp = 1;
  for (int i = 0; i < (size * size); i++) {
    if (game_board.white & (tmp << i)) {
      c = O;
    }
    else if (game_board.black & (tmp << i)) {
      c = X;
    }
    else {
      c = EMPTY;
    }
    write(file, &c, sizeof(char));
    if ((i + 1) % size == 0) {
      c = '\n';
      write(file, &c, sizeof(char));
    }
  }
  close(file);
  return 1;

}



void print_board(board game_board, bool current_color) {
  __uint128_t tmp = 1;
  __uint128_t legal = legal_moves(game_board, current_color);
  int size = game_board.size;
  char rows = A;
  printf("  ");
  for (int i = 0; i < size; i++) {
    printf("%d ", i + 1);
  }
  printf("\n");
  for (int i = 0; i < (size * size); i++) {
    if (i % size == 0) {
      printf("%c ", rows++);
    }
    if (game_board.white & (tmp << i)) {
      printf("%c ", O);
    }
    else if (game_board.black & (tmp << i)) {
      printf("%c ", X);
    }
    else if (legal & (tmp << i)) {
      printf("%c ", AVAIL);
    }
    else {
      printf("%c ", EMPTY);
    }
    if (i % size == size - 1) {
      printf("\n");
    }
  }
  printf("\n");
}

bool ask(char question[]) {
  char answer;
  printf("%s\n", question);
  scanf("%c", &answer);
  if (tolower(answer) == 'y') {
    return true;
  }
  return false;
}


void game(board game_board, bool current_color, bool mute,
          int (blackPlay) (board, bool, int (*)(board, bool, int *), int *,
                           bool), int (*black_heuristic) (board, bool, int *),
          int *black_parameters, int (whitePlay) (board, bool,
                                                  int (*)(board, bool, int *),
                                                  int *, bool),
          int (*white_heuristic) (board, bool, int *),
          int *white_parameters) {
  int val;

  while (!game_over(game_board)) {
    if (!legal_moves(game_board, current_color)) {
      current_color = !current_color;
    }
    print_board(game_board, current_color);
    if (current_color) {
      printf("Black turn\n");
      val =
        blackPlay(game_board, current_color, black_heuristic,
                  black_parameters, mute);
      while (!play_move(&game_board, current_color, val)) {
        if (val == -1) {
          if (ask("Would you like to save [y/N] ?")) {
            if (save_file
                (game_board, current_color, game_board.size, "SavedGame.txt"))
              printf("Game saved\n");
          }
          else
            printf("Game not saved\n");
          return;
        }
        printf("Wrong move\n");
        val =
          blackPlay(game_board, current_color, black_heuristic,
                    black_parameters, mute);
      }
    }
    else {
      printf("White turn\n");
      val =
        whitePlay(game_board, current_color, white_heuristic,
                  white_parameters, mute);
      while (!play_move(&game_board, current_color, val)) {
        if (val == -1) {
          if (ask("Would you like to save [y/N] ?")) {
            if (save_file
                (game_board, current_color, game_board.size, "SavedGame.txt"))
              printf("Game saved\n");
          }
          else
            printf("Game not saved\n");
          return;
        }
        printf("Wrong move\n");
        val =
          whitePlay(game_board, current_color, white_heuristic,
                    white_parameters, mute);
      }
    }
    current_color = !current_color;
    printf("\n");
  }
  print_board(game_board, current_color);
  printf("END\n");
  printf("Score Black: %d\n", scores(game_board.black));
  printf("Score White: %d\n", scores(game_board.white));
}
