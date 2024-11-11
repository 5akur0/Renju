#include <bits/stdc++.h>
using namespace std;

const int SIZE = 15;
int board[SIZE][SIZE];

// 1代表黑子(X)，2代表白子(O)，0代表无子
void InitializeBoard() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

void PrintBoard() {
// 上边框
    for (int i = 0; i < SIZE - 1; i++) {
        printf("┌───");
    }
    printf("┐\n");

    // 中间部分
    for (int i = 0; i < SIZE - 1; i++) {
        for (int j = 0; j < SIZE - 1; j++) {
            printf("│   ");
        }
        printf("│\n");

        // 画横线
        if (i < SIZE - 2) {
            for (int j = 0; j < SIZE - 1; j++) {
                printf("├───");
            }
            printf("┤\n");
        }
    }

    // 画底边框
    for (int i = 0; i < SIZE - 1; i++) {
        printf("└───");
    }
    printf("┘\n");
}


int main() {
    InitializeBoard();
    PrintBoard();
    return 0;
}
