#include "Board.h"

#include <cctype>
#include <cstdio>
#include <iostream>

#include "Evaluate.h"

using namespace std;

void Board::SetCell(int i, int j, int value)
{
    board[i][j] = value;
}

void Board::SetLastMove(int x, int y)
{
    lastMoveX = x;
    lastMoveY = y;
}