#ifndef AIALGORITHMS_H
#define AIALGORITHMS_H

#include "Board.h"
#include "Evaluate.h"

class AIAlgorithms {
public:
    int AlphaBeta(int board[16][16], int depth, int alpha, int beta);
    POINTS seekPoints(int board[16][16]);
};

#endif // AIALGORITHMS_H