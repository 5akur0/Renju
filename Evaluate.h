#ifndef EVALUATE_H
#define EVALUATE_H

#include "Board.h"
#include <cstring>

#define C_NONE 0 // 棋子：黑子,白子,无子
#define C_BLACK 1
#define C_WHITE 2

// 棋型代号 下标 权重
#define OTHER 0   // 0,其他棋型不考虑
#define WIN 1     // 1000000,白赢
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

extern int tuple6type[4][4][4][4][4][4]; // 需要初始化

void init_tuple6type();

#endif // EVALUATE_H