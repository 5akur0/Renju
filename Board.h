#ifndef BOARD_H
#define BOARD_H


class Board
{
public:
    int board[16][16];
    void Initialize();
    void Print();
    int GetCell(int i, int j) const;
    void SetCell(int i, int j, int value);
    int ToIndex(char c) const; // 添加这个函数声明

private:
    int size;
    char ToAlpha(int i) const;
    void PrintChar(int i, int j) const;
};

#endif // BOARD_H