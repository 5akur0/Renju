#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"
#include "AI.h"

extern int lastMoveX; // 记录最后一次落子的X坐标
extern int lastMoveY; // 记录最后一次落子的Y坐标

class GameManager
{
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
    int moveCount; // 添加这个成员变量
    void PlayGame();
    bool CheckWin();
    bool IsBoardFull(); // 修改这个函数声明
    bool GetPlayerMove(int &x, int &y);
    void PromptSaveAndQuit();
    bool CheckDirection(int x, int y, int dx, int dy, int player); // 添加这个函数声明
};

#endif // GAMEMANAGER_H