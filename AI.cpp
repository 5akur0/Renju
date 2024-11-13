#include "AI.h"
#include "Board.h"
#include <cstdlib>
#include <ctime>

AI::AI() : lastMoveX(-1), lastMoveY(-1)
{
    std::srand(std::time(nullptr)); // 初始化随机数种子
}

void AI::MakeMove(Board &board)
{
    int size = board.GetSize();
    do
    {
        lastMoveX = std::rand() % size + 1;
        lastMoveY = std::rand() % size + 1;
    } while (board.GetCell(lastMoveX, lastMoveY) != 0);

    board.SetCell(lastMoveX, lastMoveY, 2);
}

int AI::GetLastMoveX() const
{
    return lastMoveX;
}

int AI::GetLastMoveY() const
{
    return lastMoveY;
}