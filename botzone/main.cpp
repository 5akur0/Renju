#include <bits/stdc++.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <vector>
#include "json.cpp/json.h"

#define SIZE 15

bool Grid[SIZE][SIZE];

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
};

const int DEPTH = 4; // 搜索深度
const int NUM = 10; // 搜索数量
const int KILLDEPTH = 16; // 杀棋搜索深度

void Board::Initialize()
{
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            board[i][j] = 0;
        }
    }
}

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
    // Implementation of iterative deepening
}

int AIAlgorithms::alphaBeta(int board[16][16], int depth, int alpha, int beta, int player) {
    // Implementation of alpha-beta pruning
    return 0;
}

POINTS AIAlgorithms::seekPoints(int board[16][16], int player) {
    // Implementation of seek points
    POINTS points;
    return points;
}

EVALUATION AIAlgorithms::evaluate(int board[16][16], int player) {
    // Implementation of evaluation function
    EVALUATION eval;
    return eval;
}

class AI {
public:
    AI() : lastMoveX(-1), lastMoveY(-1) {}
    std::pair<int, int> MakeMove(Board& board, int player);
    int GetLastMoveX() const { return lastMoveX; }
    int GetLastMoveY() const { return lastMoveY; }
private:
    int lastMoveX;
    int lastMoveY;
};

std::pair<int, int> AI::MakeMove(Board& board, int player) {
    using namespace std;
    auto start = chrono::high_resolution_clock::now();
    AIAlgorithms aiAlgorithms;
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
const int currentPlayer = 1;
const int BOARD_LEN = 15;

bool isForbiddenMove(int board[16][16], int x, int y) {
    // Implementation of forbidden move check
    return false;
}

void make_state(int board[16][16]) {
    for (int i = 1; i <= BOARD_LEN; ++i) {
        for (int j = 1; j <= BOARD_LEN; ++j) {
            int idx = (i - 1) * BOARD_LEN + (j - 1);
            if (board[i][j] == 1) {
                state[idx] = 1;
            } else if (board[i][j] == 2) {
                state[idx] = -1;
            } else {
                state[idx] = 0;
            }
        }
    }
}

bool long_connect() {
    // Implementation of long connect check
    return false;
}

bool tt_special_case(std::string& m_str, size_t pos, int t_case) {
    // Implementation of tt special case check
    return false;
}

bool ff_special_case(std::string& m_str, size_t pos, int f_case) {
    // Implementation of ff special case check
    return false;
}

bool three_three() {
    // Implementation of three-three check
    return false;
}

bool four_four() {
    // Implementation of four-four check
    return false;
}

bool five_connect() {
    // Implementation of five connect check
    return false;
}

void init_tuple6type() {
    // Implementation of tuple6type initialization
}

int main() {
    memset(Grid, 0, sizeof(Grid));

    // 读取输入的 JSON 数据
    Json::Value input;
    Json::Reader reader;
    std::string line;
    getline(std::cin, line);
    reader.parse(line, input);

    // 更新棋盘状态
    const Json::Value& requests = input["requests"];
    for (const auto& request : requests) {
        int x = request["x"].asInt();
        int y = request["y"].asInt();
        int player = request["player"].asInt(); // 1 表示玩家，2 表示 AI
        Grid[x][y] = true; // 根据需要更新，可能需要存储玩家信息
    }

    // 初始化 AI 和棋盘
    AI ai;
    Board board;
    board.Initialize();
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            if (Grid[i][j]) {
                board.SetCell(i + 1, j + 1, 2); // 假设玩家为白棋
            }
        }
    }

    // AI 下棋
    ai.MakeMove(board, 1); // 假设 AI 为黑棋

    // 获取 AI 的落子位置
    int x = ai.GetLastMoveX() - 1; // Adjust index if needed
    int y = ai.GetLastMoveY() - 1;

    // 更新棋盘状态
    Grid[x][y] = true;
    board.SetCell(x + 1, y + 1, 1);

    // 准备输出的 JSON 数据
    Json::Value response;
    response["x"] = x;
    response["y"] = y;

    Json::Value output;
    output["response"] = response;

    // 输出结果
    Json::StreamWriterBuilder writer;
    std::string outputStr = Json::writeString(writer, output);
    std::cout << outputStr << std::endl;

    return 0;
}