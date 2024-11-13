#include "AI.h"
#include "Board.h"
#include "AIAlgorithms.h"
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>

AI::AI() : lastMoveX(-1), lastMoveY(-1)
{
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board)
{
    AIAlgorithms aiAlgorithms;
    POINTS bestPoints = aiAlgorithms.seekPoints(board.board);
    //find index of score[10] max
    int maxIndex = 0;
    for (int i = 0; i < 10; i++)
    {
        if (bestPoints.score[i] >= bestPoints.score[maxIndex])
        {
            maxIndex = i;
        }
    }
    std::pair<int, int> pos = bestPoints.pos[maxIndex];
    lastMoveX = pos.first;
    lastMoveY = pos.second;

    board.SetCell(lastMoveX, lastMoveY, C_WHITE);
}

int AI::GetLastMoveX() const {
    return lastMoveX;
}

int AI::GetLastMoveY() const {
    return lastMoveY;
}