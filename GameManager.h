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
    void SetSize();
    void PlayGame();
    bool CheckWin();
    bool GetPlayerMove(int &x, int &y);
    void PromptSaveAndQuit(); // 添加这个函数声明
};

#endif // GAMEMANAGER_H