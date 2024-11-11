#include <bits/stdc++.h>
using namespace std;

int SIZE = 15;
int board[51][51];
// 一行一列冗余，但方便编程

void SetSize() {
    do {
        printf("请输入一个介于5到50之间的边长，默认为15（输入0退出）: ");
        if (scanf("%d", &SIZE) != 1) {
            while (getchar() != '\n');
        } else if (SIZE == 0) {
            printf("程序退出。\n");
            break;
        } else if (SIZE < 5 || SIZE > 50) {
            printf("输入无效，请输入一个介于5到50之间的正整数。\n");
        }
    } while (SIZE < 5 || SIZE > 50);
    printf("边长已设置为%d。\n", SIZE);
}

// 1代表黑子(●)，2代表白子(◯)，0代表无子
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
        if (i == SIZE) {
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

int main() {
    SetSize();
    InitializeBoard();
    PrintBoard();
    return 0;
}

