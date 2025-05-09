#include <iostream>

#include "AI.h"
#include "Board.h"
#include "GameManager.h"

using namespace std;

void ShowMenu(GameManager& gameManager)
{
    string input;
    int choice;
    while (true) {
        cout << "1. 新开始\n";
        cout << "2. 存盘\n";
        cout << "3. 读盘\n";
        cout << "4. 清除所有存档\n";
        cout << "q. 退出\n";
        cout << "请输入你的选择: ";
        cin >> input;

        if (input == "q") {
            gameManager.QuitGame();
        }

        try {
            choice = stoi(input);
        } catch (invalid_argument&) {
            cout << "无效的选择，请重新输入。\n";
            continue;
        }

        switch (choice) {
        case 1:
            gameManager.NewGame();
            break;
        case 2:
            gameManager.SaveGame();
            break;
        case 3:
            gameManager.LoadGame();
            break;
        case 4:
            gameManager.ClearAllSaves();
            break;
        default:
            cout << "无效的选择，请重新输入。\n";
        }
    }
}

int main()
{
    init_tuple6type();
    GameManager gameManager;
    ShowMenu(gameManager);
    return 0;
}