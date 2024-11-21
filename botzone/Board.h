#ifndef BOARD_H
#define BOARD_H

class Board {
public:
    int board[16][16];
    void SetCell(int i, int j, int value);
    void SetLastMove(int x, int y);

private:
    int lastMoveX;
    int lastMoveY;
};

#endif // BOARD_H