#include <bits/stdc++.h>
using namespace std;

const int SIZE = 15;
int board[SIZE + 1][SIZE + 1];
// 一行一列冗余，但方便编程

// 1代表黑子(●)，2代表白子(◯)，0代表无子
void InitializeBoard() {
    for (int i = 0; i < SIZE + 1; i++) {
        for (int j = 0; j < SIZE + 1; j++) {
            board[i][j] = 0;
        }
    }
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
        printf(" %c", 'a' - 1 + j);
    }
    printf("\n");
}

int main() {
    InitializeBoard();
    PrintBoard();
    return 0;
}
