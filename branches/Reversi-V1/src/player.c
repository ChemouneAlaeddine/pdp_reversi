#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "player.h"
#include "board.h"


static void emptyBuffer(){
  int c = 0;
  while (c != '\n' && c != EOF){
    c = getchar();
  }
}

//fgets + delete the final enter + empty the buffer if the string is too long
static int fgetsCustom(char* move){
  char* positionEnter = NULL;

  if (fgets(move, LONGUEUR, stdin) != NULL){
    positionEnter = strchr(move, '\n');
    if (positionEnter != NULL){
      *positionEnter = '\0';
    }else{
      emptyBuffer();
    }
    return 1;
  }
  else{
    emptyBuffer();
    return 0;
  }
}

int playerH(board game_board, bool color){
  int size=game_board.size;
  char move[LONGUEUR]="00";
  int len=0;
  char first = move[0];
  char second = move[1];
  while(len!=2 || (!((first >= 'A' && first <= 'A'+(size-1)) || (first >= 'a' && first <='a'+(size-1))) || !(second >= '1' && second <= '1'+(size-1)))){
    printf("Case('A-%c ou a-%c''1-%c') : ", 'A'+(size-1), 'a'+(size-1), '1'+(size-1));
    fgetsCustom(move);
    len = strlen(move);
    first = move[0];
    second = move[1];
    if(len>2){
      printf("The move is too long (only 2 character needed)\n");
    }else if(len<2){
      printf("The move is too short (2 character needed)\n");
    }

    else if(first >= 'A' && first <= 'A'+(size-1) && (second >= '1' && second <= '1'+(size-1))){
      return (first-'A')+(second-'1')*size;
    }
    else if(first >= 'a' && first <= 'a'+(size-1) && (second >= '1' && second <= '1'+(size-1))){
      return (first-'a')+(second-'1')*size;
    }
    else{
      printf("Syntax incorect, 1st character must be a letter between a and %c and second a number between 1 and %c\n",'a'+size-1,'1'+size-1);
    }
  }
  return -1;
}



int ia0(board game_board, bool color){
  srand(clock());
  __uint128_t moves = legalMoves(game_board, color);
  int size = game_board.size;
  int number = scores(moves);
  int squares[number];
  getSquares(moves, size ,squares);
  int play=squares[rand()%number];
  printf("%c%c\n",'A'+play%size,'1'+play/size);
  return play;
}



int ia_heuristique(board board, bool color){
  srand(clock());
  __uint128_t moves = legalMoves(board, color),tmp = 1;
  int size = board.size, max = -10;
  int number = scores(moves);
  int squares[number], evaluation[number];
  getSquares(moves, size, squares);
  tmp = tmp<<(size*size - 1);
  for (int i = 0; i < number; i++)
    evaluation[i] = 0;
  for (int i = 0; i < number; i++){
    bool corner = (squares[i] == 0)||(squares[i] == size - 1)||(squares[i] == size*size - 1)||(squares[i] == size*(size - 1)),
      around_corner_1 = (squares[i] == 1)||(squares[i] == size)||(squares[i] == size + 1),
      corner_1_taken = (board.black&(tmp>>(size*size - 1)))||(board.white&(tmp>>(size*size - 1))),
      around_corner_2 = (squares[i] == size - 2)||(squares[i] == 2*size - 1)||(squares[i] == 2*size - 2),
      corner_2_taken = (board.black&(tmp>>(size*(size - 1))))||(board.white&(tmp>>(size*(size - 1)))),
      around_corner_3 = (squares[i]==size*size - 2)||(squares[i] == size*(size - 1) - 1)||(squares[i] == size*(size - 1) - 2),
      corner_3_taken = (board.black&(tmp))||(board.white&(tmp)),
      around_corner_4 = (squares[i] == size*(size - 1) + 1)||(squares[i] == size*(size - 2))||(squares[i] == size*(size - 2) + 1),
      corner_4_taken = (board.black&(tmp>>(size - 1)))||(board.white&(tmp>>(size - 1))),
      edge = ((squares[i] > 1)&&(squares[i] < size - 2))||
          ((squares[i] > size)&&(squares[i] < size*(size - 2))&&(squares[i]%size == 0))||
          ((squares[i] > 3*size - 2)&&(squares[i] < size*(size - 2))&&((squares[i] + 1)%size == 0))||
          ((squares[i] > (size*(size - 1) + 1))&&(squares[i] < size*size - 2)),
      before_edge_1 = (squares[i] > size + 1)&&(squares[i] < 2*size - 2),
      edge_1_taken = (board.black&(tmp>>(size*size - 1 - squares[i] + size)))||(board.white&(tmp>>(size*size - 1 - squares[i] + size))),
      before_edge_2 = (squares[i] > size + 1)&&(squares[i]<size*(size - 2) + 1)&&((squares[i] - 1)%size == 0),
      edge_2_taken = (board.black&(tmp>>(size*size - squares[i])))||(board.white&(tmp>>(size*size - squares[i]))),
      before_edge_3 = (squares[i] > 3*size - 3)&&(squares[i] < size*(size - 2) - 1)&&((squares[i] + 2)%size == 0),
      edge_3_taken = (board.black&(tmp>>(size*size - squares[i] - 2)))||(board.white&(tmp>>(size*size - squares[i] - 2))),
      before_edge_4 = (squares[i] > (size*(size - 2) + 1))&&(squares[i] < size*(size - 1) - 2),
      edge_4_taken = (board.black&(tmp>>(size*size - 1 - squares[i] - size)))||(board.white&(tmp>>(size*size - 1 - squares[i] - size))),
      around_any_corner = (around_corner_1 && !corner_1_taken)||(around_corner_2 && !corner_2_taken)||
                          (around_corner_3 && !corner_3_taken)||(around_corner_4 && !corner_4_taken),
      around_any_corner_taken = (around_corner_1 && corner_1_taken)||(around_corner_2 && corner_2_taken)||
                                (around_corner_3 && corner_3_taken)||(around_corner_4 && corner_4_taken),
      before_any_edge = (before_edge_1 && !edge_1_taken)||(before_edge_2 && !edge_2_taken)||
                        (before_edge_3 && !edge_3_taken)||(before_edge_4 && !edge_4_taken),
      before_any_edge_taken = (before_edge_1 && edge_1_taken)||(before_edge_2 && edge_2_taken)||
                        (before_edge_3 && edge_3_taken)||(before_edge_4 && edge_4_taken);

    evaluation[i] += numPoints(board, color, squares[i]);
    if (corner){
      evaluation[i] += 4;
    }else{
      if (around_any_corner){
        evaluation[i] += -4;
      }else{
        if (around_any_corner_taken){
          evaluation[i] += 2;
        }else{
          if (size>4){
            if (edge){
              evaluation[i] += 2;
            }else{
              if (before_any_edge){
                evaluation[i] += -2;
              }else{
                if (before_any_edge_taken){
                  evaluation[i] += 1;
                }else{// center
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
    if (evaluation[s] == max){
      best_shots[l] = s;
      l++;
    }
  int play = best_shots[rand()%nb_best_shots];
  printf("%c%c\n",'A'+squares[play]%size,'1'+squares[play]/size);
  return squares[play];
}



int ia_minimax(board game_board, bool color){
  node root;
  root.game = game_board;
  root.color = color;
  int play = ia_minimax_rec(root,MINIMAX_DEPTH,true);
  printf("%c%c\n",'A'+play%game_board.size,'1'+play/game_board.size);
  return  play;
}

int ia_minimax_rec(node curnode, int depth, bool maxPlayer){
  __uint128_t moves = legalMoves(curnode.game,curnode.color);
  int nb_moves = scores(moves);
  if(depth == 0) return h_logic(curnode.game,!curnode.color);
  else if(nb_moves ==0){
    __uint128_t moves2 = legalMoves(curnode.game,!curnode.color);
    int nb_moves2 = scores(moves2);
    if(nb_moves2 == 0) return h_logic(curnode.game,!curnode.color);
    else{
      node newNode;
      newNode.game = curnode.game;
      newNode.color = !curnode.color;
      return ia_minimax_rec(newNode,depth-1,!maxPlayer);
    }
  }
  int square[nb_moves];
  getSquares(moves,curnode.game.size,square);
  int move;
  if(maxPlayer){
    int bestValue = -20000;
    for(int i = 0; i<nb_moves;i++){
      board tmpboard = curnode.game;
      playMove(&tmpboard,curnode.color,square[i]);
      node newNode;
      newNode.game = tmpboard;
      newNode.color = !curnode.color;
      int v = ia_minimax_rec(newNode,depth-1,false);
      if(v>bestValue){
        bestValue = v;
        move = square[i];
      }
    }
    if(depth == MINIMAX_DEPTH) return move;
    return bestValue;
  }
  else{
    int bestValue = 20000;
    for(int i = 0; i<nb_moves; i++){
      board tmpboard = curnode.game;
      playMove(&tmpboard,curnode.color,square[i]);
      node newNode;
      newNode.game = tmpboard;
      newNode.color = !curnode.color;
      int v = ia_minimax_rec(newNode,depth-1,true);
      if(v<bestValue){
        bestValue = v;
        move = square[i];
      }
    }
    if(depth == MINIMAX_DEPTH) return move;
    return bestValue;
  }
}
