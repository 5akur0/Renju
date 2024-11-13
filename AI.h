#ifndef AI_H
#define AI_H

#include "Board.h"

class AI {
public:
    AI();
    void MakeMove(Board &board);
    int GetLastMoveX() const;
    int GetLastMoveY() const;

private:
    int lastMoveX;
    int lastMoveY;
};

#endif // AI_H