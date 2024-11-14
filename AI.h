#ifndef AI_H
#define AI_H

#include "Board.h"
#include "AIAlgorithms.h"

class AI
{
public:
    AI();
    void MakeMove(Board &board);
};

#endif // AI_H