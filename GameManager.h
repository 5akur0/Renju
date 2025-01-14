#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AI.h"
#include "Board.h"
#include <string>

const int BOARD_SIZE = 15;
const int CELL_SIZE = 40;
const int MARGIN = 40;

const int BUTTON_RADIUS = 10;

class GameManager {
public:
    GameManager();
    void NewGame();
    bool CheckWin();
    bool CheckDirection(int x, int y, int dx, int dy, int player);
    bool IsBoardFull();
    void SaveGame(const std::string& filename);
    void LoadGame(const std::string& filename);
    const std::string& GetSaveFolder() const { return saveFolder; }
    void QuitGame();
    void SetLastMove(int x, int y);
    int GetLastMoveX() const { return lastMoveX; }
    int GetLastMoveY() const { return lastMoveY; }
    std::pair<int, int> GetBestMove(int player);
    void PromptSaveAndQuit();
    Board board;

private:
    AI ai;
    int moveCount;
    int lastMoveX;
    int lastMoveY;
    std::string saveFolder;
};

#endif // GAMEMANAGER_H