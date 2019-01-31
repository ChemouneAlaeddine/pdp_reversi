#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include "board.h"

/** This file is dedicated to our functions related to the user interface.
*   it detects the different options, it can load a file,
*   choose between AI and real player and it display the stat of
*   the game on the console.
**/

void load_ai(char* filename, char* usage,
              int (**ai)(board, bool, int(*)(board, bool, int*), int*, bool),
              int (**heuristic)(board, bool, int*), int* parameters);

/** Load the file of filename, check if the format is correct
*   if so, we modify game_board and current_color to be able to
*   retrieve the state of the saved game
**/
int load_file(char* filename,char* usage, board* game_board, bool *current_color);

/** Save the current state of the game_board and the next player current_color
*   in a file and respect the save file format
**/
int save_file(board game_board, bool current_color, int size, char* filename);

/** Display on the console the current game_board and the current_color **/
void print_board(board game_board, bool current_color);

/** ask the user if he wants to save the game, returns true if the first letter entered is 'y' **/
bool ask(char question[]);

/** This is the core game function :
*   Until it reach the end of the game it asks the next player
*   (either a real player or an IA) the move he wants to play
*   modify the game_board accordingly.
*   It then calculate the next possible moves available and
*   Display on the console the game board with the legal moves available
*   Once we reach a game ending situation we display the score before exiting
**/
void game(board game_board, bool current_color, bool mute,
          int(blackPlay)(board, bool, int(*)(board, bool, int*), int*, bool),
          int(*black_heuristic)(board, bool, int*), int* black_parameters ,
          int(whitePlay)(board, bool, int(*)(board, bool, int*), int*, bool),
          int(*white_heuristic)(board, bool, int*), int* white_parameters);

#endif
