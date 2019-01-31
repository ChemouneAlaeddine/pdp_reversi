#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "game.h"
#include "board.h"
#include "player.h"

/** usage function if the options are wrong **/
static void usage_option(char *f, char *s) {
  printf("%s: Wrong options : %s\n", f, s);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int (*black_player) (board, bool, int (*)(board, bool, int *), int *,
                       bool) = playerH;
  int (*black_heuristic) (board, bool, int *) = h_logic2;
  int black_parameters[10] = { 5, 10, 5, 16, 20, 6, 2, 0, 0, 0 };

  int (*white_player) (board, bool, int (*)(board, bool, int *), int *,
                       bool) = playerH;
  int (*white_heuristic) (board, bool, int *) = h_logic2;
  int white_parameters[10] = { 5, 10, 5, 16, 20, 6, 2, 0, 0, 0 };

  bool current_color = BLACK;
  int size = 8;
  board game_board;
  new_game(&game_board, size);

  bool s = false, w = false, b = false;
  int c;

  while (1) {
    static struct option long_options[] = {
      {"ai", no_argument, 0, 'a'},
      {"white", optional_argument, 0, 'w'},
      {"black", optional_argument, 0, 'b'},
      {"contest", required_argument, 0, 'c'},
      {"size", required_argument, 0, 's'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    c = getopt_long(argc, argv, "aw::b::c:s:h", long_options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
    case 'a':
      if (w || b) {
        usage_option(argv[0], "-a IA already assign");
      }
      black_player = ai_negamax_alpha_beta;
      white_player = ai_negamax_alpha_beta;
      b = true;
      w = true;
      break;

    case 'w':
      if (w) {
        usage_option(argv[0], "-w White IA already assign");
      }
      if (optarg) {
        load_ai(optarg, argv[0], &white_player, &white_heuristic,
                white_parameters);
      }
      else {
        white_player = ai_random;
      }
      w = true;
      break;

    case 'b':
      if (b) {
        usage_option(argv[0], "-b Black IA already assign");
      }
      if (optarg) {
        load_ai(optarg, argv[0], &black_player, &black_heuristic,
                black_parameters);
      }
      else {
        black_player = ai_random;
      }
      b = true;
      break;

    case 'c':
      if (w || b || s || (optind < argc)) {
        usage_option(argv[0], "-c [filename]");
      }
      else {
        size = load_file(optarg, argv[0], &game_board, &current_color);
        if (legal_moves(game_board, current_color)) {
          ai_negamax_alpha_beta(game_board, current_color, black_heuristic,
                                black_parameters, false);
        }
        else {
          printf("No move possible\n");
        }
        return 0;
      }
      break;

    case 's':
      if (s) {
        usage_option(argv[0], "Can't assigne -s[1-5] multiple time ");
      }
      size = 2 * atoi(optarg);
      if (size / 2 >= 1 && size / 2 <= 5) {
        new_game(&game_board, size);
        s = true;
      }
      else {
        usage_option(argv[0], "-s [1-5]");
      }
      break;

    case 'h':
      printf("Hello new player, you need help to venture into our game\n"
             "Worry not, we are here to help you out in this journey\n"
             "\n"
             "usage: ./reversi [options] [savefile]\n"
             "\n"
             "No options will start the game with 2 human players\n"
             "Either on a new 8x8 game if no savefile are given\n"
             "or on the game saved in the savefile\n"
             "\n"
             "Available options :\n"
             "-s SIZE [1-5]\n"
             "     set the size of the board to 2xSIZE\n"
             "\n"
             "-b\n"
             "     will use default AI of black player (ai negamax alpha beta)\n"
             "     see the README for further explanations\n"
             "\n"
             "-w\n"
             "     will use default AI of white player (ai negamax alpha beta)\n"
             "     see the README for further explanations\n"
             "\n"
             "-a\n"
             "     will use both default AI of black and white\n"
             "     (ai negamax alpha beta)\n"
             "\n"
             "-c FILENAME\n"
             "     use default AI to play the next move of FILENAME\n"
             "\n" "\n");
      exit(EXIT_SUCCESS);
      break;

    case '?':
      exit(EXIT_FAILURE);

    default:
      exit(EXIT_FAILURE);
    }
  }
  if (optind < argc) {
    if (s) {
      usage_option(argv[0],
                   "Can't specify a size '-s [size]' when loading a save");
    }
    else {
      size = load_file(argv[optind], argv[0], &game_board, &current_color);
      s = true;
    }
    if (optind + 1 < argc) {
      usage_option(argv[0], "Specify option before loadfile");
    }
  }
  game(game_board, current_color, false,
       black_player, black_heuristic, black_parameters,
       white_player, white_heuristic, white_parameters);
  return 0;
}
