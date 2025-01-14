#include "GameManager.h"

#include <SDL.h>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <iostream>

#include "AI.h"

namespace fs = std::__fs::filesystem;
using namespace std;

GameManager::GameManager()
    : board()
    , ai()
    , moveCount(0)
    , saveFolder("存档")
{
    fs::create_directory(saveFolder);
}

void GameManager::NewGame()
{
    board.Initialize();
    moveCount = 0;
    lastMoveX = -1;
    lastMoveY = -1;
}

bool GameManager::CheckWin()
{
    int player = board.GetCell(lastMoveX, lastMoveY);
    if (player != 0) {
        if (CheckDirection(lastMoveX, lastMoveY, 1, 0, player) || // 水平方向
            CheckDirection(lastMoveX, lastMoveY, 0, 1, player) || // 垂直方向
            CheckDirection(lastMoveX, lastMoveY, 1, 1, player) || // 斜向右下
            CheckDirection(lastMoveX, lastMoveY, 1, -1, player)) { // 斜向右上
            return true;
        }
    }
    return false;
}

bool GameManager::CheckDirection(int x, int y, int dx, int dy, int player)
{
    int count = 1;
    for (int i = 1; i < 5; ++i) {
        int nx = x + i * dx;
        int ny = y + i * dy;
        if (nx < 1 || nx > 15 || ny < 1 || ny > 15 || board.GetCell(nx, ny) != player) {
            break;
        }
        count++;
    }
    for (int i = 1; i < 5; ++i) {
        int nx = x - i * dx;
        int ny = y - i * dy;
        if (nx < 1 || nx > 15 || ny < 1 || ny > 15 || board.GetCell(nx, ny) != player) {
            break;
        }
        count++;
    }
    return count >= 5;
}

bool GameManager::IsBoardFull()
{
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            if (board.GetCell(i, j) == C_NONE) {
                return false;
            }
        }
    }
    return true;
}

void GameManager::SaveGame()
{
    // 保存游戏的实现
}

void GameManager::LoadGame()
{
    // 读取游戏的实现
}

void GameManager::ClearAllSaves()
{
    // 清除所有存档的实现
}

void GameManager::QuitGame()
{
    printf("程序退出。\n");
    exit(0);
}

void GameManager::SetLastMove(int x, int y)
{
    lastMoveX = x;
    lastMoveY = y;
}

std::pair<int, int> GameManager::GetBestMove(int player)
{
    ai.MakeMove(board, player);
    return { ai.GetLastMoveX(), ai.GetLastMoveY() };
}

void GameManager::PromptSaveAndQuit()
{
    // 提示保存并退出的实现
}