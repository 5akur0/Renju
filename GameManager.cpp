#include "GameManager.h"
#include "AI.h"
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <cctype>

namespace fs = std::__fs::filesystem;
using namespace std;

int lastMoveX = -1;
int lastMoveY = -1;

GameManager::GameManager() : board(), ai(), moveCount(0) {}

void GameManager::NewGame()
{
    board.Initialize();
    moveCount = 0; // 初始化步数
    lastMoveX = -1;
    lastMoveY = -1;
    board.Print();
    PlayGame();
}

void GameManager::PlayGame()
{
    int x, y;
    while (true)
    {
        if (moveCount % 2 == 0)
        { // 玩家回合
            if (!GetPlayerMove(x, y))
            {
                continue;
            }
            if (x == -1 && y == -1)
            {
                PromptSaveAndQuit();
                return;
            }
            board.SetCell(x, y, C_BLACK); // 玩家使用黑子
            lastMoveX = x;
            lastMoveY = y;
        }
        else
        { // AI回合
            ai.MakeMove(board);
        }
        moveCount++; // 更新步数
        board.Print();
        // 检查胜利条件
        if (CheckWin())
        {
            printf((moveCount % 2 == 1) ? "玩家胜利！\n" : "AI胜利！\n");
            break;
        }
        // 检查是否平局
        if (IsBoardFull())
        {
            printf("棋盘已满，平局！\n");
            break;
        }
    }
}

bool GameManager::GetPlayerMove(int &x, int &y) {
    printf("请输入你的落子位置（格式：数字字母，例如6a，输入q退出）：");
    string input;
    cin >> input;
    // 检查输入是否为退出命令
    if (input == "q") {
        x = -1;
        y = -1;
        return true;
    }
    // 检查输入格式是否正确
    if (input.length() < 2 || !isalpha(input.back())) {
        printf("无效的输入格式，请重新输入。\n");
        return false;
    }
    // 解析输入
    size_t pos = 0;
    while (pos < input.length() && isdigit(input[pos])) {
        pos++;
    }
    if (pos == 0 || pos == input.length()) {
        printf("无效的输入格式，请重新输入。\n");
        return false;
    }
    x = stoi(input.substr(0, pos));
    char col = input[pos];
    y = board.ToIndex(col); // 使用 Board::ToIndex 将字母转换为列号
    if (x < 1 || x > 15 || y < 1 || y > 15 || board.GetCell(x, y) != 0) {
        printf("无效的位置，请重新输入。\n");
        return false;
    }
    return true;
}

bool GameManager::IsBoardFull() {
    for (int i = 1; i <= 15; ++i) {
        for (int j = 1; j <= 15; ++j) {
            if (board.GetCell(i, j) == C_NONE) {
                return false;
            }
        }
    }
    return true;
}

void GameManager::SaveGame() {
    char filename[100];
    while (true) {
        printf("请输入要保存的文件名（例如：savegame.txt）：");
        scanf("%s", filename);
        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".txt") == 0) {
            break;
        } else {
            printf("文件名必须以 .txt 结尾。\n");
        }
    }
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法打开文件进行存盘。\n");
        return;
    }
    fprintf(file, "%d\n", moveCount); // 保存当前步数
    fprintf(file, "%d %d\n", lastMoveX, lastMoveY); // 保存最后一次落子的坐标
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            fprintf(file, "%d ", board.GetCell(i, j));
        }
        fprintf(file, "\n");
    }
    fclose(file);
    printf("游戏已保存到文件 %s。\n", filename);
}

void GameManager::LoadGame() {
    char filename[100];
    FILE *file = nullptr;
    while (true) {
        printf("请输入要读取的文件名（例如：savegame.txt），或输入 'q' 退出：");
        scanf("%s", filename);
        if (strcmp(filename, "q") == 0) {
            printf("取消读盘操作。\n");
            return;
        }
        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".txt") == 0) {
            file = fopen(filename, "r");
            if (file != NULL) {
                break;
            } else {
                printf("无法打开文件 %s 进行读盘。\n", filename);
            }
        } else {
            printf("文件名必须以 .txt 结尾。\n");
        }
    }
    fscanf(file, "%d", &moveCount); // 读取当前步数
    fscanf(file, "%d %d", &lastMoveX, &lastMoveY); // 读取最后一次落子的坐标
    for (int i = 1; i <= 15; i++) {
        for (int j = 1; j <= 15; j++) {
            int value;
            fscanf(file, "%d", &value);
            board.SetCell(i, j, value);
        }
    }
    fclose(file);
    printf("游戏已从文件 %s 读入。\n", filename);
    board.Print();
}

void GameManager::ClearAllSaves() {
    for (const auto &entry : fs::directory_iterator(".")) {
        if (entry.path().extension() == ".txt") {
            fs::remove(entry.path());
        }
    }
}

void GameManager::QuitGame() {
    printf("程序退出。\n");
    exit(0);
}

void GameManager::PromptSaveAndQuit() {
    char choice;
    printf("是否保存游戏进度？(y/n)：");
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        SaveGame();
    }
    QuitGame();
}

bool GameManager::CheckWin() {
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

bool GameManager::CheckDirection(int x, int y, int dx, int dy, int player) {
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