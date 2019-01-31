#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>



static void emptyBuffer() {
  int c = 0;
  while (c != '\n' && c != EOF) {
    c = getchar();
  }
}

/* fgets + delete the final enter + empty the buffer if the string is too long */
static int fgetsCustom(char *move) {
  char *positionEnter = NULL;

  if (fgets(move, BUFFER, stdin) != NULL) {
    positionEnter = strchr(move, '\n');
    if (positionEnter != NULL) {
      *positionEnter = '\0';
    }
    else {
      emptyBuffer();
    }
    return 1;
  }
  else {
    emptyBuffer();
    return 0;
  }
}

int playerH(board game_board, bool color,
            int (*heuristic) (board, bool, int *), int *parameters,
            bool mute) {
  (void) color;
  (void) heuristic;
  (void) parameters;
  (void) mute;

  int size = game_board.size;
  char move[BUFFER] = "00";
  int len = 0;
  char first = toupper(move[0]);
  char second = toupper(move[1]);
  char third = toupper(move[2]);
  while (len != 2 || (!(first >= A && first <= A + (size - 1))
                      || !(second >= '1' && second <= '1' + (size - 1)))
         || first != Q) {
    printf("Case('%c-%c''1-%d') : ", A, A + (size - 1), 1 + (size - 1));
    fgetsCustom(move);
    len = strlen(move);
    first = toupper(move[0]);
    second = toupper(move[1]);
    third = toupper(move[2]);
    if (first == Q && len == 1) {
      return -1;
    }
    if (first >= '1' && first <= '1' + (size - 1)) {
      if (len == 2) {
        char tmp = first;
        first = second;
        second = tmp;
      }
      else {
        char tmp = first;
        first = third;
        third = second;
        second = tmp;
      }
    }
    if ((len > 2 && size < 10) || (len > 3 && size == 10)) {
      printf("The move is too long (only 2 character needed)\n");
    }
    else if (len < 2) {
      printf("The move is too short (2 character needed)\n");
    }
    else if (first >= A && first <= A + (size - 1)
             && (second >= '1' && second <= '1' + (size - 1))
             && ((second == '1' && third == '0' && size == 10)
                 || (third == '\0' && size <= 10))) {
      if (second == '1' && third == '0') {
        return (first - A) * size + 9;
      }
      else {
        return (first - A) * size + (second - '1');
      }
    }
    else {
      printf
        ("Syntax incorect, 1st character must be a letter between %c and %c and second a number between 1 and %d\n",
         A, A + size - 1, 1 + size - 1);
    }
  }
  return -1;
}



int ai_random(board game_board, bool color,
              int (*heuristic) (board, bool, int *), int *parameters,
              bool mute) {
  (void) heuristic;
  (void) parameters;
  srand(clock());
  __uint128_t moves = legal_moves(game_board, color);
  int size = game_board.size;
  int number = scores(moves);
  if (number == 0) {
    return 0;
  }
  int squares[number];
  get_squares(moves, size, squares);
  int play = squares[rand() % number];
  if (!mute) {
    printf("%c%d\n", A + play / size, 1 + play % size);
  }
  return play;
}

int ai_basic(board game_board, bool color,
             int (*heuristic) (board, bool, int *), int *parameters,
             bool mute) {
  (void) heuristic;
  (void) parameters;
  __uint128_t moves = legal_moves(game_board, color);
  int size = game_board.size;
  int number = scores(moves);
  if (number == 0) {
    return 0;
  }
  int squares[number];
  int play;
  int score = 0;
  get_squares(moves, size, squares);
  for (int i = 0; i < number; i++) {
    board tmp = game_board;
    play_move(&tmp, color, squares[i]);
    __uint128_t score_board;
    if (color) {
      score_board = tmp.black;
    }
    else {
      score_board = tmp.white;
    }
    if (score < scores(score_board)) {
      score = scores(score_board);
      play = squares[i];
    }
  }
  if (!mute) {
    printf("%c%d\n", A + play / size, 1 + play % size);
  }
  return play;
}




int ai_heuristic(board game_board, bool color,
                 int (*heuristic) (board, bool, int *), int *parameters,
                 bool mute) {
  srand(clock());
  __uint128_t moves = legal_moves(game_board, color), tmp = 1;
  int size = game_board.size, max = -10;
  int number = scores(moves);
  int squares[number], evaluation[number];
  get_squares(moves, size, squares);
  tmp = tmp << (size * size - 1);
  for (int i = 0; i < number; i++)
    evaluation[i] = 0;
  for (int i = 0; i < number; i++) {
    bool corner = (squares[i] == 0) || (squares[i] == size - 1)
      || (squares[i] == size * size - 1)
      || (squares[i] == size * (size - 1)), around_corner_1 =
      (squares[i] == 1) || (squares[i] == size)
      || (squares[i] == size + 1), corner_1_taken =
      (game_board.black & (tmp >> (size * size - 1)))
      || (game_board.white & (tmp >> (size * size - 1))), around_corner_2 =
      (squares[i] == size - 2) || (squares[i] == 2 * size - 1)
      || (squares[i] == 2 * size - 2), corner_2_taken =
      (game_board.black & (tmp >> (size * (size - 1))))
      || (game_board.white & (tmp >> (size * (size - 1)))), around_corner_3 =
      (squares[i] == size * size - 2) || (squares[i] == size * (size - 1) - 1)
      || (squares[i] == size * (size - 1) - 2), corner_3_taken =
      (game_board.black & tmp)
      || (game_board.white & tmp), around_corner_4 =
      (squares[i] == size * (size - 1) + 1)
      || (squares[i] == size * (size - 2))
      || (squares[i] == size * (size - 2) + 1), corner_4_taken =
      (game_board.black & (tmp >> (size - 1)))
      || (game_board.white & (tmp >> (size - 1))), edge = ((squares[i] > 1)
                                                           && (squares[i] <
                                                               size - 2))
      || ((squares[i] > size) && (squares[i] < size * (size - 2))
          && (squares[i] % size == 0)) || ((squares[i] > 3 * size - 2)
                                           && (squares[i] < size * (size - 2))
                                           && ((squares[i] + 1) % size == 0))
      || ((squares[i] > (size * (size - 1) + 1))
          && (squares[i] < size * size - 2)), before_edge_1 =
      (squares[i] > size + 1)
      && (squares[i] < 2 * size - 2), edge_1_taken =
      (game_board.black & (tmp >> (size * size - 1 - squares[i] + size)))
      || (game_board.white & (tmp >> (size * size - 1 - squares[i] + size))),
      before_edge_2 = (squares[i] > size + 1)
      && (squares[i] < size * (size - 2) + 1)
      && ((squares[i] - 1) % size == 0), edge_2_taken =
      (game_board.black & (tmp >> (size * size - squares[i])))
      || (game_board.white & (tmp >> (size * size - squares[i]))),
      before_edge_3 = (squares[i] > 3 * size - 3)
      && (squares[i] < size * (size - 2) - 1)
      && ((squares[i] + 2) % size == 0), edge_3_taken =
      (game_board.black & (tmp >> (size * size - squares[i] - 2)))
      || (game_board.white & (tmp >> (size * size - squares[i] - 2))),
      before_edge_4 = (squares[i] > (size * (size - 2) + 1))
      && (squares[i] < size * (size - 1) - 2), edge_4_taken =
      (game_board.black & (tmp >> (size * size - 1 - squares[i] - size)))
      || (game_board.white & (tmp >> (size * size - 1 - squares[i] - size))),
      around_any_corner = (around_corner_1 && !corner_1_taken)
      || (around_corner_2 && !corner_2_taken) || (around_corner_3
                                                  && !corner_3_taken)
      || (around_corner_4
          && !corner_4_taken), around_any_corner_taken = (around_corner_1
                                                          && corner_1_taken)
      || (around_corner_2 && corner_2_taken) || (around_corner_3
                                                 && corner_3_taken)
      || (around_corner_4 && corner_4_taken), before_any_edge = (before_edge_1
                                                                 &&
                                                                 !edge_1_taken)
      || (before_edge_2 && !edge_2_taken) || (before_edge_3 && !edge_3_taken)
      || (before_edge_4
          && !edge_4_taken), before_any_edge_taken = (before_edge_1
                                                      && edge_1_taken)
      || (before_edge_2 && edge_2_taken) || (before_edge_3 && edge_3_taken)
      || (before_edge_4 && edge_4_taken);

    evaluation[i] += num_points(game_board, color, squares[i]);
    if (corner) {
      evaluation[i] += 4;
    }
    else {
      if (around_any_corner) {
        evaluation[i] += -4;
      }
      else {
        if (around_any_corner_taken) {
          evaluation[i] += 2;
        }
        else {
          if (size > 4) {
            if (edge) {
              evaluation[i] += 2;
            }
            else {
              if (before_any_edge) {
                evaluation[i] += -2;
              }
              else {
                if (before_any_edge_taken) {
                  evaluation[i] += 1;
                }
                else {          // center
                  evaluation[i] += 1;
                }
              }
            }
          }
        }
      }
    }
    if (evaluation[i] > max)
      max = evaluation[i];
  }
  int nb_best_shots = 0;
  for (int s = 0; s < number; s++)
    if (evaluation[s] == max)
      nb_best_shots++;
  int best_shots[nb_best_shots], l = 0;
  for (int s = 0; s < nb_best_shots; s++)
    best_shots[s] = 0;
  for (int s = 0; s < number; s++)
    if (evaluation[s] == max) {
      best_shots[l] = s;
      l++;
    }
  int play = best_shots[rand() % nb_best_shots];
  if (!mute) {
    printf("%c%d\n", A + squares[play] / size, 1 + squares[play] % size);
  }
  if (((int) (sizeof(squares) / sizeof(int)) >= play) && (play >= 0))
    return squares[play];
  else
    return ai_random(game_board, color, heuristic, parameters, mute);
}


static int ai_minimax_rec(node curnode, int depth, double begin_time,
                          bool maxPlayer, int (*heuristic) (board, bool,
                                                            int *),
                          int *parameters) {
  double limit_time = parameters[1];
  if ((double) (clock() - begin_time) / CLOCKS_PER_SEC > limit_time - 0.1 &&
      depth != parameters[0]) {
    return 0;
  }
  __uint128_t moves = legal_moves(curnode.game, curnode.color);
  int nb_moves = scores(moves);
  if (depth == 0) {
    if (parameters[0] % 2)
      return heuristic(curnode.game, !curnode.color, parameters);
    else
      return heuristic(curnode.game, curnode.color, parameters);
  }
  else if (nb_moves == 0) {
    __uint128_t moves2 = legal_moves(curnode.game, !curnode.color);
    int nb_moves2 = scores(moves2);
    if (nb_moves2 == 0)
      return heuristic(curnode.game, !curnode.color, parameters);
    else {
      node newNode;
      newNode.game = curnode.game;
      newNode.color = !curnode.color;
      return ai_minimax_rec(newNode, depth - 1, begin_time, !maxPlayer,
                            heuristic, parameters);
    }
  }
  int square[nb_moves];
  get_squares(moves, curnode.game.size, square);
  int move;
  if (maxPlayer) {
    int bestValue = INT_MIN;
    for (int i = 0; i < nb_moves; i++) {
      board tmpboard = curnode.game;
      play_move(&tmpboard, curnode.color, square[i]);
      node newNode;
      newNode.game = tmpboard;
      newNode.color = !curnode.color;
      int v = ai_minimax_rec(newNode, depth - 1, begin_time, false, heuristic,
                             parameters);
      if (v > bestValue) {
        bestValue = v;
        move = square[i];
      }
    }
    if (depth == parameters[0])
      return move;
    return bestValue;
  }
  else {
    int bestValue = INT_MAX;
    for (int i = 0; i < nb_moves; i++) {
      board tmpboard = curnode.game;
      play_move(&tmpboard, curnode.color, square[i]);
      node newNode;
      newNode.game = tmpboard;
      newNode.color = !curnode.color;
      int v = ai_minimax_rec(newNode, depth - 1, begin_time, true, heuristic,
                             parameters);
      if (v < bestValue) {
        bestValue = v;
        move = square[i];
      }
    }
    if (depth == parameters[0])
      return move;
    return bestValue;
  }
}

int ai_minimax(board game_board, bool color,
               int (*heuristic) (board, bool, int *), int *parameters,
               bool mute) {
  node root;
  root.game = game_board;
  root.color = color;
  clock_t begin_time = clock();
  int play = ai_minimax_rec(root, parameters[0], begin_time, true, heuristic,
                            parameters);
  if (!mute) {
    printf("%c%d\n", A + play / game_board.size, 1 + play % game_board.size);
  }
  return play;
}

static int ai_negamax_alpha_beta_rec(node curnode, int depth,
                                     double begin_time, bool maxPlayer,
                                     int alpha, int beta,
                                     int (*heuristic) (board, bool, int *),
                                     int *parameters) {
  double limit_time = parameters[1];
  if ((double) (clock() - begin_time) / CLOCKS_PER_SEC > limit_time - 0.1 &&
      depth != parameters[0]) {
    return 0;
  }
  __uint128_t moves = legal_moves(curnode.game, curnode.color);
  int nb_moves = scores(moves);
  if (depth == 0) {
    if (parameters[0] % 2)
      return maxPlayer ? heuristic(curnode.game, !curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            !curnode.color,
                                                            parameters);
    else
      return maxPlayer ? heuristic(curnode.game, curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            curnode.color,
                                                            parameters);
  }
  else if (nb_moves == 0) {
    __uint128_t moves2 = legal_moves(curnode.game, !curnode.color);
    int nb_moves2 = scores(moves2);
    if (nb_moves2 == 0)
      return maxPlayer ? heuristic(curnode.game, !curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            !curnode.color,
                                                            parameters);
    else {
      node newNode;
      newNode.game = curnode.game;
      newNode.color = !curnode.color;
      return -ai_negamax_alpha_beta_rec(newNode, depth - 1, begin_time,
                                        !maxPlayer, -beta, -alpha, heuristic,
                                        parameters);
    }
  }
  int square[nb_moves];
  get_squares(moves, curnode.game.size, square);
  int move;
  int bestValue = INT_MIN;
  for (int i = 0; i < nb_moves; i++) {
    board tmpboard = curnode.game;
    play_move(&tmpboard, curnode.color, square[i]);
    node newNode;
    newNode.game = tmpboard;
    newNode.color = !curnode.color;
    int v =
      -ai_negamax_alpha_beta_rec(newNode, depth - 1, begin_time, !maxPlayer,
                                 -beta, -alpha, heuristic, parameters);
    if (v > bestValue) {
      bestValue = v;
      move = square[i];
    }
    if (bestValue > alpha)
      alpha = bestValue;
    if (beta <= alpha)
      break;
  }
  if (depth == parameters[0])
    return move;
  return bestValue;
}

int ai_negamax_alpha_beta(board game_board, bool color,
                          int (*heuristic) (board, bool, int *),
                          int *parameters, bool mute) {
  node root;
  root.game = game_board;
  root.color = color;
  clock_t begin_time = clock();
  int play =
    ai_negamax_alpha_beta_rec(root, parameters[0], begin_time, true, -20000,
                              20000, heuristic, parameters);
  if (!mute) {
    printf("%c%d\n", A + play / game_board.size, 1 + play % game_board.size);
  }
  return play;
}

static int ai_negamax_alpha_beta_sym_rec(node curnode, int depth,
                                         double begin_time, bool maxPlayer,
                                         int alpha, int beta,
                                         int (*heuristic) (board, bool,
                                                           int *),
                                         int *parameters) {
  double limit_time = parameters[1];
  if ((double) (clock() - begin_time) / CLOCKS_PER_SEC > limit_time - 0.1 &&
      depth != parameters[0]) {
    return 0;
  }
  __uint128_t moves = legal_moves(curnode.game, curnode.color);
  int nb_moves = scores(moves);
  if (depth == 0) {
    if (parameters[0] % 2)
      return maxPlayer ? heuristic(curnode.game, !curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            !curnode.color,
                                                            parameters);
    else
      return maxPlayer ? heuristic(curnode.game, curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            curnode.color,
                                                            parameters);
  }
  else if (nb_moves == 0) {
    __uint128_t moves2 = legal_moves(curnode.game, !curnode.color);
    int nb_moves2 = scores(moves2);
    if (nb_moves2 == 0)
      return maxPlayer ? heuristic(curnode.game, !curnode.color,
                                   parameters) : -heuristic(curnode.game,
                                                            !curnode.color,
                                                            parameters);
    else {
      node newNode;
      newNode.game = curnode.game;
      newNode.color = !curnode.color;
      return -ai_negamax_alpha_beta_sym_rec(newNode, depth - 1, begin_time,
                                            !maxPlayer, -beta, -alpha,
                                            heuristic, parameters);
    }
  }
  int square[nb_moves];
  get_squares(moves, curnode.game.size, square);
  int move;
  int bestValue = INT_MIN;
  for (int i = 0; i < nb_moves; i++) {
    board tmpboard = curnode.game;
    play_move(&tmpboard, curnode.color, square[i]);
    if (i > 0) {
      bool exist = false;
      for (int j = 0; j < i; j++) {
        board testBoard = curnode.game;
        play_move(&testBoard, curnode.color, square[j]);
        if (transposed_board(testBoard, tmpboard)) {
          printf("coucou\n");
          exist = true;
          break;
        }
      }
      if (exist)
        continue;
    }
    node newNode;
    newNode.game = tmpboard;
    newNode.color = !curnode.color;
    int v = -ai_negamax_alpha_beta_sym_rec(newNode, depth - 1, begin_time,
                                           !maxPlayer, -beta, -alpha,
                                           heuristic,
                                           parameters);
    if (v > bestValue) {
      bestValue = v;
      move = square[i];
    }
    if (bestValue > alpha)
      alpha = bestValue;
    if (beta <= alpha)
      break;
  }
  if (depth == parameters[0])
    return move;
  return bestValue;
}

int ai_negamax_alpha_beta_sym(board game_board, bool color,
                              int (*heuristic) (board, bool, int *),
                              int *parameters, bool mute) {
  node root;
  root.game = game_board;
  root.color = color;
  clock_t begin_time = clock();
  int play =
    ai_negamax_alpha_beta_sym_rec(root, parameters[0], begin_time, true,
                                  -20000, 20000, heuristic, parameters);
  if (!mute) {
    printf("%c%d\n", A + play / game_board.size, 1 + play % game_board.size);
  }
  return play;
}

int ai_monte_carlo(board game_board, bool color,
                   int (*heuristic) (board, bool, int *), int *parameters,
                   bool mute) {
  (void) heuristic;
  (void) parameters;
  tree root;
  root.nbgame = 1;
  root.nbwin = 0;
  root.idparent = -1;
  root.color = color;
  root.id = 0;
  root.game = game_board;
  root.first_child = -1;
  root.next_brother = -1;
  root.payout = 0;
  tree *tree_tab = malloc(sizeof(tree) * 2000000);
  int next_id = 1;
  tree_tab[0] = root;
  tree node = root;
  clock_t t = clock();
  while ((double) (clock() - t) / CLOCKS_PER_SEC < 1 && next_id < 1999970) {
    /* Selection */
    while (node.first_child != -1) {
      tree tmpnode = tree_tab[node.first_child];
      node = tmpnode;
      while (tmpnode.next_brother != -1) {
        tmpnode = tree_tab[tmpnode.next_brother];
        if (tmpnode.payout > node.payout)
          node = tmpnode;
      }
    }
    /* Expension */
    srand(clock());
    __uint128_t moves = legal_moves(node.game, node.color);
    int nb_moves = scores(moves);
    int selec = node.id;
    if (nb_moves == 0) {
      __uint128_t moves2 = legal_moves(node.game, !node.color);
      int nb_moves2 = scores(moves2);
      if (nb_moves2 > 0) {
        int square[nb_moves2];
        get_squares(moves2, node.game.size, square);
        int r = rand() % nb_moves2;
        for (int i = 0; i < nb_moves2; i++) {
          if (i == 0) {
            node.first_child = next_id;
            tree_tab[node.id] = node;
          }
          board tmpboard = node.game;
          play_move(&tmpboard, !node.color, square[i]);
          tree newNode;
          newNode.game = tmpboard;
          newNode.color = node.color;
          newNode.nbgame = 0;
          newNode.nbwin = 0;
          newNode.id = next_id;
          newNode.idparent = node.id;
          newNode.first_child = -1;
          newNode.payout = 0.9;
          if (i < nb_moves2 - 1)
            newNode.next_brother = next_id + 1;
          else
            newNode.next_brother = -1;
          tree_tab[next_id] = newNode;
          if (i == r)
            selec = next_id;
          next_id++;
        }
      }
    }
    else {
      int square[nb_moves];
      get_squares(moves, node.game.size, square);
      int r = rand() % nb_moves;
      for (int i = 0; i < nb_moves; i++) {
        if (i == 0) {
          node.first_child = next_id;
          tree_tab[node.id] = node;
        }
        board tmpboard = node.game;
        play_move(&tmpboard, node.color, square[i]);
        tree newNode;
        newNode.game = tmpboard;
        newNode.color = !node.color;
        newNode.nbgame = 0;
        newNode.nbwin = 0;
        newNode.id = next_id;
        newNode.idparent = node.id;
        newNode.first_child = -1;
        newNode.payout = 0.9;
        if (i < nb_moves - 1)
          newNode.next_brother = next_id + 1;
        else
          newNode.next_brother = -1;
        tree_tab[next_id] = newNode;
        if (i == r)
          selec = next_id;
        next_id++;
      }
    }
    /* Simulation */
    node = tree_tab[selec];
    board tmpboard = node.game;
    bool tmpcolor = node.color;
    while (!game_over(tmpboard)) {
      __uint128_t moves = legal_moves(tmpboard, tmpcolor);
      int nb_moves = scores(moves);
      if (nb_moves > 0) {
        int square[nb_moves];
        get_squares(moves, tmpboard.size, square);
        int r = rand() % nb_moves;
        play_move(&tmpboard, tmpcolor, square[r]);
      }
      tmpcolor = !tmpcolor;
    }
    /* Back propagation */
    bool winner;
    if (scores(tmpboard.black) - scores(tmpboard.white) > 0)
      winner = true;
    else
      winner = false;
    while (node.idparent != -1) {
      if (node.color == winner)
        node.nbwin++;
      node.nbgame++;
      node.payout = ((double) (node.nbwin / node.nbgame));
      tree_tab[node.id] = node;
      node = tree_tab[node.idparent];
    }
    tree_tab[0].nbgame++;
    if (color == winner)
      tree_tab[0].nbwin++;
  }
  node = tree_tab[tree_tab[0].first_child];
  double max = node.payout;
  int mv = 1;
  int best_move = 0;
  while (node.next_brother != -1) {
    node = tree_tab[node.next_brother];
    if (node.payout > max) {
      max = node.payout;
      best_move = mv;
    }
    mv++;
  }
  __uint128_t moves = legal_moves(tree_tab[0].game, tree_tab[0].color);
  int square[mv];
  get_squares(moves, tree_tab[0].game.size, square);
  int play = square[best_move];
  if (!mute) {
    printf("%c%d\n", A + play / game_board.size, 1 + play % game_board.size);
  }
  free(tree_tab);
  return play;
}
