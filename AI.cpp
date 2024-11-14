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
    // 调用 AlphaBeta 函数，设置搜索深度为 4
    aiAlgorithms.AlphaBeta(board.board, 6, INT_MIN, INT_MAX);
    DECISION decision = aiAlgorithms.getDecision();
    // 从 decision 中获取最佳落子位置
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;
    if (board.GetCell(lastMoveX, lastMoveY) == C_NONE)
    {
        board.SetCell(lastMoveX, lastMoveY, C_WHITE);
    }
    else
    {
        // 处理无效的落子位置，可能需要重新计算或报错
        std::cerr << "AI 选择了无效的落子位置！" << std::endl;
    }
    board.SetCell(lastMoveX, lastMoveY, C_WHITE);
}

int AI::GetLastMoveX() const
{
    return lastMoveX;
}

int AI::GetLastMoveY() const
{
    return lastMoveY;
}