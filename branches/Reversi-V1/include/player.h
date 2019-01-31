#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include "board.h"


/** This file is dedicated to our different functions which
*   return a playable move,either by asking a move to a real
*   player or by using our AIs each of these functions need
*   to have the following prototype in order to get called correctly :
*   int my_function(board current_game, bool next_color_to_play)
**/

#define LONGUEUR 10
#define MINIMAX_DEPTH 5


typedef struct node{
  board game;
  bool color;
}node;

/**Ask a player the move he want to make
*   param are the struct bitboard containing the
*   the position of the pawn and the color of the playerH
*   after checking if the move is syntaxicaly correct return
*   the index index of the square of the board (or -1 if there
*   was a problem)
**/
int playerH(board game_board, bool color);

/**Simple AI which choose a random available move and return it**/
int ia0(board game_board, bool color);

/** Heuristic according to the location of the boxes **/
int ia_heuristique(board game_board, bool color);

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
int ia_minimax(board game_board, bool color);

/** For each legal moves possible we call minimax again until we reach
*   the desired depth or until we have no more move available,
*   it then uses h_logic to give a value to the reached board
*   to be able to find the best move possible in the end
**/
int ia_minimax_rec(node node, int depth, bool maximizingPlayer);


#endif
