#ifndef AIALGORITHMS_H
#define AIALGORITHMS_H

#include "Board.h"
#include "Evaluate.h"
#include "ForbiddenMove.h"
#include "utility"

const int DEPTH = 4; // 搜索深度
const int NUM = 10; // 搜索数量

struct POINTS { // 最佳落子位置,[0]分数最高,[9]分数最低
    std::pair<int, int> pos[NUM];
    int score[NUM]; // 此处落子的局势分数
};

struct DECISION {
    std::pair<int, int> pos; // 位置
    int eval; // 对分数的评估
};

class AIAlgorithms {
public:
    int AlphaBeta(int board[16][16], int depth, int alpha, int beta, int player);
    POINTS seekPoints(int board[16][16], int player);
    EVALUATION evaluate(int board[16][16], int player);
    DECISION getDecision() const;

private:
    DECISION decision;
};

#endif // AIALGORITHMS_H