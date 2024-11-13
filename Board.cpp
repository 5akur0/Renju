#include "Board.h"
#include <iostream>
#include <cstdio>
#include <cctype>

using namespace std;

void Board::Initialize()
{
    for (int i = 0; i < size + 1; i++)
    {
        for (int j = 0; j < size + 1; j++)
        {
            board[i][j] = 0;
        }
    }
}

void Board::Print()
{
    for (int i = size; i >= 1; i--)
    {
        printf("%2d ", i);
        for (int j = 1; j <= size; j++)
        {
            PrintChar(i, j);
        }
        printf("\n");
    }
    printf("  ");
    for (int j = 1; j <= size; j++)
    {
        printf(" %c", ToAlpha(j));
    }
    printf("\n");
}

void Board::SetSize(int size)
{
    this->size = size;
    Initialize();
}

int Board::GetSize() const
{
    return size;
}

int Board::GetCell(int i, int j) const
{
    return board[i][j];
}

void Board::SetCell(int i, int j, int value)
{
    board[i][j] = value;
}

char Board::ToAlpha(int i) const
{
    if (1 <= i && i <= 26)
    {
        return 'a' + i - 1;
    }
    else if (27 <= i && i <= 52)
    {
        return 'A' + i - 27;
    }
    return ' '; // invalid
}

int Board::ToIndex(char c) const
{
    if (islower(c))
    {
        return c - 'a' + 1;
    }
    else if (isupper(c))
    {
        return c - 'A' + 27;
    }
    return -1; // invalid
}

void Board::PrintChar(int i, int j) const
{
    if (board[i][j] == 0)
    {
        if (i == size)
        {
            if (j == 1)
            {
                printf("┌─");
            }
            else if (j == size)
            {
                printf("┐ ");
            }
            else
            {
                printf("┬─");
            }
        }
        else if (i == 1)
        {
            if (j == 1)
            {
                printf("└─");
            }
            else if (j == size)
            {
                printf("┘ ");
            }
            else
            {
                printf("┴─");
            }
        }
        else
        {
            if (j == 1)
            {
                printf("├─");
            }
            else if (j == size)
            {
                printf("┤ ");
            }
            else
            {
                printf("┼─");
            }
        }
    }
    else if (board[i][j] == 1)
    {
        printf("● ");
    }
    else
    {
        printf("○ ");
    }
}