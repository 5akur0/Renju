#include "GameManager.h"
#include "AI.h"
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <cctype>

namespace fs = std::filesystem;
using namespace std;

GameManager::GameManager() : board(15), ai() {}

void GameManager::NewGame() {
    SetSize();
    board.Initialize();
    board.Print();
    PlayGame();
}

void GameManager::PlayGame() {
    int x, y;
    bool playerTurn = true; // true 表示玩家回合，false 表示AI回合
    while (true) {
        if (playerTurn) {
            if (!GetPlayerMove(x, y)) {
                continue;
            }
            if (x == -1 && y == -1) {
                PromptSaveAndQuit();
                return;
            }
            board.SetCell(x, y, 1); // 玩家使用黑子
        } else {
            ai.MakeMove(board);
        }
        board.Print();
        // 检查胜利条件
        if (CheckWin()) {
            printf(playerTurn ? "玩家胜利！\n" : "AI胜利！\n");
            break;
        }
        playerTurn = !playerTurn;
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
    if (input.length() < 2 || !isdigit(input[0]) || !isalpha(input[1])) {
        printf("无效的输入格式，请重新输入。\n");
        return false;
    }

    // 解析输入
    x = input[0] - '0';
    char col = input[1];
    y = board.ToIndex(col); // 使用 Board::ToIndex 将字母转换为列号

    if (x < 1 || x > board.GetSize() || y < 1 || y > board.GetSize() || board.GetCell(x, y) != 0) {
        printf("无效的位置，请重新输入。\n");
        return false;
    }

    return true;
}

bool GameManager::CheckWin() {
    // 实现检查胜利条件的逻辑
    return false;
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

    fprintf(file, "%d\n", board.GetSize());
    for (int i = 1; i <= board.GetSize(); i++) {
        for (int j = 1; j <= board.GetSize(); j++) {
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

    int size;
    fscanf(file, "%d", &size);
    if (size < 5 || size > 50) {
        printf("存档文件无效。\n");
        fclose(file);
        return;
    }

    board.SetSize(size);
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
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
    printf("所有存档文件已被清除。\n");
}

void GameManager::QuitGame() {
    printf("程序已退出。\n");
    exit(0);
}

void GameManager::PromptSaveAndQuit() {
    char choice;
    printf("你想在退出前保存游戏吗？(y/n)：");
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        SaveGame();
    }
    QuitGame();
}

void GameManager::SetSize() {
    int size;
    do {
        printf("请输入一个介于5到50之间的边长，默认为15（输入0退出）: ");
        if (scanf("%d", &size) != 1) {
            while (getchar() != '\n');
        } else if (size == 0) {
            printf("程序退出。\n");
            QuitGame();
        } else if (size < 5 || size > 50) {
            printf("输入无效，请输入一个介于5到50之间的正整数。\n");
        }
    } while (size < 5 || size > 50);
    board.SetSize(size);
    printf("边长已设置为%d。\n", size);
}