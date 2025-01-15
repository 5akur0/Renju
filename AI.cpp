#include "AI.h"
#include "AIAlgorithms.h"
#include "Board.h"
#include <bits/stdc++.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

AI::AI()
    : lastMoveX(-1), lastMoveY(-1) {
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board, int player) {
    using namespace std;

    // 开始计时
    auto start = chrono::high_resolution_clock::now();

    AIAlgorithms aiAlgorithms;
    // if (aiAlgorithms.analysizeKill(board.board, KILLDEPTH, player)) {
    //     cout << "AI杀棋" << endl;
    // } else {
    aiAlgorithms.alphaBeta(board.board, DEPTH, INT_MIN, INT_MAX, player);
    // }
    DECISION decision = aiAlgorithms.getDecision();
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;

    if (lastMoveX <= 0 || lastMoveX > 15 || lastMoveY <= 0 || lastMoveY > 15) {
        lastMoveX = 8;
        lastMoveY = 8;
    }

    cout << "AI落子: " << lastMoveX << " " << lastMoveY << endl;

    // 结束计时，输出时间
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    if (duration.count() < 1000) {
        this_thread::sleep_for(chrono::milliseconds(1000) - duration);
    }
    cout << "AI思考时间: " << duration.count() << "毫秒" << endl;
}

int AI::GetLastMoveX() const {
    return lastMoveX;
}

int AI::GetLastMoveY() const {
    return lastMoveY;
}