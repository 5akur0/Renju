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
}

std::pair<int, int> AI::MakeMove(Board& board, int player)
{
    using namespace std;

    // 开始计时
    auto start = chrono::high_resolution_clock::now();

    AIAlgorithms aiAlgorithms;
    // if (aiAlgorithms.analysizeKill(board.board, KILLDEPTH, player)) {
    //     cout << "AI杀棋" << endl;
    // } else {
    aiAlgorithms.iterativeDeepening(board.board, player);
    // }
    DECISION decision = aiAlgorithms.getDecision();
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;
    if (lastMoveX <= 0 || lastMoveY <= 0) {
        lastMoveX = 8;
        lastMoveY = 8;
    }
    return std::make_pair(lastMoveX, lastMoveY);
}

int AI::GetLastMoveX() const
{
    return lastMoveX;
}

int AI::GetLastMoveY() const
{
    return lastMoveY;
}