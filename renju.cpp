#include <bits/stdc++.h>
using namespace std;

const int SIZE = 15;
int board[SIZE][SIZE];

// 1代表黑子(X), 2代表白子(O)
void InitializeBoard() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

void PrintBoard() {
// 上边框
    for (int i = 0; i < SIZE; i++) {
        printf("┌───");
    }
    printf("┐\n");

    // 中间部分
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("│   ");
        }
        printf("│\n");

        // 画横线
        if (i < SIZE - 1) {  // 最后一行不需要绘制分隔线
            for (int j = 0; j < SIZE; j++) {
                printf("├───");
            }
            printf("┤\n");
        }
    }

    // 画底边框
    for (int i = 0; i < SIZE; i++) {
        printf("└───");
    }
    printf("┘\n");
}


int main() {
    InitializeBoard();
    PrintBoard();
    return 0;
}
