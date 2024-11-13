#include "AIAlgorithms.h"
#include <bits/stdc++.h>

// Function to copy the board
void copyBoard(const int src[16][16], int dest[16][16]) {
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            dest[i][j] = src[i][j];
        }
    }
}

// Function to reverse the board
void reverseBoard(const int src[16][16], int dest[16][16]) {
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            if (src[i][j] == C_BLACK) {
                dest[i][j] = C_WHITE;
            } else if (src[i][j] == C_WHITE) {
                dest[i][j] = C_BLACK;
            } else {
                dest[i][j] = C_NONE;
            }
        }
    }
}

struct DECISION
{
    std::pair<int, int> pos; // 位置
    int eval;                // 对分数的评估
};

DECISION decision; // analyse得到的最佳落子点

int AIAlgorithms::AlphaBeta(int board[16][16], int depth, int alpha, int beta)
{
    gameResult RESULT = myEvaluate::evaluate(board).result;
    if (depth == 0 || RESULT != R_DRAW)
    { // 如果模拟落子可以分出输赢，那么直接返回结果，不需要再搜索
        if (depth == 0)
        {
            POINTS P;
            P = seekPoints(board); // 生成最佳的可能落子位置
            return P.score[0];     // 返回最佳位置对应的最高分
        }
        else
        {
            return myEvaluate::evaluate(board).score;
        }
    }
    else if (depth % 2 == 0)
    { // max层,我方(白)决策
        int sameBoard[16][16];
        copyBoard(board, sameBoard);
        POINTS P = seekPoints(sameBoard);

        for (int i = 0; i < 10; ++i)
        {
            sameBoard[P.pos[i].first][P.pos[i].second] = C_WHITE; // 模拟己方落子,不能用board,否则可能改变board的信息
            int a = AlphaBeta(sameBoard, depth - 1, alpha, beta);
            sameBoard[P.pos[i].first][P.pos[i].second] = C_NONE; // 还原落子
            if (a > alpha)
            {
                alpha = a;
                if (depth == 4)
                { // 4是自己设立的深度(可以改为6,8,但必须为偶数),用来找最佳落子
                    decision.pos.first = P.pos[i].first;
                    decision.pos.second = P.pos[i].second;
                    decision.eval = a;
                }
            }
            if (beta <= alpha)
                break; // 剪枝
        }
        return alpha;
    }
    else
    { // min层,敌方(黑)决策
        int rBoard[16][16];
        reverseBoard(board, rBoard);
        POINTS P = seekPoints(rBoard); // 找对于黑子的最佳位置,需要将棋盘不同颜色反转,因为seekPoint是求白色方的最佳位置

        int sameBoard[16][16];
        copyBoard(board, sameBoard);

        for (int i = 0; i < 10; ++i)
        {
            sameBoard[P.pos[i].first][P.pos[i].second] = C_BLACK; // 模拟敌方落子
            int a = AlphaBeta(sameBoard, depth - 1, alpha, beta);
            sameBoard[P.pos[i].first][P.pos[i].second] = C_NONE; // 还原落子
            if (a < beta)
                beta = a;
            if (beta <= alpha)
                break; // 剪枝
        }
        return beta;
    }
}

POINTS AIAlgorithms::seekPoints(int board[16][16])
{
    bool B[16][16]; // 局部搜索标记数组
    int worth[16][16];
    POINTS best_points;

    memset(B, 0, sizeof(B));
    for (int i = 1; i <= 15; ++i)
    { // 每个非空点附近8个方向延伸3个深度,若不越界则标记为可走
        for (int j = 1; j <= 15; ++j)
        {
            if (board[i][j] != C_NONE)
            {
                for (int k = -3; k <= 3; ++k)
                {
                    if (i + k >= 1 && i + k <= 15)
                    {
                        B[i + k][j] = true;
                        if (j + k >= 1 && j + k <= 15)
                            B[i + k][j + k] = true;
                        if (j - k >= 1 && j - k <= 15)
                            B[i + k][j - k] = true;
                    }
                    if (j + k >= 1 && j + k <= 15)
                        B[i][j + k] = true;
                }
            }
        }
    }

    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 1; j <= 15; ++j)
        {
            worth[i][j] = -INT_MAX;
            if (board[i][j] == C_NONE && B[i][j] == true)
            {
                board[i][j] = C_BLACK;
                worth[i][j] = myEvaluate::evaluate(board).score;
                board[i][j] = C_NONE;
            }
        }
    }

    int w;
    for (int k = 0; k < 10; ++k)
    {
        w = -INT_MAX;
        for (int i = 1; i <= 15; ++i)
        {
            for (int j = 1; j <= 15; ++j)
            {
                if (worth[i][j] > w)
                {
                    w = worth[i][j];
                    std::pair<int, int> tmp(i, j);
                    best_points.pos[k] = tmp;
                }
            }
        }
        best_points.score[k] = w;
        worth[best_points.pos[k].first][best_points.pos[k].second] = -INT_MAX; // 清除掉上一点,计算下一点的位置和分数
    }
    return best_points;
}