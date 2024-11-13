#include "GameManager.h"
#include "AI.h"
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <cctype>

namespace fs = std::__fs::filesystem;
using namespace std;

GameManager::GameManager() : board(), ai(), moveCount(0), lastMoveX(-1), lastMoveY(-1) {}

void GameManager::NewGame()
{
    SetSize();
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
            board.SetCell(x, y, 1); // 玩家使用黑子
        }
        else
        { // AI回合
            ai.MakeMove(board);
            x = ai.GetLastMoveX(); // 获取AI最后一次落子的X坐标
            y = ai.GetLastMoveY(); // 获取AI最后一次落子的Y坐标
        }
        moveCount++; // 更新步数
        lastMoveX = x;
        lastMoveY = y;
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

bool GameManager::GetPlayerMove(int &x, int &y)
{
    printf("请输入你的落子位置（格式：数字字母，例如6a，输入q退出）：");
    string input;
    cin >> input;

    // 检查输入是否为退出命令
    if (input == "q")
    {
        x = -1;
        y = -1;
        return true;
    }

    // 检查输入格式是否正确
    if (input.length() < 2 || !isdigit(input[0]) || !isalpha(input[1]))
    {
        printf("无效的输入格式，请重新输入。\n");
        return false;
    }

    // 解析输入
    x = input[0] - '0';
    char col = input[1];
    y = board.ToIndex(col); // 使用 Board::ToIndex 将字母转换为列号

    if (x < 1 || x > board.GetSize() || y < 1 || y > board.GetSize() || board.GetCell(x, y) != 0)
    {
        printf("无效的位置，请重新输入。\n");
        return false;
    }

    return true;
}

bool GameManager::CheckWin()
{
    int player = board.GetCell(lastMoveX, lastMoveY);
    if (player != 0)
    {
        if (CheckDirection(lastMoveX, lastMoveY, 1, 0, player) || // 水平方向
            CheckDirection(lastMoveX, lastMoveY, 0, 1, player) || // 垂直方向
            CheckDirection(lastMoveX, lastMoveY, 1, 1, player) || // 斜向右下
            CheckDirection(lastMoveX, lastMoveY, 1, -1, player))
        { // 斜向右上
            return true;
        }
    }
    return false;
}

bool GameManager::CheckDirection(int x, int y, int dx, int dy, int player)
{
    int count = 0;
    for (int i = -4; i <= 0; ++i)
    {
        count = 0;
        for (int j = 0; j < 5; ++j)
        {
            int nx = x + (i + j) * dx;
            int ny = y + (i + j) * dy;
            if (nx >= 1 && nx <= board.GetSize() && ny >= 1 && ny <= board.GetSize() && board.GetCell(nx, ny) == player)
            {
                count++;
            }
            else
            {
                break;
            }
        }
        if (count == 5)
        {
            return true;
        }
    }
    return false;
}

bool GameManager::IsBoardFull()
{
    return moveCount >= board.GetSize() * board.GetSize();
}

void GameManager::SaveGame()
{
    char filename[100];
    while (true)
    {
        printf("请输入要保存的文件名（例如：savegame.txt）：");
        scanf("%s", filename);

        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".txt") == 0)
        {
            break;
        }
        else
        {
            printf("文件名必须以 .txt 结尾。\n");
        }
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("无法打开文件进行存盘。\n");
        return;
    }

    fprintf(file, "%d\n", board.GetSize());
    fprintf(file, "%d\n", moveCount);               // 保存当前步数
    fprintf(file, "%d %d\n", lastMoveX, lastMoveY); // 保存最后一次落子的坐标
    for (int i = 1; i <= board.GetSize(); i++)
    {
        for (int j = 1; j <= board.GetSize(); j++)
        {
            fprintf(file, "%d ", board.GetCell(i, j));
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("游戏已保存到文件 %s。\n", filename);
}

void GameManager::LoadGame()
{
    char filename[100];
    FILE *file = nullptr;

    while (true)
    {
        printf("请输入要读取的文件名（例如：savegame.txt），或输入 'q' 退出：");
        scanf("%s", filename);

        if (strcmp(filename, "q") == 0)
        {
            printf("取消读盘操作。\n");
            return;
        }

        if (strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".txt") == 0)
        {
            file = fopen(filename, "r");
            if (file != NULL)
            {
                break;
            }
            else
            {
                printf("无法打开文件 %s 进行读盘。\n", filename);
            }
        }
        else
        {
            printf("文件名必须以 .txt 结尾。\n");
        }
    }

    int size;
    fscanf(file, "%d", &size);
    if (size < 5 || size > 50)
    {
        printf("存档文件无效。\n");
        fclose(file);
        return;
    }

    board.SetSize(size);
    fscanf(file, "%d", &moveCount);                // 读取当前步数
    fscanf(file, "%d %d", &lastMoveX, &lastMoveY); // 读取最后一次落子的坐标
    for (int i = 1; i <= size; i++)
    {
        for (int j = 1; j <= size; j++)
        {
            int value;
            fscanf(file, "%d", &value);
            board.SetCell(i, j, value);
        }
    }

    fclose(file);
    printf("游戏已从文件 %s 读入。\n", filename);
    board.Print();
}

void GameManager::ClearAllSaves()
{
    for (const auto &entry : fs::directory_iterator("."))
    {
        if (entry.path().extension() == ".txt")
        {
            fs::remove(entry.path());
        }
    }
    printf("所有存档文件已被清除。\n");
}

void GameManager::QuitGame()
{
    printf("程序已退出。\n");
    exit(0);
}

void GameManager::PromptSaveAndQuit()
{
    char choice;
    printf("你想在退出前保存游戏吗？(y/n)：");
    cin >> choice;
    if (choice == 'y' || choice == 'Y')
    {
        SaveGame();
    }
    QuitGame();
}

void GameManager::SetSize()
{
    int size;
    do
    {
        printf("请输入一个介于5到50之间的边长，默认为15（输入0退出）: ");
        if (scanf("%d", &size) != 1)
        {
            while (getchar() != '\n')
                ;
        }
        else if (size == 0)
        {
            printf("程序退出。\n");
            QuitGame();
        }
        else if (size < 5 || size > 50)
        {
            printf("输入无效，请输入一个介于5到50之间的正整数。\n");
        }
    } while (size < 5 || size > 50);
    board.SetSize(size);
    printf("边长已设置为%d。\n", size);
}