#include "GameManager.h"
#include <SDL.h>
#include <vector>

static const int BOARD_SIZE = 15;
static const int CELL_SIZE = 40;
static const int MARGIN = 50;

void DrawBoard(SDL_Renderer* renderer, const std::vector<std::vector<int>>& board)
{
    // 绘制网格
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 竖线
        SDL_RenderDrawLine(renderer, MARGIN + i * CELL_SIZE, MARGIN, MARGIN + i * CELL_SIZE, MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        // 横线
        SDL_RenderDrawLine(renderer, MARGIN, MARGIN + i * CELL_SIZE, MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, MARGIN + i * CELL_SIZE);
    }

    // 绘制棋子
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] != 0) {
                if (board[r][c] == 1) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                }
                SDL_Rect rect = { MARGIN + c * CELL_SIZE - CELL_SIZE / 4, MARGIN + r * CELL_SIZE - CELL_SIZE / 4, CELL_SIZE / 2, CELL_SIZE / 2 };
                SDL_RenderFillRect(renderer, &rect);
                if (board[r][c] == 2) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawRect(renderer, &rect);
                }
            }
        }
    }
}

void RunGameUI()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }

    SDL_Window* window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        BOARD_SIZE * CELL_SIZE + 2 * MARGIN, BOARD_SIZE * CELL_SIZE + 2 * MARGIN, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    bool isBlackTurn = true;

    GameManager gameManager;
    gameManager.NewGame();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x - MARGIN;
                    int y = event.button.y - MARGIN;

                    if (x >= 0 && y >= 0) {
                        int col = x / CELL_SIZE;
                        int row = y / CELL_SIZE;
                        if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
                            if (board[row][col] == 0) {
                                board[row][col] = isBlackTurn ? 1 : 2;
                                isBlackTurn = !isBlackTurn;
                            }
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        DrawBoard(renderer, board);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}