#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

#include "game.h"
#include "player.h"
#include "board.h"

static void usage_open_file(char*f, char* s){
  printf("%s: Can't find file '%s'\n",f,s);
  exit(EXIT_FAILURE);
}

static void usage_file_format(char* f){
  printf("%s: Wrong file format\n",f);
  exit(EXIT_FAILURE);
}

static void usage_save(char*f){
  printf("%s: Unable to save file\n",f);
  exit(EXIT_FAILURE);
}

static void usage_option(char*f ,char* s){
  printf("%s: Wrong options : %s\n",f,s);
  exit(EXIT_FAILURE);
}

int loadFile(char* filename,char* usage, board *game_board, bool *currentColor){
  game_board->white=0;
  game_board->black=0;
  int dim = 0;            /*Dimension of the game's grid*/

  if(filename==NULL){        /*Check if there is atleast 1 argument*/
    usage_open_file(usage,"null");
  }
  int file;
  if(( file= open(filename,O_RDONLY))<0){ /*Try to open the last argument as a file*/
    usage_open_file(usage,filename);
  }
  else{
    ssize_t readSize;
    char c;
    int sizeY =0;          /*Count the Y axis' size*/
    bool foundCurrentColor = false;    /*Check if we found the next player to play*/
    while((readSize = read(file,&c,sizeof(char))) != 0){


      while(!foundCurrentColor){
        /*This loop try to find the next player to play, exit the load program if it fails*/
        bool com = false;   /*Tell if we are in comment state for this line*/
        while(c!='\n'){

          if(c=='#' || com==true){
            com = true;
          }
          else if((c == 'X' || c == 'x' || c=='o' || c=='O') && foundCurrentColor == false){
            if(c == 'X' || c == 'x'){
              *currentColor=BLACK;
            }
            else if(c == 'O' || c == 'o'){
              *currentColor=WHITE;
            }
            foundCurrentColor = true;
          }
          else if(c!=' ' && (int)c!=9){
            usage_file_format(usage);
          }
          if((readSize = read(file,&c,sizeof(char)))==0){
            usage_file_format(usage);
          }
        }if((readSize = read(file,&c,sizeof(char)))==0){
          usage_file_format(usage);
        }
      }
      int sizeX = 0; /*Counts the X axis' size*/
      bool com=false; /*Tells if we are in comment state for this line*/
      while(c!='\n'){
        if(c=='#' || com==true){
          com = true;
        }
        else if((c == 'X') || (c == 'x') || (c=='o') || (c=='O') || (c=='_')){
          if(c == 'X' || c == 'x'){
            addPawn(&game_board->black,sizeX+sizeY*dim);
          }
          else if(c == 'O' || c == 'o'){
            addPawn(&game_board->white,sizeX+sizeY*dim);
          }
          sizeX++;
        }
        else if(c !=' ' && (int)c != 9){
          usage_file_format(usage);
        }
        if((readSize = read(file,&c,sizeof(char)))==0){
          c='\n';
        }
      }


      if(sizeX>0){ /*increment sizeY if previous line wasn't empty or null*/
        sizeY++;
        if(dim == 0){
          dim = sizeX;
        }
        if((dim != sizeX) || (sizeX%2!=0)){  /*Check if all size are the same and aren't odd*/
          usage_file_format(usage);
        }
      }
    }
    if(sizeY!= dim){
      usage_file_format(usage);
    }
  }
  close(file);
  game_board->size=dim;
  return dim;
}

int saveFile(board game_board, bool currentColor,int size){
  int file;
  if(( file= open("saves/SavedGame.txt",O_RDWR | O_CREAT ,00700))<0){
    usage_save("");
  }
  char c;
  if(currentColor == BLACK) c = 'X';
  else c = 'O';
  write(file,&c,sizeof(char));
  c = '\n';
  write(file,&c,sizeof(char));
  __uint128_t tmp=1;
  for(int i=0;i<(size*size); i++){
    if(game_board.white&(tmp<<i)){
      c = 'O';
    }
    else if(game_board.black&(tmp<<i)){
      c = 'X';
    }
    else{
      c = '_';
    }
    write(file,&c,sizeof(char));
    if((i+1)%size == 0){
      c = '\n';
      write(file,&c,sizeof(char));
    }
  }
  close(file);
  return 1;

}


void printBoard(board game_board, bool currentColor){
  __uint128_t tmp=1;
  __uint128_t legal = legalMoves(game_board, currentColor);
  int size=game_board.size;
  char cols ='A';
  printf("   ");
  for(int i=0; i<size; i++){
    printf("%c ",cols++);
  }
  printf("\n");
  for(int i=0;i<(size*size); i++){
    if(i%size==0){
      if(i/size+1==10){
        printf("%d ",i/size+1);
      }
      else{
        printf(" %d ",i/size+1);
      }
    }
    if(game_board.white&(tmp<<i)){
      printf("O ");
    }
    else if(game_board.black&(tmp<<i)){
      printf("X ");
    }
    else if(legal&(tmp<<i)){
      printf("* ");
    }
    else{
      printf("_ ");
    }
    if(i%size==size-1){
      printf("\n");
    }
  }
  printf("\n");
}


void game(board game_board, bool currentColor, int(blackPlay)(board, bool), int(whitePlay)(board, bool)){
  while(!gameOver(game_board)){
    if(!legalMoves(game_board, currentColor)){
      currentColor=!currentColor;
    }
    printBoard(game_board, currentColor);
    if(currentColor){
      printf("Black turn\n");
      while(!playMove(&game_board, currentColor, blackPlay(game_board, currentColor))){
        printf("Wrong move\n");
      }
    }
    else{
      printf("White turn\n");
      while(!playMove(&game_board, currentColor, whitePlay(game_board, currentColor))){
        printf("Wrong move\n");
      }
    }
    currentColor=!currentColor;
    printf("\n");
  }
  printBoard(game_board, currentColor);
  printf("FINI\n");
  printf("Score Black: %d\n",scores(game_board.black));
  printf("Score White: %d\n",scores(game_board.white));

}


int main(int argc, char*argv[]){
  int number_AI = 3;
  int(*ia_array[3])(board, bool)={ia0,ia_heuristique,ia_minimax};
  int(*blackPlay)(board, bool) = playerH;
  int(*whitePlay)(board, bool) = playerH;
  bool currentColor=BLACK;
  int size = 8;
  board game_board;
  newGame(&game_board,size);

  bool s=false,w=false,b=false;
  int c;

  while(1){
    static struct option long_options[] ={
      {"a",     no_argument,       0, 'a'},
      {"w",     no_argument,       0, 'w'},
      {"W",     required_argument, 0, 'W'},
      {"b",     no_argument,       0, 'b'},
      {"B",     required_argument, 0, 'B'},
      {"c",     required_argument, 0, 'c'},
      {"s",     required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    int option_index = 0;
    c = getopt_long (argc, argv, "awW:bB:c:s:", long_options, &option_index);

    if (c ==-1){
      break;
    }

    switch (c){
      case 'a':
      if(w || b){
        usage_option(argv[0], "-a IA already assign");
      }
      blackPlay = ia0;
      whitePlay = ia_minimax;
      b=true;
      w=true;
      break;

      case 'w':
      if(w){
        usage_option(argv[0], "-w White IA already assign");
      }
      whitePlay = ia0;
      w=true;
      break;

      case 'b':
      if(b){
        usage_option(argv[0], "-b Black IA already assign");
      }
      blackPlay = ia0;
      b=true;
      break;

      case 'W':
      if(w){
        usage_option(argv[0], "-W White IA already assign");
      }
      if(atoi(optarg)>=0 && atoi(optarg)<number_AI){
        whitePlay = ia_array[atoi(optarg)];
        w=true;
      }
      else{
        usage_option(argv[0], "-W [0-2]");
      }
      break;

      case 'B':
      if(b){
        usage_option(argv[0], "-B Black IA already assign");
      }
      if(atoi(optarg)>=0 && atoi(optarg)<number_AI){
        blackPlay = ia_array[atoi(optarg)];
        b=true;
      }
      else{
        usage_option(argv[0], "-B [0-2]");
      }
      break;

      case 'c':
      if(w || b || s || (optind < argc)){
        usage_option(argv[0], "-c [filename]");
      }
      else{
        size = loadFile(optarg,argv[0],&game_board, &currentColor);
        if(legalMoves(game_board,currentColor)){
          ia0(game_board,currentColor);
        }
        else{
          printf("No move possible\n");
        }
        return 0;
      }
      break;

      case 's':
      if(s){
        usage_option(argv[0], "Can't assigne -s[1-5] multiple time ");
      }
      size=2*atoi(optarg);
      if(size/2>=1 && size/2<=5){
        newGame(&game_board,size);
        s=true;
      }
      else{
        usage_option(argv[0], "-s [1-5]");
      }
      break;

      case '?':
      exit(EXIT_FAILURE);

      default:
      exit(EXIT_FAILURE);
    }
  }
  if (optind < argc){
    if(s){
      usage_option(argv[0], "Can't specify a size '-s [size]' when loading a save");
    }
    else{
      size = loadFile(argv[optind],argv[0],&game_board, &currentColor);
      s=true;
    }
    if (optind+1 < argc){
      usage_option(argv[0], "Specify option before loadfile");
    }
  }
  game(game_board, currentColor, blackPlay, whitePlay);
  return 0;
}
