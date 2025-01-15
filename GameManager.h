#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AI.h"
#include "Board.h"
#include <stack>
#include <string>
#include <utility>

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
    void UndoMove();
    void SetLastMove(int x, int y);
    int GetLastMoveX() const { return lastMoveX; }
    int GetLastMoveY() const { return lastMoveY; }
    std::pair<int, int> GetBestMove(int player);
    Board board;
    std::stack<std::pair<std::pair<int, int>, int>> moveHistory;

private:
    AI ai;
    int moveCount;
    int lastMoveX;
    int lastMoveY;
    std::string saveFolder;
};

#endif // GAMEMANAGER_H