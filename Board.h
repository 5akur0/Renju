#ifndef BOARD_H
#define BOARD_H

class Board {
public:
    Board(int size = 15);
    void Initialize();
    void Print();
    void SetSize(int size);
    int GetSize() const;
    int GetCell(int i, int j) const;
    void SetCell(int i, int j, int value);
    int ToIndex(char c) const; // 添加这个函数声明

private:
    int size;
    int board[51][51];
    char ToAlpha(int i) const;
    void PrintChar(int i, int j) const;
};

#endif // BOARD_H