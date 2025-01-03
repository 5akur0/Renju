#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AI.h"
#include "Board.h"
#include <string>

const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int MARGIN = 40;

class GameManager {
public:
    GameManager();
    void NewGame();
    bool CheckWin();
    bool CheckDirection(int x, int y, int dx, int dy, int player);
    bool IsBoardFull();
    void SaveGame();
    void LoadGame();
    void ClearAllSaves();
    void QuitGame();
    void PromptSaveAndQuit();

private:
    Board board;
    AI ai;
    int moveCount;
    int lastMoveX;
    int lastMoveY;
    std::string saveFolder;
};

#endif // GAMEMANAGER_H