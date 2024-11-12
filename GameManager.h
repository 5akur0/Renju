#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"
#include "AI.h"

class GameManager {
public:
    GameManager();
    void NewGame();
    void SaveGame();
    void LoadGame();
    void ClearAllSaves();
    void ExitGame();

private:
    Board board;
    AI ai;
    void SetSize();
    void PlayGame();
    bool CheckWin();
};

#endif // GAMEMANAGER_H