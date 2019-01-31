Reversi is a strategy board game for two players and 
the goal of this project is to program a Reversi player.
So to test several approaches, we have implemented different AI and heuristics.

All the user interface is in text mode.

Project tree :               
        .
        ├── bin/
        ├── include/
        │   ├── board.h
        │   ├── game.h
        │   └── player.h
        ├── Makefile
        ├── obj/
        ├── README.txt
        ├── src/
        │   ├── board.c
        │   ├── game.c
        │   ├── main.c
        │   ├── main_tools.c
        │   └── player.c
        └── test/
            ├── ai/
            ├── contest/
            ├── data/
            ├── gen/
            ├── saves/
            └── test.bash
            

The project is divided in two programs :
        
        -reversi
                Wich allows to play reversi human versus human, 
                AI vs human or AI vs AI.
                
        -reversi_tools
                Which allows to generate some advanced game and 
                test some AI by making them play some generated games
                in a row and access some data like the score or the 
                time taking by the AI to play. 
               
_______________________________________________________________________________
_______________________________________________________________________________              

In these two programs, we use textfile to set our game or AI parameters:

Example of a game save file : 

"
X
________
________
________
___XO___  #comments
___OX___
________
________
________
"

This file described a game of reversi, with the first line for the current 
player, and the other line for the board(each line represents a board's rows).

'X' Represents black
'O' Represents white
'_' Represents empty squares
'#' Represents a comment, 
    and all the character of the line after the # will be ignored

The size of the board must be even and between 2 and 10 include.
The spaces and tab are ignored.

_______________________________________________________________________________

List of AI and Heuristic:

We can split our AI in two categories:

Those that chose from the current legal move and that don't need parameters:
        -ai_random:     simply choose a random move from the available one
        -ai_basic:      choose the move based on the number of points earned
        -ai_heuristic:  choose the best available move from the current legal 
                        move based on the position on the board and the number of points earned
        -ai_monte_carlo:play multiple game randomly to study the probability of 
                        winning in a configuration and play a move in consequences

And those that evaluate the move available with an evaluation function and create a search 
tree containing all the possible move within a certain depth. To use this AI you need to specify 
the max depth and the maximum time allocated to search a move 
(past this time it return the best move it find until now):

        -ai_minimax:            use a minimax algorithm to search in the tree
        -ai_negamax_alpha_beta: use a negamax algorithm to search in the tree and the alpha_beta pruning 
                                algorithm to decrease the number of nodes evaluated
        -ai_negamax_alpha_beta_sym: use negamax and alpha_beta pruning but search for symmetry in the 
                                configuration of the board to further reduce the number of nodes evaluated 

We can use two evaluations functions:

-h_logic: evaluate certain aspects of the board like the difference of the number of pawn of each player or 
          the placement of the pawns. To use it you need to specify the weight of each aspects:
          
	-The difference of the number of pawn
	-The number of pawns on the edges
	-the number of pawns on the corners
	-the number of available move of the opponent
	
-h_logic2: this is a improved version of h_logic, 
           to use it you need to specify the weight of each aspects too:

	-The difference of pawn
	-The number of pawns on the edges of the two players
	-the number of pawns on the corners of the two players
	-the actual number of available move of the player and the opponent 
	-the potential number of available move of the player and the opponent
	
Example of AI parameters file : 
"
ai_negamax_alpha_beta           #AI name
5                               #Tree search max depth
10                              #Max response time in seconds
h_logic2                        #Heuristics name
10                              #Parameters 1 of heuristics
30                              #Parameters 2 of heuristics
20                              #Parameters 3 of heuristics
15                              #Parameters 4 of heuristics
15                              #Parameters 5 of heuristics

"

Each line with an argument must be finish with a newline.
All the parameters below the AI name are only used by the tree search AI.
 
This Format is mainly use to make our test quickly and with more flexibility.

There is multiple example of file in "test/ai/"
_______________________________________________________________________________
_______________________________________________________________________________


Reversi : 

To compile the reversi program, use the command 
"make" in the root of the project tree.

usage: ./bin/reversi [options] [savefile]

No options will start the game with 2 human players
Either on a new 8x8 game if no savefile are given
or on the game saved in the savefile

Available options :
-s --size [1-5]
     set the size of the board to 2xSIZE

-b --black=FILENAME
     will use default AI for black player
     The argument FILENAME is optionnal (and must be used with the long options), 
     and allows to change the AI by the AI which is in the argument file 
     (See above for the configuration AI files)

-w --black=FILENAME
     will use default AI for white player
     The argument FILENAME is optionnal (and must be used with the long options), 
     and allows to change the AI by the AI which is in the argument file 
     (See above for the configuration AI files)
     
-a
     will use both default AI for black and white

-c FILENAME
     use default AI of the black player
     to play the next move of FILENAME
     
Default AI : (negamax alpha beta with h_logic2 as heuristic)
This default AI is the best that we have implemented.

_______________________________________________________________________________

Reversi_tools : 


usage: ./bin/reversi_tools [options]

Different tools made to tests reversi AI
Don't do anything without -gtp options

Available options :
-g --generate [N]    Generate N random 8x8 board of reversi
                     with 10 moves already played.
                     The size of the board or the progress in
                     the game can be modify with option -s or -d

-s --size [1-5]      Set the board size of the generator to 2xSIZE

-d --depth [N]       Set the progress of the generator to 2xN,
                     so the generator will play 2xN moves

-t --test [N]        Play the first move of N generated games
                     with the AI of the black player
                     and display time informations

-p --play [N]        Play N generated games with AI and display
                     the score and time informations of the AI
                     in CSV format

-w --white FILENAME  Set the AI of the white player to the AI
                     which is specified in the file
                     (default : negamax alpha beta with h_logic2 as heuristic)

-b --black FILENAME  Set the AI of the black player to the AI
                     which is specified in the file
                     (default : negamax alpha beta with h_logic2 as heuristic)

-u --unmute          Unmute the AI and show all the moves
                     played by them in the process of
                     the different functions

-v --verbose         Unmute the AI and show the progress in
                     percentage of the different process

-h --help            Display the help

_______________________________________________________________________________
_______________________________________________________________________________

Data :

The data repertory was used to store the CSV files we got by playing a lot of matches with 
the reversi_tools and redirecting the standard output to the files. 
We then proceed to use those data to calculate the win rate and the average time spent 
by each move of our IAs in the ODS files. 
Each file name give us the information of which AI played which color. 
For example the file named B-Corner_W-actu stores the data of the games we played 
with corner counting as our black heuristic and actual mobility counting as our white heuristic.

Here is a list of the different parameters we used to generate those files 
(to replicate those parameters we used the heuristics h_logic2 
and set every parameter to 0 except the one we wanted):

        - max : count how many pawn we own.
        - corner : count how many corner we own.
        - edge : count how many pawn situated at edge we own.
        - actu : count how many available moves we have.
        - pot : count how many move we could have in the futur (enemy pawn next to an empty square)
