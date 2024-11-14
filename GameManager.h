#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AI.h"
#include "Board.h"

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
    int lastMoveX;
    int lastMoveY;
    void PlayGame();
    bool CheckWin();
    bool IsBoardFull();
    bool GetPlayerMove(int& x, int& y);
    void PromptSaveAndQuit();
    bool CheckDirection(int x, int y, int dx, int dy,
        int player); // 添加这个函数声明
};

#endif // GAMEMANAGER_H