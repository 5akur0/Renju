
#include <bits/stdc++.h>
using namespace std;

int SIZE = 15;
int board[51][51];

void SetSize();
void InitializeBoard();
char ToAlpha(int i);
void PrintChar(int i, int j);
void PrintBoard();
void SaveGame();
void LoadGame();
void NewGame();
void ExitGame();

int main() {
    int choice;
    do {
        cout << "1. 新开始\n";
        cout << "2. 存盘\n";
        cout << "3. 读盘\n";
        cout << "4. 退出\n";
        cout << "请输入你的选择: ";
        cin >> choice;

        switch (choice) {
            case 1:
                NewGame();
                break;
            case 2:
                SaveGame();
                break;
            case 3:
                LoadGame();
                break;
            case 4:
                ExitGame();
                break;
            default:
                printf("无效的选择，请重新输入。\n");
        }
    } while (choice != 4);
    return 0;
}

void SetSize() {
    do {
        printf("请输入一个介于5到50之间的边长，默认为15（输入0退出）: ");
        if (scanf("%d", &SIZE) != 1) {
            while (getchar() != '\n');
        } else if (SIZE == 0) {
            printf("程序退出。\n");
            exit(0);
        } else if (SIZE < 5 || SIZE > 50) {
            printf("输入无效，请输入一个介于5到50之间的正整数。\n");
        }
    } while (SIZE < 5 || SIZE > 50);
    printf("边长已设置为%d。\n", SIZE);
}

void InitializeBoard() {
    for (int i = 0; i < SIZE + 1; i++) {
        for (int j = 0; j < SIZE + 1; j++) {
            board[i][j] = 0;
        }
    }
}

char ToAlpha(int i) {
    if (i <= 26) {
        return 'a' + i - 1;
    }
    return 'A' + i - 27;
}

void PrintChar(int i, int j) {
    if (board[i][j] == 0) {
        if (i != SIZE) {
            if (j == 1) {
                printf("┌─");
            } else if (j == SIZE) {
                printf("┐");
            } else {
                printf("┬─");
            }
        } else if (i == 1) {
            if (j == 1) {
                printf("└─");
            } else if (j == SIZE) {
                printf("┘");
            } else {
                printf("┴─");
            }
        } else {
            if (j == 1) {
                printf("├─");
            } else if (j == SIZE) {
                printf("┤");
            } else {
                printf("┼─");
            }
        }
    } else if (board[i][j] == 1) {
        printf("● ");
    } else {
        printf("○ ");
    }
}

void PrintBoard() {
    for (int i = SIZE; i >= 1; i--) {
        printf("%2d ", i);
        for (int j = 1; j <= SIZE; j++) {
            PrintChar(i, j);
        }
        printf("\n");
    }
    printf("  ");
    for (int j = 1; j <= SIZE; j++) {
        printf(" %c", ToAlpha(j));
    }
    printf("\n");
}

void SaveGame() {
    FILE *file = fopen("savegame.txt", "w");
    if (file == NULL) {
        printf("无法打开文件进行存盘。\n");
        return;
    }

    fprintf(file, "%d\n", SIZE);
    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            fprintf(file, "%d ", board[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("游戏已存盘。\n");
}

void LoadGame() {
    FILE *file = fopen("savegame.txt", "r");
    if (file == NULL) {
        printf("无法打开文件进行读盘。\n");
        return;
    }

    fscanf(file, "%d", &SIZE);
    if (SIZE < 5 || SIZE > 50) {
        printf("存档文件无效。\n");
        fclose(file);
        return;
    }

    for (int i = 1; i <= SIZE; i++) {
        for (int j = 1; j <= SIZE; j++) {
            fscanf(file, "%d", &board[i][j]);
        }
    }

    fclose(file);
    printf("游戏已读盘。\n");
    PrintBoard();
}

void NewGame() {
    SetSize();
    InitializeBoard();
    PrintBoard();
    // 实现新游戏的代码
}

void ExitGame() {
    printf("程序退出。\n");
    exit(0);
}