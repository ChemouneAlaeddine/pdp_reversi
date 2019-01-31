#include <stdio.h>
#include <stdbool.h>

#include "board.h"


void printUlintBoard(__uint128_t ulint, int size){
  __uint128_t tmp=1;
  for(int i=0;i<(size*size); i++){
    if(ulint&(tmp<<i)){
      printf("1 ");
    }
    else{
      printf("0 ");
    }
    if(i%size==size-1){
      printf("\n");
    }
  }
}


void addPawn(__uint128_t *ulint, int square){
  __uint128_t tmp=1;
  *ulint = *ulint|(tmp<<(square));
}


void removePawn(__uint128_t *ulint, int square){
  __uint128_t tmp=1;
  *ulint = *ulint&(~(tmp<<square));
}


__uint128_t shift(__uint128_t ulint, int size,direction dir){
  __uint128_t tmp = ulint;
  switch(dir){
    case NORTH :
    return tmp >> size;

    case NORTHWEST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size);
    }
    return tmp >> (size+1);

    case WEST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size);
    }
    return tmp >> 1;

    case SOUTHWEST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size);
    }
    return tmp << (size-1);

    case SOUTH :
    return tmp << size;

    case SOUTHEAST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size+(size-1));
    }
    return tmp << (size+1);

    case EAST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size+(size-1));
    }
    return tmp << 1;

    case NORTHEAST :
    for (int i=0; i<size;i++){
      removePawn(&tmp,i*size+(size-1));
    }
    return tmp >> (size-1);

    default :
    return 0;
  }
}


void mask(__uint128_t *ulint,int size){
  __uint128_t tmp=1;
  tmp = (tmp<<(size*size));
  tmp-=1;
  *ulint = *ulint&tmp;
}


void getSquares(__uint128_t ulint, int size, int* squares){
  __uint128_t tmp=1;
  int j=0;
  for(int i=0; i<size*size; i++){
    if (ulint &(tmp<<i)){
      squares[j++]=i;
    }
  }
}


int scores(__uint128_t ulint){
  int count;
  for (count=0; ulint; count++){
    ulint &= ulint - 1;
  }
  return count;
}


__uint128_t legalMoves(board game_board, bool color){
  __uint128_t moves=0, player=0, opponent=0;
  int size = game_board.size;
  if(color){
    player = game_board.black;
    opponent = game_board.white;
  }
  else{
    player = game_board.white;
    opponent = game_board.black;
  }
  for(int i=0; i<8; i++){
    direction dir = i;
    __uint128_t candidates = opponent & (shift(player,size,dir));
    mask(&candidates,size);
    while (candidates !=0){
      moves = moves | (~(opponent|player)&(shift(candidates,size,dir)));
      candidates = opponent & (shift(candidates,size,dir));
      mask(&candidates,size);
    }
  }
  mask(&moves,size);
  return moves;
}


bool playMove(board *game_board, bool color, int move){
  __uint128_t tmp=1;
  __uint128_t *player, *opponent;
  int size = game_board->size;
  if(color){
    player = &game_board->black;
    opponent = &game_board->white;
  }
  else{
    player = &game_board->white;
    opponent = &game_board->black;
  }
  tmp=tmp<<move;
  if(legalMoves(*game_board, color)&(tmp)){
      for(int i=0; i<8; i++){
        tmp=1;
        tmp=tmp<<move;
        direction dir = i;
        __uint128_t candidates = 0;
        tmp = *opponent & (shift(tmp,size,dir));
        while (tmp !=0){
          candidates = candidates | tmp;
          tmp = *opponent & (shift(tmp,size,dir));
        }
        if (candidates!=0 && shift(candidates,size,dir)&*player){
          *opponent = *opponent ^ candidates;
          tmp=1;
          tmp=tmp<<move;
          candidates = candidates | tmp;
          *player = *player | candidates;
        }
      }
    return true;
  }
  return false;
}


void newGame(board *game_board, int size){
  game_board->black=0;
  game_board->white=0;
  game_board->size=size;
  addPawn(&game_board->white, size*size/2-size/2-1);
  addPawn(&game_board->black, size*size/2-size/2);
  addPawn(&game_board->black, size*size/2+size/2-1);
  addPawn(&game_board->white, size*size/2+size/2);
}


bool gameOver(board game_board){
  if(legalMoves(game_board, WHITE)|legalMoves(game_board, BLACK)){
    return false;
  }
  return true;

}

/*=================================HEURISTIC=================================*/


int numPoints(board game_board, bool color, int move){
  __uint128_t tmp=1;
  __uint128_t player, opponent;
  int size = game_board.size, points = 0;
  if(color){
    player = game_board.black;
    opponent = game_board.white;
  }
  else{
    player = game_board.white;
    opponent = game_board.black;
  }
  tmp=tmp<<move;
  if(legalMoves(game_board, color)&(tmp)){
      for(int i=0; i<8; i++){
        tmp=1;
        tmp=tmp<<move;
        direction dir = i;
        __uint128_t candidates = 0;
        tmp = opponent & (shift(tmp,size,dir));
        while (tmp !=0){
          candidates = candidates | tmp;
          tmp = opponent & (shift(tmp,size,dir));
        }
        if (candidates!=0 && shift(candidates,size,dir)&player){
          opponent = opponent ^ candidates;
          points += scores(candidates);
        }
      }
    return points;
  }
  return 0;
}


int h_logic(board game_board,bool color){
  int value=0;
  int coefMax = 2;
  int coefEdge = 10;
  int coefCorner = 200;
  int coefMob = 0;

  __uint128_t myBoard;
  __uint128_t opponentBoard;
  unsigned player = color;
  if(player){
    myBoard = game_board.black;
    opponentBoard = game_board.white;
  }else{
    myBoard = game_board.white;
    opponentBoard = game_board.black;
  }
  /*Maximisation*/
  int opponentPawn = scores(opponentBoard);
  int myPawn = scores(myBoard);
  int maximisation;
  if(opponentPawn != 0){
    maximisation = (int)(myPawn/opponentPawn);
  }

  /*Positioning*/
  /*check the number of corner*/
  __uint128_t corner=0;
  addPawn(&corner,0);
  addPawn(&corner,game_board.size-1);
  addPawn(&corner,(game_board.size-1)*game_board.size);
  addPawn(&corner,(game_board.size-1)*game_board.size+(game_board.size-1));
  int nbCorner = scores(myBoard & corner);
  /*Check the number of edges*/
  __uint128_t edges=0;
  int j=1;
  for(int i=0;i<game_board.size*game_board.size;i++){
    /*top edge*/
    if(i>=0 && i<=game_board.size-1)addPawn(&edges,i);
    /*bottom edge*/
    if(i>=(game_board.size-1)*game_board.size && i<=(game_board.size-1)*game_board.size+game_board.size+1)addPawn(&edges,i);
    /*left edge*/
    if(i==game_board.size*j){
      addPawn(&edges,i);
      addPawn(&edges,i+game_board.size-1);
      j++;
    }
  }
  int nbEdges = scores(myBoard & edges);
  /*Mobility*/
  int mobility = scores(legalMoves(game_board,!player));

  value = coefMax*maximisation + coefEdge*nbEdges + coefCorner*nbCorner - coefMob*mobility;

  return value;
}
