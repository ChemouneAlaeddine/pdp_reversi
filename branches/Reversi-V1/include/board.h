#ifndef BOARD_H_
#define BOARD_H_

#include <stdbool.h>

/*Boolean for player color*/
#define BLACK 1
#define WHITE 0

/**This structure represent a reversi board, with two
*  unsigned int of 128 bit representing the position
*  of the black and white pawn and its size in an integer**/
struct board{
  __uint128_t white;
  __uint128_t black;
  int size;
}typedef board;


/*  Board format exemple for an int equal to 42387 (1010010110010011)
 *  and a size of 4:
 *
 *                      1 1 0 0
 *                      1 0 0 1
 *                      1 0 1 0
 *                      0 1 0 1
 *
 *  The order of the bit is reverted in this format **/


/**An enum to represent the 8 possible direction
*  in the reversi game**/
enum direction{
  NORTH,
  NORTHWEST,
  WEST,
  SOUTHWEST,
  SOUTH,
  SOUTHEAST,
  EAST,
  NORTHEAST
}typedef direction;


/**Print an unsigned int of 128 bits in a board format
*  between 0 and size-1 bits**/
void printUlintBoard(__uint128_t ulint, int size);


/**Put the nth bit of ulint on**/
void addPawn(__uint128_t *ulint, int square);


/**Put the nth bit of ulint off**/
void removePawn(__uint128_t *ulint, int square);


/**Shift an unsigned int of 128 bits in any cardinal or primary
*  intercardinal direction according to the board format and return it**/
__uint128_t shift(__uint128_t ulint, int size,direction dir);


/**Put the bit of ulint after size-1 to 0**/
void mask(__uint128_t *ulint,int size);


/**Fill an array of int with the index of the on-bits
*  between 0 and size-1 bits in ulint**/
void getSquares(__uint128_t ulint, int size, int* squares);


/**Return the number of on-bits in ulint**/
int scores(__uint128_t ulint);


/**Return an unsigned int of 128 bits representing the legalMoves
*  for a color in a state of a reversi game**/
__uint128_t legalMoves(board game_board, bool color);


/**Return a boolean representing the playability of a move
*  in a Reversi game and play it**/
bool playMove(board *game_board, bool color, int move);


/**Create a start Board of a certain size, for Reversi**/
void newGame(board *game_board, int size);


/**Return a boolean, true if the game is over, false if not**/
bool gameOver(board game_board);


/*=================================HEURISTIC=================================*/


/**Returns the nomber of points of a move played**/
int numPoints(board game_board, bool color, int move);


/**Heuristic based on othello strategy
*  -Maximisation: aving more pawn that the enemy
*  -Positioning: Edge and corner are difficult to retake
*  -Mobility: Lower number of possible move of the opponent
**/
int h_logic(board game_board,bool color);



#endif
