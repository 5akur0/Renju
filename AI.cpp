#include "AI.h"
#include "AIAlgorithms.h"
#include "Board.h"
#include <bits/stdc++.h>
#include <chrono>
#include <cstdlib>
#include <ctime>

AI::AI()
    : lastMoveX(-1)
    , lastMoveY(-1)
{
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board& board, int player)
{
    using namespace std::chrono; // 使用 chrono 命名空间

    // 开始计时
    auto start = high_resolution_clock::now();

    AIAlgorithms aiAlgorithms;
    aiAlgorithms.AlphaBeta(board.board, DEPTH, INT_MIN, INT_MAX, player); // 使用常量 DEPTH
    DECISION decision = aiAlgorithms.getDecision();
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;
    if (lastMoveX <= 0 || lastMoveY <= 0) {
        lastMoveX = 8;
        lastMoveY = 8;
    }

    // 确保落子位置有效
    if (board.GetCell(lastMoveX, lastMoveY) == C_NONE) {
        board.SetCell(lastMoveX, lastMoveY, player);
    } else {
        // 无效的落子位置，直接报错
        std::cerr << "AI选择了无效的落子位置，运行终止" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "AI选择的落子位置: (" << lastMoveX << ", " << lastMoveY << ")" << std::endl;

    // 结束计时，输出时间
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    if (duration.count() < 200) {
        std::this_thread::sleep_for(milliseconds(200) - duration);
    }
    std::cout << "AI思考时间: " << duration.count() << "毫秒" << std::endl;
}

int AI::GetLastMoveX() const
{
    return lastMoveX;
}

int AI::GetLastMoveY() const
{
    return lastMoveY;
}