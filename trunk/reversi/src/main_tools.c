#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "board.h"
#include "player.h"

#define STRING_BUFFER 50

/* Display the score and time of every game and
*  follow the CSV file format to be able to store the data in a file */
void print_csv(int n, int black_score, double *black_time, int white_score,
               double *white_time, bool mute_ai) {
  if (n == 0 || !mute_ai) {
    printf
      ("GAME, Black score, Black min time, Black max time, Black mean time, White score, White min time, White max time, White mean time\n");
  }
  printf("%4d, %11d, %14f, %14f, %15f, %11d, %14f, %14f, %15f\n", n,
         black_score, black_time[0], black_time[1], black_time[2],
         white_score, white_time[0], white_time[1], white_time[2]);
}

/* Play a game with the 2 AIs found in the parameters
*  calculate the time of each move to be able to display it*/
int test_game(int n, board game_board, bool current_color, bool mute_ai,
              int (black_player) (board, bool, int (*)(board, bool, int *),
                                  int *, bool), int (*black_heuristic) (board,
                                                                        bool,
                                                                        int
                                                                        *),
              int *black_parameters, int (white_player) (board, bool,
                                                         int (*)(board, bool,
                                                                 int *),
                                                         int *, bool),
              int (*white_heuristic) (board, bool, int *),
              int *white_parameters) {
  int val;
  double black_time[3] = { 100, 0, 0 };
  int div_b = 0;
  double white_time[3] = { 100, 0, 0 };
  int div_w = 0;
  double tmp;
  clock_t t;
  while (!game_over(game_board)) {
    if (!legal_moves(game_board, current_color)) {
      current_color = !current_color;
    }
    if (current_color) {
      t = clock();
      val =
        black_player(game_board, current_color, black_heuristic,
                     black_parameters, mute_ai);
      tmp = (double) (clock() - t) / CLOCKS_PER_SEC;
      while (!play_move(&game_board, current_color, val)) {
        printf("Wrong move\n");
        t = clock();
        val =
          black_player(game_board, current_color, black_heuristic,
                       black_parameters, mute_ai);
        tmp = (double) (clock() - t) / CLOCKS_PER_SEC;
      }
      div_b++;
      black_time[2] += tmp;
      if (tmp < black_time[0]) {
        black_time[0] = tmp;
      }
      if (tmp > black_time[1]) {
        black_time[1] = tmp;
      }
    }
    else {
      t = clock();
      val =
        white_player(game_board, current_color, white_heuristic,
                     white_parameters, mute_ai);
      tmp = (double) (clock() - t) / CLOCKS_PER_SEC;
      while (!play_move(&game_board, current_color, val)) {
        printf("Wrong move\n");
        t = clock();
        val =
          white_player(game_board, current_color, white_heuristic,
                       white_parameters, mute_ai);
        tmp = (double) (clock() - t) / CLOCKS_PER_SEC;
      }
      div_w++;
      white_time[2] += tmp;
      if (tmp < white_time[0]) {
        white_time[0] = tmp;
      }
      if (tmp > white_time[1]) {
        white_time[1] = tmp;
      }
    }
    current_color = !current_color;
  }
  white_time[2] = white_time[2] / div_w;
  black_time[2] = black_time[2] / div_b;
  print_csv(n, scores(game_board.black), black_time, scores(game_board.white),
            white_time, mute_ai);
  return 0;
}

void concat_str_int(char *dest, char *src, int integer) {
  char tmp[STRING_BUFFER];
  strcpy(dest, src);
  sprintf(tmp, "%d", integer);
  strcat(dest, tmp);
}

/* This function is used to display the progress we have made in our testing*/
void print_progress(int actual_iter, int nbr_iter) {
  printf("In Progress : %d%% (Game n°%d)\n",
         (int) ((double) actual_iter / nbr_iter * 100), actual_iter);
}
/* Generate n game played by the ai_ramdom and
*  store them in the dest_gen repertory */
void generate_games(int n, int size, int depth_gen, char *dest_gen,
                    bool mute_ai, bool mute_progress) {
  char filename[STRING_BUFFER];
  board game_board;
  int tmp;
  for (int i = 0; i < n; i++) {
    if (!mute_progress) {
      print_progress(i, n);
    }
    concat_str_int(filename, dest_gen, i);
    new_game(&game_board, size);
    for (int j = 0; j < depth_gen; j++) {
      play_move(&game_board, 1,
                ai_random(game_board, 1, h_logic, &tmp, mute_ai));
      play_move(&game_board, 0,
                ai_random(game_board, 0, h_logic, &tmp, mute_ai));
    }
    save_file(game_board, 1, size, filename);
  }
}

/* Play n games with the 2 AIs placed in parameters and
*  calculate the data such as the score and the time used */
void test_play(int n, char *dest_gen, char *usage, bool mute_ai,
               bool mute_progress, int (black_player) (board, bool,
                                                       int (*)(board, bool,
                                                               int *), int *,
                                                       bool),
               int (*black_heuristic) (board, bool, int *),
               int *black_parameters) {
  bool current_color;
  double max = 0;
  double min = 100 * CLOCKS_PER_SEC;
  double mean = 0;
  double tmp = 0;
  char filename[STRING_BUFFER];
  board game_board;
  clock_t t;
  for (int i = 0; i < n; i++) {
    if (!mute_progress) {
      print_progress(i, n);
    }
    concat_str_int(filename, dest_gen, i);
    load_file(filename, usage, &game_board, &current_color);
    if (legal_moves(game_board, current_color)) {
      t = clock();
      black_player(game_board, current_color, black_heuristic,
                   black_parameters, mute_ai);
      tmp = clock() - t;
      mean += tmp;
      if (tmp > max) {
        max = tmp;
      }
      if (tmp < min) {
        min = tmp;
      }
    }
  }
  printf("Temps moyen : %f\n", mean / n / CLOCKS_PER_SEC);
  printf("Temps min : %f\n", min / CLOCKS_PER_SEC);
  printf("Temps max : %f\n", max / CLOCKS_PER_SEC);
}


int main(int argc, char *argv[]) {
  int size = 8;
  int nbr_generate = 0;
  int nbr_test = 0;
  int nbr_play = 0;
  int depth_gen = 5;
  char dest_gen[STRING_BUFFER] = "test/gen/Gen-random-";
  char filename[STRING_BUFFER];
  board game_board;
  bool current_color;
  bool mute_ai = true;
  bool mute_progress = true;

  int (*black_player) (board, bool, int (*)(board, bool, int *), int *,
                       bool) = ai_negamax_alpha_beta;
  int (*black_heuristic) (board, bool, int *) = h_logic2;
  int black_parameters[10] = { 5, 10, 20, 20, 500, 20, 20, 0, 0, 0 };

  int (*white_player) (board, bool, int (*)(board, bool, int *), int *,
                       bool) = ai_negamax_alpha_beta;
  int (*white_heuristic) (board, bool, int *) = h_logic2;
  int white_parameters[10] = { 5, 10, 20, 20, 500, 20, 20, 0, 0, 0 };

  int c;
  while (1) {
    static struct option long_options[] = {
      {"generate", required_argument, 0, 'g'},
      {"size", required_argument, 0, 's'},
      {"depth", required_argument, 0, 'd'},
      {"test", required_argument, 0, 't'},
      {"white", required_argument, 0, 'w'},
      {"black", required_argument, 0, 'b'},
      {"play", required_argument, 0, 'p'},
      {"unmute", no_argument, 0, 'u'},
      {"verbose", no_argument, 0, 'v'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    c =
      getopt_long(argc, argv, "g:s:d:t:w:b:p:uvh", long_options,
                  &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
    case 's':
      size = 2 * atoi(optarg);
      break;

    case 'd':
      depth_gen = atoi(optarg);
      break;

    case 'w':
      load_ai(optarg, argv[0], &white_player, &white_heuristic,
              white_parameters);
      break;

    case 'b':
      load_ai(optarg, argv[0], &black_player, &black_heuristic,
              black_parameters);
      break;

    case 'g':
      nbr_generate = atoi(optarg);
      break;

    case 't':
      nbr_test = atoi(optarg);
      break;

    case 'p':
      nbr_play = atoi(optarg);
      break;

    case 'u':
      mute_ai = false;
      break;

    case 'v':
      mute_ai = false;
      mute_progress = false;
      break;

    case 'h':
      printf("usage: ./reversi_tools [options]\n"
             "\n"
             "Different tools made to tests reversi AI\n"
             "Don't do anything without -gtp options\n"
             "\n"
             "Available options :\n"
             "-g --generate [N]    Generate N random 8x8 board of reversi\n"
             "                     with 10 moves already played.\n"
             "                     The size of the board or the progress in\n"
             "                     the game can be modify with option"
             " -s or -d\n"
             "\n"
             "-s --size [1-5]      Set the board size of the generator"
             " to 2xSIZE\n"
             "\n"
             "-d --depth [N]       Set the progress of the generator to 2xN,\n"
             "                     so the generator will play 2xN moves\n"
             "\n"
             "-t --test [N]        Play the first move of N generated games\n"
             "                     with the AI of the black player\n"
             "                     and display time informations\n"
             "\n"
             "-p --play [N]        Play N generated games with AI and display\n"
             "                     the score and time informations of the AI\n"
             "                     in CSV format\n"
             "\n"
             "-w --white FILENAME  Set the AI of the white player to the AI\n"
             "                     which is specified in the file\n"
             "                     (default : negamax alpha beta with h_logic2 "
             "as heuristic)\n"
             "                     See the README for further informations\n"
             "\n"
             "-b --black FILENAME  Set the AI of the black player to the AI\n"
             "                     which is specified in the file\n"
             "                     (default : negamax alpha beta with h_logic2 "
             "as heuristic)\n"
             "                     See the README for further informations\n"
             "\n"
             "-u --unmute          Unmute the AI and show all the moves\n"
             "                     played by them in the process of\n"
             "                     the different functions\n"
             "\n"
             "-v --verbose         Unmute the AI and show the progress in\n"
             "                     percentage of the different process\n"
             "\n" "-h --help            Display the help\n" "\n");
      exit(EXIT_SUCCESS);
      break;
    }
  }

  if (nbr_generate > 0) {
    generate_games(nbr_generate, size, depth_gen, dest_gen, mute_ai,
                   mute_progress);
  }

  if (nbr_test > 0) {
    test_play(nbr_test, dest_gen, argv[0], mute_ai, mute_progress,
              black_player, black_heuristic, black_parameters);
  }

  if (nbr_play > 0) {
    for (int i = 0; i < nbr_play; i++) {
      if (!mute_progress) {
        print_progress(i, nbr_play);
      }
      concat_str_int(filename, dest_gen, i);
      size = load_file(filename, argv[0], &game_board, &current_color);
      test_game(i, game_board, current_color, mute_ai,
                black_player, black_heuristic, black_parameters,
                white_player, white_heuristic, white_parameters);
    }
  }
  if (!nbr_generate && !nbr_test && !nbr_play) {
    printf("Nothing to do, use -h or --help for more information.\n");
  }
  return 0;
}
