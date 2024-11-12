#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Board.h"

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
    void SetSize();
};

#endif // GAMEMANAGER_H