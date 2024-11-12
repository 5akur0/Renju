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
    void QuitGame();

private:
    Board board;
    AI ai;
    int moveCount;
    void SetSize();
    void PlayGame();
    bool CheckWin();
    bool IsBoardFull();
    bool GetPlayerMove(int &x, int &y);
    void PromptSaveAndQuit();
};

#endif // GAMEMANAGER_H