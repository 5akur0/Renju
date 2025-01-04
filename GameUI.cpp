#include "GameManager.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include <vector>
#include <iostream>

// 定义最小透明度
const float MIN_OPACITY = 0.3f;

GameManager gameManager;

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
void DrawBoard(SDL_Renderer* renderer, int offsetX, int offsetY)
{
    const int lineWidth = 2; // 设置线宽

    // 设置颜色为黑色
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 250);

    // 绘制竖线
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int w = 0; w < lineWidth; ++w) {
            SDL_RenderDrawLine(renderer, offsetX + MARGIN + i * CELL_SIZE + w, offsetY + MARGIN,
                offsetX + MARGIN + i * CELL_SIZE + w, offsetY + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        }
    }

    // 绘制横线
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int w = 0; w < lineWidth; ++w) {
            SDL_RenderDrawLine(renderer, offsetX + MARGIN, offsetY + MARGIN + i * CELL_SIZE + w,
                offsetX + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, offsetY + MARGIN + i * CELL_SIZE + w);
        }
    }

    // 绘制棋子
    for (int r = 1; r <= BOARD_SIZE; ++r) {
        for (int c = 1; c <= BOARD_SIZE; ++c) {
            int cell = gameManager.board.GetCell(r, c);
            if (cell != 0) {
                int pieceRadius = CELL_SIZE / 2 - 2;
                int centerRadius = pieceRadius * 0.3;

                // 计算棋子的实际绘制位置
                int pieceX = offsetX + MARGIN + (c - 1) * CELL_SIZE;
                int pieceY = offsetY + MARGIN + (r - 1) * CELL_SIZE;

                if (cell == 1) {
                    SDL_Color centerColor = { 0, 0, 0, 255 };
                    SDL_Color edgeColor = { 25, 25, 25, 255 };
                    DrawSmoothGradientCircle(renderer, pieceX, pieceY,
                        pieceRadius, centerColor, edgeColor, centerRadius);
                } else {
                    SDL_Color centerColor = { 255, 255, 255, 255 };
                    SDL_Color edgeColor = { 225, 225, 225, 255 };
                    DrawSmoothGradientCircle(renderer, pieceX, pieceY,
                        pieceRadius, centerColor, edgeColor, centerRadius);
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

void DrawGradientRect(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color startColor, SDL_Color endColor)
{
    for (int i = 0; i < rect.w; ++i) {
        float t = static_cast<float>(i) / rect.w;
        Uint8 r = static_cast<Uint8>(startColor.r * (1 - t) + endColor.r * t);
        Uint8 g = static_cast<Uint8>(startColor.g * (1 - t) + endColor.g * t);
        Uint8 b = static_cast<Uint8>(startColor.b * (1 - t) + endColor.b * t);
        Uint8 a = static_cast<Uint8>(startColor.a * (1 - t) + endColor.a * t);

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawLine(renderer, rect.x + i, rect.y, rect.x + i, rect.y + rect.h);
    }
}

void DrawSlider(SDL_Renderer* renderer, int x, int y, int width, int height, float value)
{
    // 滑动条背景：从左到右透明度渐变
    SDL_Color startColor = { 0, 0, 0, 10 }; // 左端透明度为10
    SDL_Color endColor = { 0, 0, 0, 160 }; // 右端透明度为160
    SDL_Rect sliderBg = { x, y, width, height };
    DrawGradientRoundedRect(renderer, sliderBg, startColor, endColor, height / 2);

    // 绘制滑块（带渐变的矩形）
    int sliderPos = static_cast<int>(x + width * value);
    int knobWidth = 5; // 滑块的宽度
    SDL_Rect knobRect = { sliderPos - knobWidth / 2, y, knobWidth, height };

    // 滑块的渐变色
    SDL_Color knobStartColor = { 150, 150, 150, 255 }; // 滑块左侧颜色
    SDL_Color knobEndColor = { 50, 50, 50, 255 }; // 滑块右侧颜色
    DrawGradientRect(renderer, knobRect, knobStartColor, knobEndColor);
}


// 主游戏循环
void RunGameUI()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }

    // 创建一个临时窗口来显示对话框
    SDL_Window* window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200, SDL_WINDOW_SHOWN);

    // 选择先后手对话框
    SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "执黑先手" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "执白后手" },
    };
    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        window,
        "选择先后手",
        "请选择执黑先手还是执白后手？",
        SDL_arraysize(buttons),
        buttons,
        NULL
    };

    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);
    bool playerIsBlack = (buttonid == 0);
    bool isBlackTurn = true; // 黑方先手

    // 销毁临时窗口，创建游戏主窗口
    SDL_DestroyWindow(window);

    int windowSize = BOARD_SIZE * CELL_SIZE + 2 * MARGIN + 50;
    window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowSize, windowSize, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);


    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
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

    SDL_Surface* bgSurface = IMG_Load("pku.png");
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


    GameManager gameManager;
    gameManager.NewGame();

    bool running = true;
    bool dragging = false;
    int sliderX = 325;
    int sliderY = 700;
    int sliderWidth = 350;
    int sliderHeight = 10;

    int drawableWidth, drawableHeight;
    SDL_GetRendererOutputSize(renderer, &drawableWidth, &drawableHeight);

    int offsetX = drawableWidth / 2 - (BOARD_SIZE * CELL_SIZE + MARGIN * 2);
    int offsetY = drawableHeight / 2 - (BOARD_SIZE * CELL_SIZE + MARGIN * 2);

    SDL_Log("Drawable Size: %d x %d", drawableWidth, drawableHeight);
    SDL_Log("OffsetX: %d, OffsetY: %d", offsetX, offsetY);

    // 定义退出按钮的位置
    int buttonX = BUTTON_RADIUS + 3;
    int buttonY = BUTTON_RADIUS + 3;


    gameManager.NewGame();

    while (running) {
        if ((!playerIsBlack && isBlackTurn) || (playerIsBlack && !isBlackTurn)) {
            auto [aiRow, aiCol] = gameManager.GetBestMove(isBlackTurn ? 1 : 2);
            if (gameManager.board.GetCell(aiRow, aiCol) == 0) {
                gameManager.board.SetCell(aiRow, aiCol, isBlackTurn ? 1 : 2);
                isBlackTurn = !isBlackTurn;

                if (gameManager.CheckWin()) {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                        "游戏结束",
                        !isBlackTurn ? "白方胜利!" : "黑方胜利!",
                        window);
                    running = false;
                    continue;
                }
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // 只在玩家回合才处理鼠标点击
                if ((playerIsBlack && isBlackTurn) || (!playerIsBlack && !isBlackTurn)) {
                    int x = event.button.x - offsetX - MARGIN;
                    int y = event.button.y - offsetY - MARGIN;

                    if (x >= 0 && y >= 0) {
                        int col = (x + CELL_SIZE / 2) / CELL_SIZE;
                        int row = (y + CELL_SIZE / 2) / CELL_SIZE;
                        if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
                            if (gameManager.board.GetCell(row + 1, col + 1) == 0) {
                                // 玩家落子
                                gameManager.board.SetCell(row + 1, col + 1, isBlackTurn ? 1 : 2);
                                isBlackTurn = !isBlackTurn;

                                // 检查胜负
                                if (gameManager.CheckWin()) {
                                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                        "游戏结束",
                                        isBlackTurn ? "白方胜利!" : "黑方胜利!",
                                        window);
                                    running = false;
                                }
                            }
                        }
                    }
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawBoard(renderer, offsetX, offsetY);
        DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));

        // 绘制圆形退出按钮
        SDL_Color centerColor = { 250, 50, 50, 255 }; // 中心颜色
        SDL_Color edgeColor = { 200, 0, 0, 255 }; // 边缘颜色
        DrawSmoothGradientCircle(renderer, buttonX, buttonY, BUTTON_RADIUS, centerColor, edgeColor, BUTTON_RADIUS * 0.3);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}