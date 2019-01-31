#include "board.h"

#include <stdio.h>


void print_ulint_board(__uint128_t ulint, int size) {
  __uint128_t tmp = 1;
  for (int i = 0; i < (size * size); i++) {
    if (ulint & (tmp << i)) {
      printf("1 ");
    }
    else {
      printf("0 ");
    }
    if (i % size == size - 1) {
      printf("\n");
    }
  }
}


__uint128_t transposed(__uint128_t matrice, int size) {
  __uint128_t tmp = 1;
  __uint128_t trans = 0;
  __uint128_t tmpinv = tmp << (size * size - 1);
  for (int i = 0; i < (size * size); i++) {
    if (matrice & (tmp << i)) {
      trans = trans | tmpinv;
    }
    tmpinv = tmpinv >> 1;
  }
  return trans;
}


__uint128_t symetric(__uint128_t matrice, int size) {
  __uint128_t tmp = 1;
  __uint128_t trans = 0;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (matrice & (tmp << (i * size + j))) {
        trans = trans | (tmp << (i * size + (size - j - 1)));
      }
    }
  }
  return trans;
}


bool transposed_board(board b1, board b2) {
  return (b1.size == b2.size)
    &&
    (((transposed(b1.black, b1.size) == b2.black)
      && (transposed(b1.white, b1.size) == b2.white))
     || ((symetric(b1.black, b1.size) == b2.black)
         && (symetric(b1.white, b1.size) == b2.white))
     || ((transposed(symetric(b1.black, b1.size), b1.size) == b2.black)
         && (transposed(symetric(b1.white, b1.size), b1.size) == b2.white)));
}


void add_pawn(__uint128_t * ulint, int square) {
  __uint128_t tmp = 1;
  *ulint = *ulint | (tmp << (square));
}


void remove_pawn(__uint128_t * ulint, int square) {
  __uint128_t tmp = 1;
  *ulint = *ulint & (~(tmp << square));
}


__uint128_t shift(__uint128_t ulint, int size, direction dir) {
  __uint128_t tmp = ulint;
  switch (dir) {
  case NORTH:
    return tmp >> size;

  case NORTHWEST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size);
    }
    return tmp >> (size + 1);

  case WEST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size);
    }
    return tmp >> 1;

  case SOUTHWEST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size);
    }
    return tmp << (size - 1);

  case SOUTH:
    return tmp << size;

  case SOUTHEAST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size + (size - 1));
    }
    return tmp << (size + 1);

  case EAST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size + (size - 1));
    }
    return tmp << 1;

  case NORTHEAST:
    for (int i = 0; i < size; i++) {
      remove_pawn(&tmp, i * size + (size - 1));
    }
    return tmp >> (size - 1);

  default:
    return 0;
  }
}


void mask(__uint128_t * ulint, int size) {
  __uint128_t tmp = 1;
  tmp = (tmp << (size * size));
  tmp -= 1;
  *ulint = *ulint & tmp;
}


void get_squares(__uint128_t ulint, int size, int *squares) {
  __uint128_t tmp = 1;
  int j = 0;
  for (int i = 0; i < size * size; i++) {
    if (ulint & (tmp << i)) {
      squares[j++] = i;
    }
  }
}


int scores(__uint128_t ulint) {
  int count;
  for (count = 0; ulint; count++) {
    ulint &= ulint - 1;
  }
  return count;
}


__uint128_t legal_moves(board game_board, bool color) {
  __uint128_t moves = 0, player = 0, opponent = 0;
  int size = game_board.size;
  if (color) {
    player = game_board.black;
    opponent = game_board.white;
  }
  else {
    player = game_board.white;
    opponent = game_board.black;
  }
  for (int i = 0; i < 8; i++) {
    direction dir = i;
    __uint128_t candidates = opponent & (shift(player, size, dir));
    mask(&candidates, size);
    while (candidates != 0) {
      moves = moves | (~(opponent | player) & (shift(candidates, size, dir)));
      candidates = opponent & (shift(candidates, size, dir));
      mask(&candidates, size);
    }
  }
  mask(&moves, size);
  return moves;
}


bool play_move(board * game_board, bool color, int move) {
  __uint128_t tmp = 1;
  __uint128_t *player, *opponent;
  int size = game_board->size;
  if (color) {
    player = &game_board->black;
    opponent = &game_board->white;
  }
  else {
    player = &game_board->white;
    opponent = &game_board->black;
  }
  tmp = tmp << move;
  if (legal_moves(*game_board, color) & (tmp)) {
    for (int i = 0; i < 8; i++) {
      tmp = 1;
      tmp = tmp << move;
      direction dir = i;
      __uint128_t candidates = 0;
      tmp = *opponent & (shift(tmp, size, dir));
      while (tmp != 0) {
        candidates = candidates | tmp;
        tmp = *opponent & (shift(tmp, size, dir));
      }
      if (candidates != 0 && shift(candidates, size, dir) & *player) {
        *opponent = *opponent ^ candidates;
        tmp = 1;
        tmp = tmp << move;
        candidates = candidates | tmp;
        *player = *player | candidates;
      }
    }
    return 1;
  }
  return 0;
}


void new_game(board * game_board, int size) {
  game_board->black = 0;
  game_board->white = 0;
  game_board->size = size;
  add_pawn(&game_board->white, size * size / 2 - size / 2 - 1);
  add_pawn(&game_board->black, size * size / 2 - size / 2);
  add_pawn(&game_board->black, size * size / 2 + size / 2 - 1);
  add_pawn(&game_board->white, size * size / 2 + size / 2);
}


bool game_over(board game_board) {
  if (legal_moves(game_board, WHITE) | legal_moves(game_board, BLACK)) {
    return false;
  }
  return true;

}


int num_points(board game_board, bool color, int move) {
  __uint128_t tmp = 1;
  __uint128_t player, opponent;
  int size = game_board.size, points = 0;
  if (color) {
    player = game_board.black;
    opponent = game_board.white;
  }
  else {
    player = game_board.white;
    opponent = game_board.black;
  }
  tmp = tmp << move;
  if (legal_moves(game_board, color) & tmp) {
    for (int i = 0; i < 8; i++) {
      tmp = 1;
      tmp = tmp << move;
      direction dir = i;
      __uint128_t candidates = 0;
      tmp = opponent & (shift(tmp, size, dir));
      while (tmp != 0) {
        candidates = candidates | tmp;
        tmp = opponent & (shift(tmp, size, dir));
      }
      if (candidates != 0 && shift(candidates, size, dir) & player) {
        opponent = opponent ^ candidates;
        points += scores(candidates);
      }
    }
    return points;
  }
  return 0;
}


int h_logic(board game_board, bool color, int *parameters) {
  int value = 0;
  int coefMax = parameters[2];
  int coefEdge = parameters[3];
  int coefCorner = parameters[4];
  int coefMob = parameters[5];

  __uint128_t myBoard;
  __uint128_t opponentBoard;
  unsigned player = color;
  if (player) {
    myBoard = game_board.black;
    opponentBoard = game_board.white;
  }
  else {
    myBoard = game_board.white;
    opponentBoard = game_board.black;
  }
  /*Maximisation */
  int opponentPawn = scores(opponentBoard);
  int myPawn = scores(myBoard);

  /*Return a big value if this move allow us to win the games */
  if (game_over(game_board) && myPawn > opponentPawn) {
    return 20000000;
  }
  else if (game_over(game_board)) {
    return -20000000;
  }

  int maximisation;
  if (opponentPawn != 0) {
    maximisation = (int) (myPawn / opponentPawn);
  }

  /*Positioning */
  /*check the number of corner */
  __uint128_t corner = 0;
  add_pawn(&corner, 0);
  add_pawn(&corner, game_board.size - 1);
  add_pawn(&corner, (game_board.size - 1) * game_board.size);
  add_pawn(&corner,
           (game_board.size - 1) * game_board.size + (game_board.size - 1));
  int nbCorner = scores(myBoard & corner);
  /*Check the number of edges */
  __uint128_t edges = 0;
  int j = 1;
  for (int i = 0; i < game_board.size * game_board.size; i++) {
    /*top edge */
    if (i >= 0 && i <= game_board.size - 1)
      add_pawn(&edges, i);
    /*bottom edge */
    if (i >= (game_board.size - 1) * game_board.size
        && i <= (game_board.size - 1) * game_board.size + game_board.size + 1)
      add_pawn(&edges, i);
    /*left edge */
    if (i == game_board.size * j) {
      add_pawn(&edges, i);
      add_pawn(&edges, i + game_board.size - 1);
      j++;
    }
  }
  int nbEdges = scores(myBoard & edges);
  /*Mobility */
  int mobility = scores(legal_moves(game_board, !player));

  value =
    coefMax * maximisation + coefEdge * nbEdges + coefCorner * nbCorner -
    coefMob * mobility;

  return value;
}

int h_logic2(board game_board, bool color, int *parameters) {
  //TODO Stability

  __uint128_t myBoard;
  __uint128_t opponentBoard;
  unsigned player = color;
  if (player) {
    myBoard = game_board.black;
    opponentBoard = game_board.white;
  }
  else {
    myBoard = game_board.white;
    opponentBoard = game_board.black;
  }

  int opponentPawn = scores(opponentBoard);
  int myPawn = scores(myBoard);

  /*Return a big value if this move allow us to win the games */
  if (game_over(game_board) && myPawn > opponentPawn) {
    return 20000000;
  }
  else if (game_over(game_board)) {
    return -20000000;
  }


  int value = 0;
  int coefMax = parameters[2];
  int coefEdge = parameters[3];
  int coefCorner = parameters[4];
  int coefActualMob = parameters[5];
  int coefPotentialMob = parameters[6];


  /*Maximisation */
  int maximisation =
    ((100 * (myPawn - opponentPawn)) / (myPawn + opponentPawn));

  /*Positioning */
  /*check the number of corner */
  __uint128_t corner = 0;
  add_pawn(&corner, 0);
  add_pawn(&corner, game_board.size - 1);
  add_pawn(&corner, (game_board.size - 1) * game_board.size);
  add_pawn(&corner,
           (game_board.size - 1) * game_board.size + (game_board.size - 1));
  int nbCorner = 0;
  int maxCorner = scores(myBoard & corner);
  int minCorner = scores(opponentBoard & corner);
  if (maxCorner + minCorner != 0) {
    nbCorner = ((100 * (maxCorner - minCorner)) / (maxCorner + minCorner));
  }
  /*Check the number of edges */
  __uint128_t edges = 0;
  int j = 1;
  for (int i = 0; i < game_board.size * game_board.size; i++) {
    /*top edge */
    if (i >= 0 && i <= game_board.size - 1)
      add_pawn(&edges, i);
    /*bottom edge */
    if (i >= (game_board.size - 1) * game_board.size
        && i <= (game_board.size - 1) * game_board.size + game_board.size + 1)
      add_pawn(&edges, i);
    /*left edge */
    if (i == game_board.size * j) {
      add_pawn(&edges, i);
      add_pawn(&edges, i + game_board.size - 1);
      j++;
    }
  }
  int maxEdge = scores(myBoard & edges);
  int minEdge = scores(opponentBoard & edges);
  int nbEdges = 0;
  if (maxEdge + minEdge != 0) {
    nbEdges = ((100 * (maxEdge - minEdge)) / (maxEdge + minEdge));
  }

  /*Mobility */
  /*Actual mobility */
  int maxAMobility = scores(legal_moves(game_board, player));
  int minAMobility = scores(legal_moves(game_board, !player));
  int actualMobility = 0;
  if (maxAMobility + minAMobility != 0) {
    actualMobility =
      ((100 * (maxAMobility - minAMobility)) / (maxAMobility + minAMobility));
  }
  /*Potential mobility counting all the opponent's pieces that are adjacent to an empty field */
  __uint128_t move = 0;
  for (int i = 0; i < 8; i++) {
    direction dir = i;
    __uint128_t candidates =
      ~(opponentBoard | myBoard) &
      (shift(opponentBoard, game_board.size, dir));
    mask(&candidates, game_board.size);
    move = move | candidates;
  }
  int maxPMobility = scores(move);

  move = 0;
  for (int i = 0; i < 8; i++) {
    direction dir = i;
    __uint128_t candidates =
      ~(myBoard | opponentBoard) & (shift(myBoard, game_board.size, dir));
    mask(&candidates, game_board.size);
    move = move | candidates;
  }
  int minPMobility = scores(move);

  int potentialMobily = 0;
  if (maxPMobility + minPMobility != 0) {
    potentialMobily =
      ((100 * (maxPMobility - minPMobility)) / (maxPMobility + minPMobility));
  }
  value =
    coefMax * maximisation + coefEdge * nbEdges + coefCorner * nbCorner +
    coefActualMob * actualMobility + coefPotentialMob * potentialMobily;

  return value;
}
