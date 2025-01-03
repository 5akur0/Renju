#include "GameManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>

// 辅助函数：绘制填充的圆形
void DrawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void DrawBoard(SDL_Renderer* renderer, const std::vector<std::vector<int>>& board, int offsetX, int offsetY)
{
    // 绘制网格
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 竖线
        SDL_RenderDrawLine(renderer, offsetX + MARGIN + i * CELL_SIZE, offsetY + MARGIN, offsetX + MARGIN + i * CELL_SIZE, offsetY + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        // 横线
        SDL_RenderDrawLine(renderer, offsetX + MARGIN, offsetY + MARGIN + i * CELL_SIZE, offsetX + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, offsetY + MARGIN + i * CELL_SIZE);
    }

    // 绘制棋子
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] != 0) {
                if (board[r][c] == 1) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    DrawFilledCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, CELL_SIZE / 2 - 2);
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    DrawFilledCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, CELL_SIZE / 2 - 2);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    DrawFilledCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, CELL_SIZE / 2 - 4);
                }
            }
        }
    }
}

void DrawSlider(SDL_Renderer* renderer, int x, int y, int width, int height, float value)
{
    // 绘制滑动条背景
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_Rect sliderBg = { x, y, width, height };
    SDL_RenderFillRect(renderer, &sliderBg);

    // 绘制滑动条前景
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_Rect sliderFg = { x, y, static_cast<int>(width * value), height };
    SDL_RenderFillRect(renderer, &sliderFg);
}

void RunGameUI()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return;
    }

    int windowSize = BOARD_SIZE * CELL_SIZE + 2 * MARGIN + 50;

    SDL_Window* window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowSize, windowSize, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // 加载背景图片
    SDL_Surface* bgSurface = IMG_Load("flower.png");
    if (!bgSurface) {
        SDL_Log("Unable to load image: %s", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return;
    }
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // 设置初始透明度
    float opacity = 0.8f;
    SDL_SetWindowOpacity(window, opacity);

    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    bool isBlackTurn = true;

    GameManager gameManager;
    gameManager.NewGame();

    bool running = true;
    bool dragging = false;
    int sliderX = MARGIN;
    int sliderY = BOARD_SIZE * CELL_SIZE + 2 * MARGIN + 10;
    int sliderWidth = BOARD_SIZE * CELL_SIZE;
    int sliderHeight = 20;

    // 计算棋盘的水平和垂直偏移量，使其居中
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    int offsetX = (windowWidth - (BOARD_SIZE * CELL_SIZE + MARGIN * 2)) / 2;
    int offsetY = (windowHeight - (BOARD_SIZE * CELL_SIZE + MARGIN * 2)) / 2;

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (x >= sliderX && x <= sliderX + sliderWidth && y >= sliderY && y <= sliderY + sliderHeight) {
                        dragging = true;
                    } else if (y < sliderY) { // 确保点击在棋盘区域
                        int boardX = x - offsetX - MARGIN;
                        int boardY = y - offsetY - MARGIN;

                        if (boardX >= 0 && boardY >= 0) {
                            int col = (boardX + CELL_SIZE / 2) / CELL_SIZE;
                            int row = (boardY + CELL_SIZE / 2) / CELL_SIZE;
                            if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
                                if (board[row][col] == 0) {
                                    board[row][col] = isBlackTurn ? 1 : 2;
                                    isBlackTurn = !isBlackTurn;
                                }
                            }
                        }
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }
            } else if (event.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    int x = event.motion.x - sliderX;
                    opacity = static_cast<float>(x) / sliderWidth;
                    if (opacity < 0.3f)
                        opacity = 0.3f; // 设置透明度下限
                    if (opacity > 1.0f)
                        opacity = 1.0f;
                    SDL_SetWindowOpacity(window, opacity);
                }
            }
        }

        // 绘制背景
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        // 绘制棋盘和滑动条
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 重置颜色和透明度
        DrawBoard(renderer, board, offsetX, offsetY);
        DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, opacity);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}