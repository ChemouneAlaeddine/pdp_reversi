#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include "board.h"

/** This file is dedicated to our functions related to the user interface.
*   it detects the different options, it can load a file,
*   choose between AI and real player and it display the stat of
*   the game on the console.
**/

/** usage function if unable to open the file **/
static void usage_open_file(char*f, char* s);

/** usage function if the loaded file do not have the correct save format **/
static void usage_file_format(char* f);

/** usage function if we are unable to save a game**/
static void usage_save(char*f);

/** usage function if the options are wrong **/
static void usage_option(char*f ,char* s);

/** Load the file of filename, check if the format is correct
*   if so, we modify game_board and currentColor to be able to
*   retrieve the state of the saved game
**/
int loadFile(char* filename,char* usage, board* game_board, bool *currentColor);

/** Save the current state of the game_board and the next player currentColor
*   in a file and respect the save file format
**/
int saveFile(board game_board, bool currentColor, int size);

/** Display on the console the current game_board and the currentColor **/
void printBoard(board game_board, bool currentColor);

/** This is the core game function :
*   Until it reach the end of the game it asks the next player
*   (either a real player or an IA) the move he wants to play
*   modify the game_board accordingly.
*   It then calculate the next possible moves available and
*   Display on the console the game board with the legal moves available
*   Once we reach a game ending situation we display the score before exiting
**/
void game(board game_board, bool currentColor, int(blackPlay)(board, bool), int(whitePlay)(board, bool));


#endif
