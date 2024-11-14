#include "AIAlgorithms.h"
#include <bits/stdc++.h>

// 获取决策结果
DECISION AIAlgorithms::getDecision() const
{
    return decision;
}

// Function to copy the board
void copyBoard(const int src[16][16], int dest[16][16])
{
    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 1; j <= 15; ++j)
        {
            dest[i][j] = src[i][j];
        }
    }
}

// Function to reverse the board
void reverseBoard(const int src[16][16], int dest[16][16])
{
    for (int i = 1; i <= 15; ++i)
    {
        for (int j = 1; j <= 15; ++j)
        {
            if (src[i][j] == C_BLACK)
            {
                dest[i][j] = C_WHITE;
            }
            else if (src[i][j] == C_WHITE)
            {
                dest[i][j] = C_BLACK;
            }
            else
            {
                dest[i][j] = C_NONE;
            }
        }
    }
}

int AIAlgorithms::AlphaBeta(int board[16][16], int depth, int alpha, int beta)
{
    gameResult RESULT = evaluate(board).result;
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
            return evaluate(board).score;
        }
    }
    else if (depth % 2 == 0)
    { // max层,我方(白)决策
        POINTS P = seekPoints(board);
        for (int i = 0; i < 10; ++i)
        {
            int sameBoard[16][16];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].first][P.pos[i].second] = C_WHITE; // 模拟己方落子,不能用board,否则可能改变board的信息
            int a = AlphaBeta(sameBoard, depth - 1, alpha, beta);
            if (a > alpha)
            {
                alpha = a;
                if (depth == DEPTH)
                {
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
        for (int i = 0; i < 10; ++i)
        {
            int sameBoard[16][16];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].first][P.pos[i].second] = C_BLACK; // 模拟敌方落子
            int a = AlphaBeta(sameBoard, depth - 1, alpha, beta);
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
            worth[i][j] = INT_MIN;
            if (board[i][j] == C_NONE && B[i][j] == true)
            {
                board[i][j] = C_WHITE;
                worth[i][j] = evaluate(board).score;
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
        board[best_points.pos[k].first][best_points.pos[k].second] = C_WHITE;
        best_points.score[k] = evaluate(board).score;
        board[best_points.pos[k].first][best_points.pos[k].second] = C_NONE;
        worth[best_points.pos[k].first][best_points.pos[k].second] = -INT_MAX; // 清除掉上一点,计算下一点的位置和分数
    }
    return best_points;
}

EVALUATION AIAlgorithms::evaluate(int board[16][16])
{
    // 各棋型权重
    int weight[17] = {0, 1000000, -10000000, 50000, -100000, 400, -100000, 400, -8000, 20, -50, 20, -50, 1, -3, 1, -3};

    int i, j, type;
    int stat[4][17]; // 统计4个方向上每种棋型的个数
    memset(stat, 0, sizeof(stat));

    int STAT[17]; // 存在这种棋型的方向的个数
    memset(STAT, 0, sizeof(STAT));

    int A[17][17]; // 包括边界的虚拟大棋盘,3表示边界
    for (int i = 0; i < 17; ++i)
        A[i][0] = 3;
    for (int i = 0; i < 17; ++i)
        A[i][16] = 3;
    for (int j = 0; j < 17; ++j)
        A[0][j] = 3;
    for (int j = 0; j < 17; ++j)
        A[16][j] = 3;
    for (int i = 1; i <= 15; ++i)
        for (int j = 1; j <= 15; ++j)
            A[i][j] = board[i][j];

    // 判断横向棋型
    for (i = 1; i <= 15; ++i)
    {
        for (j = 0; j < 12; ++j)
        {
            type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
            stat[0][type]++;
        }
    }
    // 判断竖向棋型
    for (j = 1; j <= 15; ++j)
    {
        for (i = 0; i < 12; ++i)
        {
            type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
            stat[1][type]++;
        }
    }
    // 判断左上至右下棋型
    for (i = 0; i < 12; ++i)
    {
        for (j = 0; j < 12; ++j)
        {
            type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
            stat[2][type]++;
        }
    }
    // 判断右上至左下棋型
    for (i = 0; i < 12; ++i)
    {
        for (j = 5; j < 17; ++j)
        {
            type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
            stat[3][type]++;
        }
    }

    EVALUATION eval;
    memset(eval.STAT, 0, sizeof(eval.STAT));

    int score = 0;
    for (i = 1; i <= 16; ++i)
    {
        score += (stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]) * weight[i]; // 初步计分

        int count = stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]; // 统计所有方向上部分棋型的个数
        if (i == WIN)
            eval.STAT[WIN] = count; // WIN
        else if (i == LOSE)
            eval.STAT[LOSE] = count; // LOSE
        else if (i == FLEX4)
            eval.STAT[FLEX4] = count; // FLEX4
        else if (i == BLOCK4)
            eval.STAT[BLOCK4] = count; // BLOCK4
        else if (i == FLEX3)
            eval.STAT[FLEX3] = count; // FLEX3
    }
    eval.result = R_DRAW;

    // 白赢
    if (eval.STAT[WIN] > 0)
        eval.result = R_WHITE;
    // 黑赢
    else if (eval.STAT[LOSE] > 0)
        eval.result = R_BLACK;

    eval.score = score;
    return eval;
}