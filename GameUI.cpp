#include "GameManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>

// 定义最小透明度
const float MIN_OPACITY = 0.1f;

void DrawFilledCircle(SDL_Renderer* renderer, int x, int y, int radius)
{
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            float distance = sqrtf(w * w + h * h);
            if (distance <= radius) {
                Uint8 alpha = 255;
                // 对圆周边缘应用抗锯齿处理
                if (distance > radius - 1.0f) {
                    alpha = static_cast<Uint8>((radius - distance) * 255);
                }

                // 获取当前绘制颜色
                Uint8 r, g, b, a;
                SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);

                // 设置带有调整后透明度的颜色
                SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}

void DrawGradientCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color innerColor, SDL_Color outerColor)
{
    // 从内到外绘制渐变圆
    for (int r = radius; r > 0; --r) {
        // 计算当前半径对应的颜色
        float t = static_cast<float>(r) / radius; // 当前半径的比例
        Uint8 rColor = static_cast<Uint8>(innerColor.r * t + outerColor.r * (1 - t));
        Uint8 gColor = static_cast<Uint8>(innerColor.g * t + outerColor.g * (1 - t));
        Uint8 bColor = static_cast<Uint8>(innerColor.b * t + outerColor.b * (1 - t));
        Uint8 aColor = static_cast<Uint8>(innerColor.a * t + outerColor.a * (1 - t));

        SDL_SetRenderDrawColor(renderer, rColor, gColor, bColor, aColor);
        DrawFilledCircle(renderer, x, y, r);
    }
}

void DrawBoard(SDL_Renderer* renderer, const std::vector<std::vector<int>>& board, int offsetX, int offsetY)
{
    // 绘制网格
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 竖线
        SDL_RenderDrawLine(renderer, offsetX + MARGIN + i * CELL_SIZE, offsetY + MARGIN,
            offsetX + MARGIN + i * CELL_SIZE, offsetY + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        // 横线
        SDL_RenderDrawLine(renderer, offsetX + MARGIN, offsetY + MARGIN + i * CELL_SIZE,
            offsetX + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, offsetY + MARGIN + i * CELL_SIZE);
    }

    // 绘制棋子
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (board[r][c] != 0) {
                if (board[r][c] == 1) {
                    // 黑棋：从深灰到黑色的渐变
                    SDL_Color innerColor = { 50, 50, 50, 255 };
                    SDL_Color outerColor = { 0, 0, 0, 255 };
                    DrawGradientCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, CELL_SIZE / 2 - 2, innerColor, outerColor);
                } else {
                    // 白棋：从浅灰到白色的渐变
                    SDL_Color innerColor = { 200, 200, 200, 255 };
                    SDL_Color outerColor = { 255, 255, 255, 255 };
                    DrawGradientCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, CELL_SIZE / 2 - 2, innerColor, outerColor);
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

    // 设定固定的窗口尺寸
    int windowSize = BOARD_SIZE * CELL_SIZE + 2 * MARGIN + 50; // 15*40 + 2*40 + 50 = 600 + 80 + 50 = 730

    SDL_Window* window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowSize, windowSize, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
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

    // 启用渲染器的混合模式，以支持透明度
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 设置渲染器的逻辑尺寸，保持渲染一致性
    SDL_RenderSetLogicalSize(renderer, windowSize, windowSize);

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
    int sliderX = 325;
    int sliderY = 680;
    int sliderWidth = 350;
    int sliderHeight = 20;

    // 获取渲染器输出尺寸（考虑高 DPI）
    int drawableWidth, drawableHeight;
    SDL_GetRendererOutputSize(renderer, &drawableWidth, &drawableHeight);

    // 计算棋盘的水平和垂直偏移量，使其居中
    int offsetX = drawableWidth / 2- (BOARD_SIZE * CELL_SIZE + MARGIN * 2);
    int offsetY = drawableHeight / 2 - (BOARD_SIZE * CELL_SIZE + MARGIN * 2);

    SDL_Log("Drawable Size: %d x %d", drawableWidth, drawableHeight);
    SDL_Log("OffsetX: %d, OffsetY: %d", offsetX, offsetY);

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
                    if (x < 0)
                        x = 0;
                    if (x > sliderWidth)
                        x = sliderWidth;

                    // 将滑动位置映射到 opacity 范围 [MIN_OPACITY, 1.0f]
                    opacity = MIN_OPACITY + (static_cast<float>(x) / sliderWidth) * (1.0f - MIN_OPACITY);
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
        DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}