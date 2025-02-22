#include "GameManager.h"
#include "GameUI.h"
#include "ForbiddenMove.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using std::string;

int mouseX, mouseY;
TTF_Font *buttonFont = nullptr;
TTF_Font *moveFont = nullptr;

// 定义最小透明度
const float MIN_OPACITY = 0.3f;

GameManager gameManager;

void DrawFilledCircle(SDL_Renderer *renderer, int x, int y, int radius) {
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

void DrawSmoothGradientCircle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color centerColor, SDL_Color edgeColor, int centerRadius) {
    int transitionStart = centerRadius; // 纯色区域的结束半径
    int transitionEnd = radius;         // 渐变区域的结束半径

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
void DrawBoard(SDL_Renderer *renderer, int offsetX, int offsetY, int board[16][16], std::pair<int, int> lastMove) {
    // 设置颜色为黑色
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 250);

    // 绘制竖线
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int w = 0; w < LINE_WIDTH; ++w) {
            SDL_RenderDrawLine(renderer, offsetX + MARGIN + i * CELL_SIZE + w, offsetY + MARGIN,
                offsetX + MARGIN + i * CELL_SIZE + w, offsetY + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE);
        }
    }

    // 绘制横线
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int w = 0; w < LINE_WIDTH; ++w) {
            SDL_RenderDrawLine(renderer, offsetX + MARGIN, offsetY + MARGIN + i * CELL_SIZE + w,
                offsetX + MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, offsetY + MARGIN + i * CELL_SIZE + w);
        }
    }

    // 绘制棋子
    for (int r = 1; r <= BOARD_SIZE; ++r) {
        for (int c = 1; c <= BOARD_SIZE; ++c) {
            int cell = board[r][c];
            if (cell != 0) {
                int pieceRadius = CELL_SIZE / 2 - 2;
                int centerRadius = pieceRadius * 0.3;

                // 计算棋子的实际绘制位置
                int pieceX = offsetX + MARGIN + (c - 1) * CELL_SIZE;
                int pieceY = offsetY + MARGIN + (r - 1) * CELL_SIZE;

                if (cell == 1) {
                    SDL_Color centerColor = {0, 0, 0, 255};
                    SDL_Color edgeColor = {25, 25, 25, 255};
                    if (r == lastMove.first && c == lastMove.second) {
                        centerColor = {15, 45, 15, 255};
                        edgeColor = {30, 60, 30, 255};
                    }
                    DrawSmoothGradientCircle(renderer, pieceX, pieceY,
                        pieceRadius, centerColor, edgeColor, centerRadius);
                } else {
                    SDL_Color centerColor = {250, 250, 250, 250};
                    SDL_Color edgeColor = {225, 225, 225, 255};
                    if (r == lastMove.first && c == lastMove.second) {
                        centerColor = {220, 200, 220, 255};
                        edgeColor = {200, 180, 200, 255};
                    }
                    DrawSmoothGradientCircle(renderer, pieceX, pieceY,
                        pieceRadius, centerColor, edgeColor, centerRadius);
                }
            }
        }
    }
}

void DrawSlider(SDL_Renderer *renderer, int x, int y, int width, int height, float value) {
    // 绘制滑动条背景
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 214, 214, 214, 255); // 背景
    SDL_RenderFillRect(renderer, &bgRect);

    // 绘制滑动条前景
    SDL_Rect fgRect = {x, y, static_cast<int>(width * value), height};
    SDL_SetRenderDrawColor(renderer, 65, 65, 65, 255); // 前景
    SDL_RenderFillRect(renderer, &fgRect);

    // 绘制滑块
    int sliderX = x + static_cast<int>(width * value) - height / 2;
    SDL_Rect sliderRect = {sliderX, y - height / 2, height, height * 2};
    SDL_SetRenderDrawColor(renderer, 193, 203, 215, 255); // 滑块
    SDL_RenderFillRect(renderer, &sliderRect);
}

void DrawCurrentPlayer(SDL_Renderer *renderer, const char *text, int windowSize, SDL_Color textColor) {
    // 检查字体是否已成功加载
    if (moveFont == nullptr) {
        SDL_Log("Move font not loaded");
        return;
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended(moveFont, text, textColor);
    if (textSurface == nullptr) {
        SDL_Log("Unable to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        SDL_Log("Unable to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);

    // 计算文本位置，使其居中显示
    SDL_Rect textRect = {(windowSize - textWidth) / 2, MARGIN, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

void DrawButton(SDL_Renderer *renderer, int x, int y, bool isHovered, int color, TTF_Font *font) {
    const char *text = "";
    SDL_Color centerColor, edgeColor;
    if (color == 0) // red
    {
        centerColor = isHovered ? SDL_Color{215, 55, 47, 255} : SDL_Color{255, 95, 87, 255};
        edgeColor = {235, 75, 67, 255};
        text = "Q";
    } else if (color == 1) // yellow
    {
        centerColor = isHovered ? SDL_Color{215, 148, 23, 255} : SDL_Color{255, 188, 46, 255};
        edgeColor = {235, 168, 26, 255};
        text = "S";
    } else if (color == 2) // green
    {
        centerColor = isHovered ? SDL_Color{20, 160, 32, 255} : SDL_Color{39, 200, 64, 255};
        edgeColor = {30, 180, 42, 255};
        text = "L";
    } else if (color == 3) // blue
    {
        centerColor = isHovered ? SDL_Color{20, 120, 160, 255} : SDL_Color{39, 160, 200, 255};
        edgeColor = {30, 140, 180, 255};
        text = "U";
    }
    DrawSmoothGradientCircle(renderer, x, y, BUTTON_RADIUS,
        centerColor, edgeColor, BUTTON_RADIUS * 0.7);

    // 渲染文本
    SDL_Color textColor = {255, 255, 255, 255};

    // 检查字体是否已成功加载
    if (font == nullptr) {
        SDL_Log("Font not loaded");
        return;
    }

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, textColor);
    if (textSurface == nullptr) {
        SDL_Log("Unable to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        SDL_Log("Unable to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {x - textWidth / 2, y - textHeight / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

// 在 RunGameUI 函数中添加按钮位置定义
int exitButtonX = BUTTON_RADIUS + GAP + BUTTON_GAP * 0;
int exitButtonY = BUTTON_RADIUS + GAP;
int saveButtonX = BUTTON_RADIUS + GAP + BUTTON_GAP * 1;
int saveButtonY = BUTTON_RADIUS + GAP;
int loadButtonX = BUTTON_RADIUS + GAP + BUTTON_GAP * 2;
int loadButtonY = BUTTON_RADIUS + GAP;
int undoButtonX = BUTTON_RADIUS + GAP + BUTTON_GAP * 3;
int undoButtonY = BUTTON_RADIUS + GAP;

// 主游戏循环
void RunGameUI() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
        SDL_Quit();
        return;
    }

    if (TTF_Init() == -1) {
        SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // 加载 button_font.ttf 字体
    buttonFont = TTF_OpenFont("assets/button_font.ttf", 18);
    if (!buttonFont) {
        SDL_Log("Unable to load button font: %s", TTF_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // 加载 move_font.ttf 字体
    moveFont = TTF_OpenFont("assets/move_font.ttf", 30);
    if (!moveFont) {
        SDL_Log("Unable to load move font: %s", TTF_GetError());
        TTF_CloseFont(buttonFont);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    // 创建一个临时窗口来显示对话框
    SDL_Window *window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        400, 200, SDL_WINDOW_SHOWN);

    // 选择先后手对话框
    SDL_MessageBoxButtonData buttons[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "执黑先手"},
        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, "执白后手"},
    };
    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        window,
        "选择先后手",
        "请选择执黑先手还是执白后手？",
        SDL_arraysize(buttons),
        buttons,
        NULL};

    int buttonid;
    SDL_ShowMessageBox(&messageboxdata, &buttonid);
    bool playerIsBlack = (buttonid == 0);
    bool isBlackTurn = true; // 黑方先手

    // 销毁临时窗口，创建游戏主窗口
    SDL_DestroyWindow(window);

    int windowSize = BOARD_SIZE * CELL_SIZE + 2 * MARGIN + 50;
    window = SDL_CreateWindow("Renju GUI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowSize, windowSize, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(buttonFont);
        TTF_CloseFont(moveFont);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(renderer, windowSize, windowSize);

    SDL_Surface *bgSurface = IMG_Load("photos/zgz.png");
    if (!bgSurface) {
        SDL_Log("Unable to load image: %s", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(buttonFont);
        TTF_CloseFont(moveFont);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(buttonFont);
        TTF_CloseFont(moveFont);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return;
    }

    float opacity = 1;
    SDL_SetWindowOpacity(window, opacity);

    GameManager gameManager;
    gameManager.NewGame();

    int drawableWidth, drawableHeight;
    SDL_GetRendererOutputSize(renderer, &drawableWidth, &drawableHeight);

    bool running = true;
    bool dragging = false;
    int sliderWidth = 100;
    int sliderHeight = 5;
    int sliderX = drawableWidth / 2 - sliderWidth - GAP * 2;
    int sliderY = GAP + BUTTON_RADIUS - sliderHeight / 2;

    int offsetX = (drawableWidth / 2 - (BOARD_SIZE - 1) * CELL_SIZE - LINE_WIDTH) / 2 - MARGIN;
    int offsetY = (drawableHeight / 2 - (BOARD_SIZE - 1) * CELL_SIZE - LINE_WIDTH) / 2 - MARGIN;

    SDL_Log("Drawable Size: %d x %d", drawableWidth, drawableHeight);
    SDL_Log("OffsetX: %d, OffsetY: %d", offsetX, offsetY);

    gameManager.NewGame();

    while (running) {
        SDL_GetMouseState(&mouseX, &mouseY);

        // 检查鼠标是否悬停在按钮上
        bool isMouseOnExitButton = (sqrt(pow(mouseX - exitButtonX, 2) + pow(mouseY - exitButtonY, 2)) <= BUTTON_RADIUS);
        bool isMouseOnSaveButton = (sqrt(pow(mouseX - saveButtonX, 2) + pow(mouseY - saveButtonY, 2)) <= BUTTON_RADIUS);
        bool isMouseOnLoadButton = (sqrt(pow(mouseX - loadButtonX, 2) + pow(mouseY - loadButtonY, 2)) <= BUTTON_RADIUS);
        bool isMouseOnUndoButton = (sqrt(pow(mouseX - undoButtonX, 2) + pow(mouseY - undoButtonY, 2)) <= BUTTON_RADIUS);

        if ((!playerIsBlack && isBlackTurn) || (playerIsBlack && !isBlackTurn)) {
            auto [aiRow, aiCol] = gameManager.GetBestMove(isBlackTurn ? 1 : 2);
            if (gameManager.board.GetCell(aiRow, aiCol) == 0) {
                // 检查禁手，如果禁手直接判AI输，结束游戏
                if (!playerIsBlack && isForbiddenMove(gameManager.board.board, aiRow, aiCol)) {
                    gameManager.board.SetCell(aiRow, aiCol, isBlackTurn ? 1 : 2);
                    gameManager.SetLastMove(aiRow, aiCol);
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                    const char *winnerText = "You Win!";
                    DrawCurrentPlayer(renderer, winnerText, windowSize, {50, 50, 255, 255});
                    SDL_RenderPresent(renderer);
                    SDL_Delay(1000);
                    for (int i = 3; i > 0; i--) {
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                        std::string counterText = "Auto Exit in ";
                        char countText[10];
                        std::snprintf(countText, sizeof(countText), "%d", i);
                        counterText += countText;
                        DrawCurrentPlayer(renderer, counterText.c_str(), windowSize, {75, 75, 255, 255});
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    running = false;
                    continue;
                }
                gameManager.board.SetCell(aiRow, aiCol, isBlackTurn ? 1 : 2);
                gameManager.SetLastMove(aiRow, aiCol);
                if (gameManager.CheckWin()) {
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                    DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));
                    // 绘制按钮
                    DrawButton(renderer, exitButtonX, exitButtonY, isMouseOnExitButton, 0, buttonFont);
                    DrawButton(renderer, saveButtonX, saveButtonY, isMouseOnSaveButton, 1, buttonFont);
                    DrawButton(renderer, loadButtonX, loadButtonY, isMouseOnLoadButton, 2, buttonFont);
                    DrawButton(renderer, undoButtonX, undoButtonY, isMouseOnUndoButton, 3, buttonFont);
                    // 显示游戏结束（You Win or You Lose）
                    const char *winnerText = "You Lose!";
                    DrawCurrentPlayer(renderer, winnerText, windowSize, {255, 50, 50, 255});
                    SDL_RenderPresent(renderer);
                    // 展示一秒
                    SDL_Delay(1000);

                    // 显示五秒退出倒计时
                    for (int i = 3; i > 0; i--) {
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                        std::string counterText = "Auto Exit in ";
                        char countText[10];
                        std::snprintf(countText, sizeof(countText), "%d", i);
                        counterText += countText;
                        DrawCurrentPlayer(renderer, counterText.c_str(), windowSize, {255, 75, 75, 255});
                        SDL_RenderPresent(renderer);
                        SDL_Delay(1000);
                    }
                    running = false;
                    continue;
                }
                isBlackTurn = !isBlackTurn;
            } else {
                std::cout << "AI落子错误" << gameManager.board.GetCell(aiRow, aiCol) << std::endl;
            }
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                const SDL_MessageBoxButtonData buttons[] = {
                    {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "是"},
                    {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "否"},
                    {0, 2, "取消"}};
                const SDL_MessageBoxColorScheme colorScheme = {
                    {/* background color */
                        {255, 255, 255},
                        /* text color */
                        {0, 0, 0},
                        /* button border color */
                        {0, 0, 0},
                        /* button background color */
                        {255, 255, 255},
                        /* button selected color */
                        {0, 0, 0}}};
                const SDL_MessageBoxData messageboxdata = {
                    SDL_MESSAGEBOX_INFORMATION, /* .flags */
                    NULL,                       /* .window */
                    "退出游戏",                 /* .title */
                    "是否保存游戏进度?",        /* .message */
                    SDL_arraysize(buttons),     /* .numbuttons */
                    buttons,                    /* .buttons */
                    &colorScheme                /* .colorScheme */
                };
                int buttonid;
                if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
                    SDL_Log("Error displaying message box");
                    continue;
                }
                if (buttonid == 1) {
                    gameManager.SaveGame("save.txt");
                    SDL_Log("游戏已保存");
                    running = false;
                } else if (buttonid == 0) {
                    SDL_Log("游戏未保存");
                    running = false;
                } else {
                    SDL_Log("取消退出");
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouseX = event.button.x;
                mouseY = event.button.y;

                // 先处理按钮点击
                if (isMouseOnExitButton) {
                    const SDL_MessageBoxButtonData buttons[] = {
                        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "是"},
                        {SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "否"},
                        {0, 2, "取消"}};
                    const SDL_MessageBoxColorScheme colorScheme = {
                        {/* background color */
                            {255, 255, 255},
                            /* text color */
                            {0, 0, 0},
                            /* button border color */
                            {0, 0, 0},
                            /* button background color */
                            {255, 255, 255},
                            /* button selected color */
                            {0, 0, 0}}};
                    const SDL_MessageBoxData messageboxdata = {
                        SDL_MESSAGEBOX_INFORMATION, /* .flags */
                        NULL,                       /* .window */
                        "退出游戏",                 /* .title */
                        "是否保存游戏进度?",        /* .message */
                        SDL_arraysize(buttons),     /* .numbuttons */
                        buttons,                    /* .buttons */
                        &colorScheme                /* .colorScheme */
                    };
                    int buttonid;
                    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
                        SDL_Log("Error displaying message box");
                        continue;
                    }
                    if (buttonid == 1) {
                        gameManager.SaveGame("save.txt");
                        SDL_Log("游戏已保存");
                        running = false;
                    } else if (buttonid == 0) {
                        SDL_Log("游戏未保存");
                        running = false;
                    } else {
                        SDL_Log("取消退出");
                    }
                    continue; // 跳过后续的棋盘点击检测
                }

                if (isMouseOnSaveButton) {
                    gameManager.SaveGame("save.txt");
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                        "提示", "游戏已保存", window);
                    continue; // 跳过后续的棋盘点击检测
                }

                if (isMouseOnLoadButton) {
                    gameManager.LoadGame("save.txt");
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                        "提示", "游戏已读取", window);
                    continue; // 跳过后续的棋盘点击检测
                }

                if (isMouseOnUndoButton) {
                    gameManager.UndoMove();
                    continue;
                }

                // 检查是否在滑块上点击
                if (mouseX >= sliderX && mouseX <= sliderX + sliderWidth && mouseY >= sliderY && mouseY <= sliderY + sliderHeight) {
                    dragging = true;
                }

                // 只在玩家回合才处理鼠标点击
                if ((playerIsBlack && isBlackTurn) || (!playerIsBlack && !isBlackTurn)) {
                    int x = event.button.x - offsetX - MARGIN;
                    int y = event.button.y - offsetY - MARGIN;

                    if (x >= 0 && y >= 0) {
                        int col = (x + CELL_SIZE / 2) / CELL_SIZE;
                        int row = (y + CELL_SIZE / 2) / CELL_SIZE;
                        if (col >= 0 && col < BOARD_SIZE && row >= 0 && row < BOARD_SIZE) {
                            if (gameManager.board.GetCell(row + 1, col + 1) == 0) {
                                // 检查禁手
                                if (isForbiddenMove(gameManager.board.board, row + 1, col + 1)) {
                                    const char *forbiddenText = "Forbidden Move!";
                                    DrawCurrentPlayer(renderer, forbiddenText, windowSize, {255, 50, 50, 255});
                                    SDL_RenderPresent(renderer);
                                    SDL_Delay(1000);
                                    continue;
                                }
                                // 玩家落子
                                gameManager.board.SetCell(row + 1, col + 1, isBlackTurn ? 1 : 2);
                                gameManager.SetLastMove(row + 1, col + 1);
                                // 检查胜负
                                if (gameManager.CheckWin()) {
                                    SDL_RenderClear(renderer);
                                    SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                                    DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                                    DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));
                                    // 绘制按钮
                                    DrawButton(renderer, exitButtonX, exitButtonY, isMouseOnExitButton, 0, buttonFont);
                                    DrawButton(renderer, saveButtonX, saveButtonY, isMouseOnSaveButton, 1, buttonFont);
                                    DrawButton(renderer, loadButtonX, loadButtonY, isMouseOnLoadButton, 2, buttonFont);
                                    DrawButton(renderer, undoButtonX, undoButtonY, isMouseOnUndoButton, 3, buttonFont);
                                    // 显示游戏结束（You Win or You Lose）
                                    const char *winnerText = "You Win!";
                                    DrawCurrentPlayer(renderer, winnerText, windowSize, {255, 50, 50, 255});
                                    SDL_RenderPresent(renderer);
                                    // 展示一秒
                                    SDL_Delay(1000);

                                    // 显示五秒退出倒计时
                                    for (int i = 3; i > 0; i--) {
                                        SDL_RenderClear(renderer);
                                        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);
                                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                                        DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
                                        DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));
                                        // 绘制按钮
                                        DrawButton(renderer, exitButtonX, exitButtonY, isMouseOnExitButton, 0, buttonFont);
                                        DrawButton(renderer, saveButtonX, saveButtonY, isMouseOnSaveButton, 1, buttonFont);
                                        DrawButton(renderer, loadButtonX, loadButtonY, isMouseOnLoadButton, 2, buttonFont);
                                        DrawButton(renderer, undoButtonX, undoButtonY, isMouseOnUndoButton, 3, buttonFont);
                                        std::string counterText = "Auto Exit in ";
                                        char countText[10];
                                        std::snprintf(countText, sizeof(countText), "%d", i);
                                        counterText += countText;
                                        DrawCurrentPlayer(renderer, counterText.c_str(), windowSize, {255, 75, 75, 255});
                                        SDL_RenderPresent(renderer);
                                        SDL_Delay(1000);
                                    }
                                    running = false;
                                    continue;
                                }
                                isBlackTurn = !isBlackTurn;
                            }
                        }
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                dragging = false;
            } else if (event.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    mouseX = event.motion.x;
                    mouseY = event.motion.y;
                    if (mouseX < sliderX) {
                        mouseX = sliderX;
                    } else if (mouseX > sliderX + sliderWidth) {
                        mouseX = sliderX + sliderWidth;
                    }
                    float newValue = static_cast<float>(mouseX - sliderX) / sliderWidth;
                    opacity = MIN_OPACITY + newValue * (1.0f - MIN_OPACITY);
                    SDL_SetWindowOpacity(window, opacity);
                }
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        DrawBoard(renderer, offsetX, offsetY, gameManager.board.board, {gameManager.GetLastMoveX(), gameManager.GetLastMoveY()});
        DrawSlider(renderer, sliderX, sliderY, sliderWidth, sliderHeight, (opacity - MIN_OPACITY) / (1.0f - MIN_OPACITY));

        // 绘制按钮
        DrawButton(renderer, exitButtonX, exitButtonY, isMouseOnExitButton, 0, buttonFont);
        DrawButton(renderer, saveButtonX, saveButtonY, isMouseOnSaveButton, 1, buttonFont);
        DrawButton(renderer, loadButtonX, loadButtonY, isMouseOnLoadButton, 2, buttonFont);
        DrawButton(renderer, undoButtonX, undoButtonY, isMouseOnUndoButton, 3, buttonFont);

        // 显示当前轮到谁下棋
        const char *currentPlayerText = (playerIsBlack == isBlackTurn) ? "Player's Turn" : "AI's Turn";
        SDL_Color currentPlayerColor = (playerIsBlack == isBlackTurn) ? SDL_Color{0, 0, 0, 255} : SDL_Color{255, 255, 255, 255};
        DrawCurrentPlayer(renderer, currentPlayerText, windowSize, currentPlayerColor);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(buttonFont);
    TTF_CloseFont(moveFont);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}