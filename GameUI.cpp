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
#include <cmath>

void DrawSmoothGradientCircle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color centerColor, SDL_Color edgeColor, int centerRadius)
{
    int transitionStart = centerRadius; // 纯色区域的结束半径
    int transitionEnd = radius; // 渐变区域的结束半径

    for (int r = radius; r > 0; --r) {
        float t;
        if (r <= transitionStart) {
            // 纯色区域
            t = 0.0f;
        } else if (r > transitionStart && r <= transitionEnd) {
            // 渐变区域：使用三次函数平滑过渡
            float u = static_cast<float>(r - transitionStart) / (transitionEnd - transitionStart);
            t = 3 * u * u - 2 * u * u * u; // 三次插值公式
        } else {
            // 完全渐变到边缘颜色
            t = 1.0f;
        }

        // 根据 t 计算当前颜色
        Uint8 rColor = static_cast<Uint8>(centerColor.r * (1 - t) + edgeColor.r * t);
        Uint8 gColor = static_cast<Uint8>(centerColor.g * (1 - t) + edgeColor.g * t);
        Uint8 bColor = static_cast<Uint8>(centerColor.b * (1 - t) + edgeColor.b * t);
        Uint8 aColor = static_cast<Uint8>(centerColor.a * (1 - t) + edgeColor.a * t);

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
                int pieceRadius = CELL_SIZE / 2 - 2;
                int centerRadius = pieceRadius * 0.3;

                if (board[r][c] == 1) {
                    // 黑棋：中心纯黑，边缘渐变到深灰
                    SDL_Color centerColor = { 0, 0, 0, 255 };
                    SDL_Color edgeColor = { 25, 25, 25, 255 };
                    DrawSmoothGradientCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, pieceRadius, centerColor, edgeColor, centerRadius);
                } else {
                    // 白棋：中心纯白，边缘渐变到浅灰
                    SDL_Color centerColor = { 255, 255, 255, 255 };
                    SDL_Color edgeColor = { 225, 225, 225, 255 };
                    DrawSmoothGradientCircle(renderer, offsetX + MARGIN + c * CELL_SIZE, offsetY + MARGIN + r * CELL_SIZE, pieceRadius, centerColor, edgeColor, centerRadius);
                }
            }
        }
    }
}

void DrawGradientRoundedRect(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color startColor, SDL_Color endColor, int radius)
{
    // 绘制左右渐变的圆角矩形
    for (int i = 0; i < rect.w; ++i) {
        float t = static_cast<float>(i) / rect.w;
        Uint8 r = static_cast<Uint8>(startColor.r * (1 - t) + endColor.r * t);
        Uint8 g = static_cast<Uint8>(startColor.g * (1 - t) + endColor.g * t);
        Uint8 b = static_cast<Uint8>(startColor.b * (1 - t) + endColor.b * t);
        Uint8 a = static_cast<Uint8>(startColor.a * (1 - t) + endColor.a * t);

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawLine(renderer, rect.x + i, rect.y + radius, rect.x + i, rect.y + rect.h - radius);

        // 绘制圆角
        for (int w = 0; w < radius; ++w) {
            for (int h = 0; h < radius; ++h) {
                if ((w - radius) * (w - radius) + (h - radius) * (h - radius) <= radius * radius) {
                    SDL_RenderDrawPoint(renderer, rect.x + i, rect.y + h);
                    SDL_RenderDrawPoint(renderer, rect.x + i, rect.y + rect.h - h - 1);
                }
            }
        }
    }
}

void DrawSlider(SDL_Renderer* renderer, int x, int y, int width, int height, float value)
{
    // 滑动条背景：从左到右渐变，表示透明度范围
    SDL_Color startColor = { 0, 0, 0, static_cast<Uint8>(MIN_OPACITY * 255) }; // 左端颜色
    SDL_Color endColor = { 0, 0, 0, 255 }; // 右端颜色
    SDL_Rect sliderBg = { x, y, width, height };
    DrawGradientRoundedRect(renderer, sliderBg, startColor, endColor, height / 2);

    // 绘制滑块
    int sliderPos = static_cast<int>(x + width * value);
    SDL_Color knobColor = { 255, 255, 255, 255 }; // 滑块颜色
    DrawFilledCircle(renderer, sliderPos, y + height / 2, height / 2, knobColor);
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

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, windowSize, windowSize);

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
    int sliderHeight = 10;

    int drawableWidth, drawableHeight;
    SDL_GetRendererOutputSize(renderer, &drawableWidth, &drawableHeight);

    int offsetX = drawableWidth / 2 - (BOARD_SIZE * CELL_SIZE + MARGIN * 2);
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
                    } else if (y < sliderY) {
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

                    opacity = MIN_OPACITY + (static_cast<float>(x) / sliderWidth) * (1.0f - MIN_OPACITY);
                    SDL_SetWindowOpacity(window, opacity);
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
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