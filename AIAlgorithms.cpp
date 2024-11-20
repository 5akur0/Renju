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
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            dest[i][j] = src[i][j];
        }
    }
}

// Function to reverse the board
void reverseBoard(const int src[16][16], int dest[16][16])
{
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

// 迭代加深的 AlphaBeta 搜索
void AIAlgorithms::iterativeDeepening(int board[16][16], int player)
{
    for (int depth = 2; depth <= DEPTH; depth += 2) {
        alphaBeta(board, depth, INT_MIN, INT_MAX, player);
        // 如果找到胜利走法，提前退出
        if (decision.eval >= WIN) {
            return;
        }
    }
    return;
}

int AIAlgorithms::alphaBeta(int board[16][16], int depth, int alpha, int beta, int player)
{
    EVALUATION eval = evaluate(board, player);
    if (depth == 0) {
        POINTS P = seekPoints(board, player);
        return P.score[0];
    }
    if (eval.result != R_DRAW) {
        return eval.score;
    }
    if (depth % 2 == 0) { // max层
        POINTS P = seekPoints(board, player);
        for (int i = 0; i < NUM; ++i) {
            int sameBoard[16][16];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].first][P.pos[i].second] = player; // 模拟己方落子
            int a = alphaBeta(sameBoard, depth - 1, alpha, beta, 3 - player);
            if (a >= 1000000) {
                if (depth == DEPTH) {
                    decision.pos.first = P.pos[i].first;
                    decision.pos.second = P.pos[i].second;
                    decision.eval = a;
                }
                return a;
            }
            if (a > alpha) {
                alpha = a;
                if (depth == DEPTH) {
                    decision.pos.first = P.pos[i].first;
                    decision.pos.second = P.pos[i].second;
                    decision.eval = a;
                }
            }
            if (beta <= alpha)
                break; // 剪枝
        }
        return alpha;
    } else { // min层,敌方决策
        int rBoard[16][16];
        reverseBoard(board, rBoard);
        POINTS P = seekPoints(rBoard, 3 - player); // 找对于敌方的最佳位置,需要将棋盘不同颜色反转
        for (int i = 0; i < NUM; ++i) {
            int sameBoard[16][16];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].first][P.pos[i].second] = 3 - player; // 模拟敌方落子
            int a = alphaBeta(sameBoard, depth - 1, alpha, beta, player);
            if (a < beta)
                beta = a;
            if (beta <= alpha)
                break; // 剪枝
        }
        return beta;
    }
}

POINTS AIAlgorithms::seekPoints(int board[16][16], int player)
{
    bool B[16][16]; // 局部搜索标记数组
    int worth[16][16];
    POINTS best_points;

    memset(B, 0, sizeof(B));
    for (int i = 1; i <= 15; ++i) { // 每个非空点附近8个方向延伸3个深度,若不越界则标记为可走
        for (int j = 1; j <= 15; ++j) {
            if (board[i][j] != C_NONE) {
                for (int k = -3; k <= 3; ++k) {
                    if (i + k >= 1 && i + k <= 15) {
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

    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            worth[i][j] = INT_MIN;
            if (board[i][j] == C_NONE && B[i][j] == true) {
                if (player == C_BLACK && isForbiddenMove(board, i, j)) {
                    std::cout << "发现禁手" << std::endl;
                } else {
                    board[i][j] = player;
                    worth[i][j] = evaluate(board, player).score;
                    board[i][j] = C_NONE;
                }
            }
        }
    }

    int w;
    for (int k = 0; k < NUM; ++k) {
        w = INT_MIN;
        for (int i = 1; i <= 15; ++i) {
            for (int j = 1; j <= 15; ++j) {
                if (worth[i][j] > w) {
                    w = worth[i][j];
                    std::pair<int, int> tmp(i, j);
                    best_points.pos[k] = tmp;
                }
            }
        }
        board[best_points.pos[k].first][best_points.pos[k].second] = player;
        best_points.score[k] = evaluate(board, player).score;
        board[best_points.pos[k].first][best_points.pos[k].second] = C_NONE;
        worth[best_points.pos[k].first][best_points.pos[k].second] = INT_MIN; // 清除掉上一点,计算下一点的位置和分数
    }
    return best_points;
}

EVALUATION AIAlgorithms::evaluate(int board[16][16], int player)
{
    int rboard[16][16];
    copyBoard(board, rboard);
    if (player == C_BLACK) {
        reverseBoard(board, rboard);
    }
    // 各棋型权重
    int weight[17] = { 0, 1000000, -10000000, 50000, -100000, 400, -100000, 400, -8000, 20, -50, 20, -50, 1, -3, 1, -3 };

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
            A[i][j] = rboard[i][j];

    // 判断横向棋型
    for (i = 1; i <= 15; ++i) {
        for (j = 0; j < 12; ++j) {
            type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
            stat[0][type]++;
        }
    }
    // 判断竖向棋型
    for (j = 1; j <= 15; ++j) {
        for (i = 0; i < 12; ++i) {
            type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
            stat[1][type]++;
        }
    }
    // 判断左上至右下棋型
    for (i = 0; i < 12; ++i) {
        for (j = 0; j < 12; ++j) {
            type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
            stat[2][type]++;
        }
    }
    // 判断右上至左下棋型
    for (i = 0; i < 12; ++i) {
        for (j = 5; j < 17; ++j) {
            type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
            stat[3][type]++;
        }
    }

    EVALUATION eval;
    memset(eval.STAT, 0, sizeof(eval.STAT));

    int score = 0;
    for (i = 1; i <= 16; ++i) {
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

std::vector<std::pair<int, int>> AIAlgorithms::seekKill(int board[16][16], int player)
{
    std::vector<std::pair<int, int>> ret;
    POINTS P = seekPoints(board, player);
    int sameBoard[16][16];
    copyBoard(board, sameBoard);

    for (int i = 0; i < NUM; ++i) {
        sameBoard[P.pos[i].first][P.pos[i].second] = player;
        if (evaluate(sameBoard, player).STAT[WIN] > 0) {
            ret.push_back(P.pos[i]);
        } else if (evaluate(sameBoard, player).STAT[FLEX4] > evaluate(board, player).STAT[FLEX4]) {
            ret.push_back(P.pos[i]);
        } else if (evaluate(sameBoard, player).STAT[BLOCK4] > evaluate(board, player).STAT[BLOCK4]) {
            ret.push_back(P.pos[i]);
        } else if (evaluate(sameBoard, player).STAT[FLEX3] > evaluate(board, player).STAT[FLEX3]) {
            ret.push_back(P.pos[i]);
        }
        sameBoard[P.pos[i].first][P.pos[i].second] = C_NONE;
    }
    return ret;
}

bool AIAlgorithms::analysizeKill(int board[16][16], int depth, int player)
{
    EVALUATION eval = evaluate(board, player);
    if (depth == 0) {
        POINTS P = seekPoints(board, player);
        board[P.pos[0].first][P.pos[0].second] = player;
        gameResult RESULT = evaluate(board, player).result;
        if ((RESULT == R_WHITE && player == C_WHITE) || (RESULT == R_BLACK && player == C_BLACK)) {
            return true;
        }
        return false;
    }
    if ((eval.result == R_WHITE && player == C_WHITE) || (eval.result == R_BLACK && player == C_BLACK)) {
        return true;
    }
    if ((eval.result == R_WHITE && player == C_BLACK) || (eval.result == R_BLACK && player == C_WHITE)) {
        return false;
    }
    if (depth % 2 == 0) { // 我方决策
        if (depth == KILLDEPTH || depth == KILLDEPTH - 2) {
            POINTS P = seekPoints(board, player);
            for (int i = 0; i < NUM; ++i) {
                int sameBoard[16][16];
                copyBoard(board, sameBoard);
                sameBoard[P.pos[i].first][P.pos[i].second] = player;
                if (analysizeKill(sameBoard, depth - 1, player)) {
                    if (depth == KILLDEPTH) {
                        decision.pos.first = P.pos[i].first;
                        decision.pos.second = P.pos[i].second;
                        decision.eval = INT_MAX;
                    }
                    return true;
                }
            }
            return false;
        }
        std::vector<std::pair<int, int>> kill = seekKill(board, player);
        if (kill.size() == 0) {
            return false;
        }
        for (auto k : kill) {
            int sameBoard[16][16];
            copyBoard(board, sameBoard);
            sameBoard[k.first][k.second] = player;
            if (analysizeKill(sameBoard, depth - 1, player)) {
                return true;
            }
        }
        return false;
    }
    // 敌方决策，选择最好的位置
    POINTS PP = seekPoints(board, 3 - player);
    int sameBoard[16][16];
    copyBoard(board, sameBoard);
    sameBoard[PP.pos[0].first][PP.pos[0].second] = 3 - player;
    return analysizeKill(sameBoard, depth - 1, player);
}