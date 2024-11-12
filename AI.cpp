#include "AI.h"
#include "Board.h"
#include <cstdlib>
#include <ctime>

AI::AI() {
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board) {
    int size = board.GetSize();
    int x, y;
    do {
        x = std::rand() % size + 1;
        y = std::rand() % size + 1;
    } while (board.GetCell(x, y) != 0);

    board.SetCell(x, y, 2);
}