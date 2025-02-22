#include "jsoncpp/json.h"
#include <bits/stdc++.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

const int DEPTH = 4;      // 搜索深度
const int NUM = 10;       // 搜索数量
const int KILLDEPTH = 16; // 杀棋搜索深度

#define SIZE 15

#define C_NONE 0
#define C_BLACK 1
#define C_WHITE 2

#define OTHER 0   // 0,其他棋型不考虑
#define WIN 1     // 100000,白赢
#define LOSE 2    //-10000000
#define FLEX4 3   // 50000,白活4
#define flex4 4   //-80000
#define BLOCK4 5  // 400
#define block4 6  //-80000
#define FLEX3 7   // 400
#define flex3 8   //-8000
#define BLOCK3 9  // 20
#define block3 10 //-40
#define FLEX2 11  // 20
#define flex2 12  //-40
#define BLOCK2 13 // 1
#define block2 14 //-2
#define FLEX1 15  // 1
#define flex1 16  //-2

int tuple6type[4][4][4][4][4][4];

enum gameResult {
    R_DRAW,
    R_WHITE,
    R_BLACK
};

struct EVALUATION {
    int score;
    gameResult result;
    int STAT[8]; // 储存部分棋形的个数
};

bool isForbiddenMove(int board[16][16], int x, int y);
void make_state(int board[16][16]);
bool long_connect();
bool five_connect();
bool three_three();
bool four_four();

class Board {
  public:
    int board[16][16];
    void SetCell(int i, int j, int value) {
        board[i][j] = value;
    }
    void SetLastMove(int x, int y) {
        lastMoveX = x;
        lastMoveY = y;
    }

  public:
    void init() {
        memset(board, 0, sizeof(board));
    }

  private:
    int lastMoveX;
    int lastMoveY;
};

struct POINTS {
    std::pair<int, int> pos[NUM];
    int score[NUM];
};

struct DECISION {
    std::pair<int, int> pos;
    int eval;
};

class AIAlgorithms {
  public:
    void iterativeDeepening(int board[16][16], int player);
    int alphaBeta(int board[16][16], int depth, int alpha, int beta, int player);
    POINTS seekPoints(int board[16][16], int player);
    EVALUATION evaluate(int board[16][16], int player);

    std::vector<std::pair<int, int>> seekKill(int board[16][16], int player);
    bool analysizeKill(int board[16][16], int depth, int player);

    DECISION getDecision() const {
        return decision;
    }

  private:
    DECISION decision;
};

void copyBoard(const int src[16][16], int dest[16][16]) {
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            dest[i][j] = src[i][j];
        }
    }
}

void reverseBoard(const int src[16][16], int dest[16][16]) {
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            dest[i][j] = src[i][j] == 1 ? 2 : (src[i][j] == 2 ? 1 : src[i][j]);
        }
    }
}

void AIAlgorithms::iterativeDeepening(int board[16][16], int player) {
    for (int depth = 2; depth <= DEPTH; depth += 2) {
        alphaBeta(board, depth, INT_MIN, INT_MAX, player);
    }
    return;
}

int AIAlgorithms::alphaBeta(int board[16][16], int depth, int alpha, int beta, int player) {
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
        POINTS P = seekPoints(rBoard, 3 - player);
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

POINTS AIAlgorithms::seekPoints(int board[16][16], int player) {
    bool B[16][16];
    int worth[16][16];
    POINTS ret;

    memset(B, 0, sizeof(B));
    for (int i = 1; i <= 15; ++i) { // 每个非空点附近8个方向延伸3个深度
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
            if (player == C_BLACK && B[i][j] && board[i][j] == C_NONE && isForbiddenMove(board, i, j)) {
                B[i][j] = false;
            }
            if (board[i][j] == C_NONE && B[i][j]) {
                board[i][j] = player;
                worth[i][j] = evaluate(board, player).score;
                board[i][j] = C_NONE;
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
                    ret.pos[k] = tmp;
                }
            }
        }
        board[ret.pos[k].first][ret.pos[k].second] = player;
        ret.score[k] = evaluate(board, player).score;
        board[ret.pos[k].first][ret.pos[k].second] = C_NONE;
        worth[ret.pos[k].first][ret.pos[k].second] = INT_MIN; // 清除掉上一点,计算下一点的位置和分数
    }
    return ret;
}

std::vector<std::pair<int, int>> AIAlgorithms::seekKill(int board[16][16], int player) {
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

bool AIAlgorithms::analysizeKill(int board[16][16], int depth, int player) {
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

EVALUATION AIAlgorithms::evaluate(int board[16][16], int player) {
    // 各棋型权重
    int weight[17] = {0, 1000000, -10000000, 50000, -100000, 400, -100000, 400, -8000, 20, -50, 20, -50, 1, -3, 1, -3};

    int i, j, type;
    int stat[4][17]; // 统计4个方向上每种棋型的个数
    memset(stat, 0, sizeof(stat));

    int STAT[17]; // 存在这种棋型的方向的个数
    memset(STAT, 0, sizeof(STAT));

    int A[17][17]; // 包括边界的虚拟大棋盘,3表示边界
    for (int i = 0; i < 17; ++i) {
        A[i][0] = 3;
    }
    for (int i = 0; i < 17; ++i) {
        A[i][16] = 3;
    }
    for (int j = 0; j < 17; ++j) {
        A[0][j] = 3;
    }
    for (int j = 0; j < 17; ++j) {
        A[16][j] = 3;
    }
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            if (player == C_WHITE) {
                A[i][j] = board[i][j];
            } else {
                if (board[i][j] == C_NONE) {
                    A[i][j] = C_NONE;
                } else {
                    A[i][j] = C_WHITE + C_BLACK - board[i][j];
                }
            }
        }
    }

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

class AI {
  public:
    AI()
        : lastMoveX(-1), lastMoveY(-1) {
    }
    std::pair<int, int> MakeMove(Board &board, int player);
    int GetLastMoveX() const { return lastMoveX; }
    int GetLastMoveY() const { return lastMoveY; }

  private:
    int lastMoveX;
    int lastMoveY;
};

std::pair<int, int> AI::MakeMove(Board &board, int player) {
    using namespace std;
    AIAlgorithms aiAlgorithms;
    // if (!aiAlgorithms.analysizeKill(board.board, KILLDEPTH, player)) {
    //     aiAlgorithms.iterativeDeepening(board.board, player);
    // }
    aiAlgorithms.iterativeDeepening(board.board, player);
    DECISION decision = aiAlgorithms.getDecision();
    lastMoveX = decision.pos.first;
    lastMoveY = decision.pos.second;
    if (lastMoveX <= 0 || lastMoveY <= 0) {
        lastMoveX = 8;
        lastMoveY = 8;
    }
    return std::make_pair(lastMoveX, lastMoveY);
}

int state[225];
int preAction;
const int BOARD_LEN = 15;

bool isForbiddenMove(int board[16][16], int x, int y) {
    board[x][y] = C_BLACK;
    make_state(board);
    board[x][y] = C_NONE;
    preAction = (x - 1) * BOARD_LEN + y - 1;
    if (long_connect()) {
        return true;
    }
    if (five_connect()) {
        return false;
    }
    if (three_three()) {
        return true;
    }
    if (four_four()) {
        return true;
    }
    return false;
}

void make_state(int board[16][16]) {
    for (int i = 1; i <= BOARD_LEN; ++i) {
        for (int j = 1; j <= BOARD_LEN; ++j) {
            int idx = (i - 1) * BOARD_LEN + (j - 1);
            if (board[i][j] == C_BLACK) {
                state[idx] = 1;
            } else if (board[i][j] == C_WHITE) {
                state[idx] = 0;
            } else {
                state[idx] = -1;
            }
        }
    }
}

bool long_connect() {
    int h = preAction / BOARD_LEN;
    int w = preAction % BOARD_LEN;
    int last_move = preAction;
    int ret;

    int bias = std::min(w, 5);
    for (int i = last_move - bias; i < last_move + 1; i++) {
        if (BOARD_LEN - 1 - i % BOARD_LEN < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6; k++) {
            if (state[k] != 1) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(h, 5);
    for (int i = last_move - bias * BOARD_LEN; i < last_move + BOARD_LEN; i += BOARD_LEN) {
        if (BOARD_LEN - 1 - i / BOARD_LEN < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * BOARD_LEN; k += BOARD_LEN) {
            if (state[k] != 1) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(std::min(h, 5), std::min(w, 5));
    for (int i = last_move - bias * BOARD_LEN - bias; i < last_move + BOARD_LEN + 1; i += BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i / BOARD_LEN < 5) || (BOARD_LEN - 1 - i % BOARD_LEN < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * BOARD_LEN + 6; k += BOARD_LEN + 1) {
            if (state[k] != 1) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(std::min(BOARD_LEN - 1 - h, 5), std::min(w, 5));
    for (int i = last_move + bias * BOARD_LEN - bias; i > last_move - BOARD_LEN + 1; i += (-(BOARD_LEN) + 1)) {
        if ((BOARD_LEN - 1 - i % BOARD_LEN < 5) || (i / BOARD_LEN < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k > i - 6 * BOARD_LEN + 6; k += (-(BOARD_LEN) + 1)) {
            if (state[k] != 1) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    return false;
}

bool tt_special_case(std::string &m_str, size_t pos, int t_case) {
    if (t_case == 1) { // oo111o
        if (pos + 6 < m_str.size()) {
            if (m_str[pos + 6] == '1')
                return true;
        }
    } else { // o111oo
        if (pos > 0) {
            if (m_str[pos - 1] == '1')
                return true;
        }
    }
    return false;
}

bool ff_special_case(std::string &m_str, size_t pos, int f_case) {
    if (f_case == 1) { // oo111o
        if (pos > 0) {
            if (m_str[pos - 1] == '1')
                return true;
        }
        if (pos + 5 < m_str.size()) {
            if (m_str[pos + 5] == '1')
                return true;
        }
        return false;
    } else if (f_case == 2) {
        if (pos > 0) {
            if (pos + 6 < m_str.size()) {
                if (m_str[pos - 1] == '1' && ((m_str[pos + 5] == 'o' && m_str[pos + 6] == '1') || (m_str[pos + 5] == '0')))
                    return true;
                return false;
            }
            if (pos + 5 < m_str.size()) {
                if (m_str[pos - 1] == '1' && m_str[pos + 5] == '0')
                    return true;
                return false;
            }
            if (m_str[pos - 1] == '1')
                return true;
            return false;
        } else
            return false;
    } else {
        if (pos + 5 < m_str.size()) {
            if (pos - 2 >= 0) {
                if ((m_str[pos - 2] == '1' && m_str[pos - 1] == 'o') || (m_str[pos - 1] == '0' && m_str[pos + 5] == '1'))
                    return true;
                return false;
            } else if (pos - 1 >= 0) {
                if (m_str[pos + 5] == '1' && m_str[pos - 1] == '0')
                    return true;
                return false;
            }
            if (m_str[pos + 5] == '1')
                return true;
            return false;
        } else
            return false;
    }
}

bool three_three() {
    int h = preAction / BOARD_LEN;
    int w = preAction % BOARD_LEN;
    int last_move = preAction;
    int width = BOARD_LEN;
    string jt1 = "o1o11o";
    string jt2 = "o11o1o";
    string ct1 = "oo111o";
    string ct2 = "o111oo";
    int three = 0;
    string m_str;
    size_t pos;
    int bias = min(w, 4);
    for (int i = last_move - bias; i < last_move + min(width - 1 - w, 4) + 1; i++) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(h, 4);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 4) + width; i += width) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(min(h, 4), min(w, 4));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 4) + width + 1; i += width + 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 4), min(h, 4));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 4)), 4) + width - 1; i += width - 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    return false;
}

bool four_four() {
    int h = preAction / BOARD_LEN;
    int w = preAction % BOARD_LEN;
    int last_move = preAction;
    int width = BOARD_LEN;
    size_t pos;
    string jf1 = "111o1";
    string jf2 = "1o111";
    string jf3 = "11o11";
    string cf1 = "o1111";
    string cf2 = "1111o";
    int four = 0;
    string m_str;
    int bias;

    bias = min(w, 5);
    for (int i = last_move - bias; i < last_move + min(width - 1 - w, 5) + 1; i++) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(h, 5);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 5) + width; i += width) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(min(h, 5), min(w, 5));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 5) + width + 1; i += width + 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 5), min(h, 5));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 5)), 5) + width - 1; i += width - 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;
    return false;
}

bool five_connect() {
    int h = preAction / BOARD_LEN;
    int w = preAction % BOARD_LEN;
    int last_player = 1;
    int last_move = preAction;
    int i, j;
    int ret;
    if (preAction == -1)
        return false;
    int bias = min(w, 4);
    for (i = last_move - bias; i < last_move + 1; i++) {
        if (BOARD_LEN - 1 - i % (BOARD_LEN) < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5; j++) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = min(h, 4);
    for (i = last_move - bias * BOARD_LEN; i < last_move + BOARD_LEN; i += BOARD_LEN) {
        if (BOARD_LEN - 1 - i / BOARD_LEN < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * BOARD_LEN; j += BOARD_LEN) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = min(min(h, 4), min(w, 4));
    for (i = last_move - bias * BOARD_LEN - bias; i < last_move + BOARD_LEN + 1; i += BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i / BOARD_LEN < 4) || (BOARD_LEN - 1 - i % BOARD_LEN < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * BOARD_LEN + 5; j += BOARD_LEN + 1) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = std::min(std::min(BOARD_LEN - 1 - h, 4), std::min(w, 4));
    for (i = last_move + bias * BOARD_LEN - bias; i > last_move - BOARD_LEN + 1; i = i - BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i % BOARD_LEN < 4) || (i / BOARD_LEN < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j > i - 5 * BOARD_LEN + 5; j = j - BOARD_LEN + 1) {
            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    return false;
}

void init_tuple6type() {
    memset(tuple6type, 0, sizeof(tuple6type)); // 全部设为0
    // 白连5,ai赢
    tuple6type[2][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][0] = WIN;
    tuple6type[0][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][1] = WIN;
    tuple6type[1][2][2][2][2][2] = WIN;
    tuple6type[3][2][2][2][2][2] = WIN; // 边界考虑
    tuple6type[2][2][2][2][2][3] = WIN;
    // 黑连5,ai输
    tuple6type[1][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][0] = LOSE;
    tuple6type[0][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][2] = LOSE;
    tuple6type[2][1][1][1][1][1] = LOSE;
    tuple6type[3][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][3] = LOSE;
    // 白活4
    tuple6type[0][2][2][2][2][0] = FLEX4;
    // 黑活4
    tuple6type[0][1][1][1][1][0] = flex4;
    // 白活3
    tuple6type[0][2][2][2][0][0] = FLEX3;
    tuple6type[0][0][2][2][2][0] = FLEX3;
    tuple6type[0][2][0][2][2][0] = FLEX3;
    tuple6type[0][2][2][0][2][0] = FLEX3;
    // 黑活3
    tuple6type[0][1][1][1][0][0] = flex3;
    tuple6type[0][0][1][1][1][0] = flex3;
    tuple6type[0][1][0][1][1][0] = flex3;
    tuple6type[0][1][1][0][1][0] = flex3;
    // 白活2
    tuple6type[0][2][2][0][0][0] = FLEX2;
    tuple6type[0][2][0][2][0][0] = FLEX2;
    tuple6type[0][2][0][0][2][0] = FLEX2;
    tuple6type[0][0][2][2][0][0] = FLEX2;
    tuple6type[0][0][2][0][2][0] = FLEX2;
    tuple6type[0][0][0][2][2][0] = FLEX2;
    // 黑活2
    tuple6type[0][1][1][0][0][0] = flex2;
    tuple6type[0][1][0][1][0][0] = flex2;
    tuple6type[0][1][0][0][1][0] = flex2;
    tuple6type[0][0][1][1][0][0] = flex2;
    tuple6type[0][0][1][0][1][0] = flex2;
    tuple6type[0][0][0][1][1][0] = flex2;
    // 白活1
    tuple6type[0][2][0][0][0][0] = FLEX1;
    tuple6type[0][0][2][0][0][0] = FLEX1;
    tuple6type[0][0][0][2][0][0] = FLEX1;
    tuple6type[0][0][0][0][2][0] = FLEX1;
    // 黑活1
    tuple6type[0][1][0][0][0][0] = flex1;
    tuple6type[0][0][1][0][0][0] = flex1;
    tuple6type[0][0][0][1][0][0] = flex1;
    tuple6type[0][0][0][0][1][0] = flex1;

    int p1, p2, p3, p4, p5, p6, x, y, ix, iy; // x:左5中黑个数,y:左5中白个数,ix:右5中黑个数,iy:右5中白个数
    for (p1 = 0; p1 < 4; ++p1) {
        for (p2 = 0; p2 < 3; ++p2) {
            for (p3 = 0; p3 < 3; ++p3) {
                for (p4 = 0; p4 < 3; ++p4) {
                    for (p5 = 0; p5 < 3; ++p5) {
                        for (p6 = 0; p6 < 4; ++p6) {
                            x = y = ix = iy = 0;

                            if (p1 == 1)
                                x++;
                            else if (p1 == 2)
                                y++;

                            if (p2 == 1) {
                                x++;
                                ix++;
                            } else if (p2 == 2) {
                                y++;
                                iy++;
                            }

                            if (p3 == 1) {
                                x++;
                                ix++;
                            } else if (p3 == 2) {
                                y++;
                                iy++;
                            }

                            if (p4 == 1) {
                                x++;
                                ix++;
                            } else if (p4 == 2) {
                                y++;
                                iy++;
                            }

                            if (p5 == 1) {
                                x++;
                                ix++;
                            } else if (p5 == 2) {
                                y++;
                                iy++;
                            }

                            if (p6 == 1)
                                ix++;
                            else if (p6 == 2)
                                iy++;

                            if (p1 == 3 || p6 == 3) {     // 有边界
                                if (p1 == 3 && p6 != 3) { // 左边界
                                    // 白冲4
                                    if (ix == 0 && iy == 4) { // 若右边有空位是活4也没关系，因为活4权重远大于冲4，再加上冲4权重变化可以不计
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    // 黑冲4
                                    if (ix == 4 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    // 白眠3
                                    if (ix == 0 && iy == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    // 黑眠3
                                    if (ix == 3 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    // 白眠2
                                    if (ix == 0 && iy == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    // 黑眠2
                                    if (ix == 2 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                } else if (p6 == 3 && p1 != 3) { // 右边界
                                    // 白冲4
                                    if (x == 0 && y == 4) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    // 黑冲4
                                    if (x == 4 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    // 黑眠3
                                    if (x == 3 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    // 白眠3
                                    if (x == 0 && y == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    // 黑眠2
                                    if (x == 2 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    // 白眠2
                                    if (x == 0 && y == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                            } else { // 无边界
                                // 白冲4
                                if ((x == 0 && y == 4) || (ix == 0 && iy == 4)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                }
                                // 黑冲4
                                if ((x == 4 && y == 0) || (ix == 4 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                }
                                // 白眠3
                                if ((x == 0 && y == 3) || (ix == 0 && iy == 3)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                }
                                // 黑眠3
                                if ((x == 3 && y == 0) || (ix == 3 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                }
                                // 白眠2
                                if ((x == 0 && y == 2) || (ix == 0 && iy == 2)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                }
                                // 黑眠2
                                if ((x == 2 && y == 0) || (ix == 2 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    // set parameters
    int turnID = 0;
    int tmppp = 0;
    int oppoColor = C_WHITE;
    int myColor = C_BLACK;

    // initialize
    init_tuple6type();
    AI ai;
    Board board;
    board.init();

    while (true) {
        string str;
        getline(cin, str);
        Json::Reader reader;
        Json::Value input;
        reader.parse(str, input);

        int xx, yy;
        if (turnID == 0) {
            xx = input["requests"][tmppp]["x"].asInt();
            yy = input["requests"][tmppp]["y"].asInt();
        } else {
            xx = input["x"].asInt();
            yy = input["y"].asInt();
        }

        if (turnID == 0 && xx != -1) {
            oppoColor = C_BLACK;
            myColor = C_WHITE;
        }
        turnID++;

        // update board
        board.SetCell(xx + 1, yy + 1, oppoColor);

        // make move
        pair<int, int> move = ai.MakeMove(board, myColor);
        board.SetCell(move.first, move.second, myColor);

        Json::Value action;
        action["x"] = move.first - 1;
        action["y"] = move.second - 1;
        Json::Value ret;
        ret["response"] = action;
        Json::FastWriter writer;
        cout << writer.write(ret) << endl;
        cout << ">>>BOTZONE_REQUEST_KEEP_RUNNING<<<" << endl;
        fflush(stdout);
    }
    return 0;
}