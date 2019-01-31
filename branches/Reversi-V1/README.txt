This program is a reversi game with a shell interface.
You can play Human vs Human, or choose between 3 differents AI to face.
You can also play AI vs AI to compare their efficiency.

Compilation : make in Reversi-V1/ 

Executable : ./game in Reversi-V1/bin/

How to use it : 

./game [option] loadfile

Option : '-s [1-5]'       Half size of the board (size between 2 & 10)
         '-b'             Black will be played by an AI
         '-w'             White will be played by an AI
         '-a'             Black and White will be played by an AI
         '-c [filename]'  Return a move played by an IA on the board save in the file
         '-B [0-2]'       Black will be played by choose AI between 3 differents AI(see below)
         '-W [0-2]'       White will be played by choose AI between 3 differents AI(see below)
         
Available AI : 0 : an AI that will choose randomly the next move
               1 : an AI that will weight each square to choose the next move
               2 : an AI based on Minimax, that will evaluate all possible move 
                   up to five move in advance to find a good move 
                   
                   
More Info : 
        -The loadfile is optionnal and some example can be found in the saves/ folder.
        -Without any option, the game will be launch on a 8x8 board and Human vs Human.
        -If you don't specify an AI to play a color, it will be played by a Human.
        -By Default, the size of a board is 8x8.

