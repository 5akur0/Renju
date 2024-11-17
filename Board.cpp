#include "Board.h"

#include <cctype>
#include <cstdio>
#include <iostream>

#include "Evaluate.h"

using namespace std;

void Board::Initialize()
{
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            board[i][j] = 0;
        }
    }
    lastMoveX = -1;
    lastMoveY = -1;
}

void Board::Print()
{
    for (int i = 15; i >= 1; i--) {
        printf("%2d ", i);
        for (int j = 1; j <= 15; j++) {
            PrintChar(i, j);
        }
        printf("\n");
    }
    printf("  ");
    for (int j = 1; j <= 15; j++) {
        printf(" %c", ToAlpha(j));
    }
    printf("\n");
}

int Board::GetCell(int i, int j) const { return board[i][j]; }

void Board::SetCell(int i, int j, int value) { board[i][j] = value; }

char Board::ToAlpha(int i) const
{
    if (1 <= i && i <= 26) {
        return 'a' + i - 1;
    } else if (27 <= i && i <= 52) {
        return 'A' + i - 27;
    }
    return ' '; // invalid
}

int Board::ToIndex(char c) const
{
    if (islower(c)) {
        return c - 'a' + 1;
    } else if (isupper(c)) {
        return c - 'A' + 27;
    }
    return -1; // invalid
}

void Board::PrintChar(int i, int j) const
{
    if (board[i][j] == 0) {
        if (i == 15) {
            if (j == 1) {
                printf("┌─");
            } else if (j == 15) {
                printf("┐ ");
            } else {
                printf("┬─");
            }
        } else if (i == 1) {
            if (j == 1) {
                printf("└─");
            } else if (j == 15) {
                printf("┘ ");
            } else {
                printf("┴─");
            }
        } else {
            if (j == 1) {
                printf("├─");
            } else if (j == 15) {
                printf("┤ ");
            } else {
                printf("┼─");
            }
        }
    } else if (board[i][j] == 1) {
        if (i == lastMoveX && j == lastMoveY) {
            printf("◉ ");
        } else {
            printf("● ");
        }
    } else {
        if (i == lastMoveX && j == lastMoveY) {
            printf("◌ ");
        } else {
            printf("○ ");
        }
    }
}

void Board::SetLastMove(int x, int y)
{
    lastMoveX = x;
    lastMoveY = y;
}