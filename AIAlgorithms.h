#ifndef AIALGORITHMS_H
#define AIALGORITHMS_H

#include "Board.h"
#include "Evaluate.h"
#include "utility"

struct POINTS
{ // 最佳落子位置,[0]分数最高,[9]分数最低
    std::pair<int, int> pos[10];
    int score[10]; // 此处落子的局势分数
};

class AIAlgorithms {
public:
    int AlphaBeta(int board[16][16], int depth, int alpha, int beta);
    POINTS seekPoints(int board[16][16]);
};

#endif // AIALGORITHMS_H