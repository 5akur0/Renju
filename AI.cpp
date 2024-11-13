#include "AI.h"
#include "Board.h"
#include "AIAlgorithms.h"
#include <cstdlib>
#include <ctime>
#include <utility>

AI::AI() : lastMoveX(-1), lastMoveY(-1)
{
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board)
{
    AIAlgorithms aiAlgorithms;
    POINTS bestPoints = aiAlgorithms.seekPoints(board.board);
    lastMoveX = bestPoints.pos[0].first;
    lastMoveY = bestPoints.pos[0].second;

    board.SetCell(lastMoveX, lastMoveY, C_WHITE);
}

int AI::GetLastMoveX() const {
    return lastMoveX;
}

int AI::GetLastMoveY() const {
    return lastMoveY;
}