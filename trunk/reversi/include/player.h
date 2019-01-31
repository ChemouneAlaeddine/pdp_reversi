#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include <limits.h>
#include "board.h"


/** This file is dedicated to our different functions which
*   return a playable move,either by asking a move to a real
*   player or by using our AIs each of these functions need
*   to have the following prototype in order to get called correctly :
*   int my_function(board current_game, bool next_color_to_play)
**/

#define BUFFER 10

enum CHAR{
  A = 'A',
  Q = 'Q',
  X = 'X',
  O = 'O',
  EMPTY = '_',
  AVAIL = '*'};

typedef struct node{
  board game;
  bool color;
}node;

typedef struct tree{
  int nbgame;
  int nbwin;
  float payout;
  bool color;
  board game;
  int id;
  int idparent;
  int first_child;
  int next_brother;
}tree;

/**Ask a player the move he want to make
*   param are the struct bitboard containing the
*   the position of the pawn and the color of the playerH
*   after checking if the move is syntaxicaly correct return
*   the index index of the square of the board (or -1 if there
*   was a problem)
**/
int playerH(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/**Simple AI which choose a random available move and return it**/
int ai_random(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

int ai_basic(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** Artificial intelligence indeterminist,
    choosing his best current shot randomly
    from a subset of best shots chosen according
    to positions on the board and the number of points earned **/
int ai_heuristic(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** MINIMAX BASED AI :
*   Analyse every next MINIMAX_DEPTH possible moves
*   and use the h_logic function to find
*   which move is the best and returns it
*
*
*   Initialize the minimax function by calling the recursive minimax function
*   with the current state of the game and the next color to play,
*   it then wait until the recursive function return the best move to play
**/
int ai_minimax(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** For each legal moves possible we call minimax again until we reach
*   the desired depth or until we have no more move available,
*   it then uses h_logic to give a value to the reached board
*   to be able to find the best move possible in the end
**/

/** NEGAMAX ALPHA BETA BASED AI :
*   Analyse every next NEGAMAX_ALPHA_BETA_DEPTH moves
*   possibles and use the h_logic function to find
*   which move is the best and returns it
*
*
*   Initialize the negamax alpha beta function by calling the recursive negamax
*   alpha beta function with the current state of the game and the next color to play,
*   it then wait until the recursive function return the best move to play
**/
int ai_negamax_alpha_beta(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** For each possible legal move, we call again negamax alpha beta
*   and we test if we have an interest to browse the subtree using
*   both alpha and beta variables, until we reach the desired depth
*   or until we have no more movement available,
*   it then uses h_logic to give a value to the table reached
*   be able to find the best possible movement at the end
**/

/** NEGAMAX ALPHA BETA BASED AI
*   by eliminating the symmetrical states found
**/
int ai_negamax_alpha_beta_sym(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** MONTE CARLO BASED AI
    plays games randomly to study the probability of winning in a configuration
**/
int ai_monte_carlo(board game_board, bool color, int(*heuristic)(board, bool, int*), int* parameters, bool mute);

/** For that, it goes through the tree of the moves he has already created by
*   choosing the best node among the candidates and re-simulates from
*   the best node known to update the probabilities
**/

#endif
