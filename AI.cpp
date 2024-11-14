#include "AI.h"
#include "Board.h"
#include "AIAlgorithms.h"
#include <cstdlib>
#include <ctime>
#include <bits/stdc++.h>

extern int lastMoveX; // 声明全局变量
extern int lastMoveY; // 声明全局变量

AI::AI()
{
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board)
{
    AIAlgorithms aiAlgorithms;
    // 调用 AlphaBeta 函数，设置搜索深度为 4
    aiAlgorithms.AlphaBeta(board.board, 4, INT_MIN, INT_MAX);
    DECISION decision = aiAlgorithms.getDecision();
    // 从 decision 中获取最佳落子位置
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;

    board.SetCell(lastMoveX, lastMoveY, C_WHITE);
}