#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>

static const int BOARD_SIZE = 15;
static const int CELL_SIZE = 40;
static const int MARGIN = 50;

int main()
{
    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE * CELL_SIZE + 2 * MARGIN,
                                BOARD_SIZE * CELL_SIZE + 2 * MARGIN),
        "Renju GUI");

    // 用于存储棋子（1黑、2白、0无）
    std::vector<std::vector<int>> board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, 0));
    bool isBlackTurn = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x - MARGIN;
                    int y = event.mouseButton.y - MARGIN;
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

        window.clear(sf::Color::White);

        // 绘制网格
        for (int i = 0; i < BOARD_SIZE; i++) {
            // 竖线
            sf::Vertex lineV[] = {
                sf::Vertex(sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN)),
                sf::Vertex(sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN + (BOARD_SIZE - 1) * CELL_SIZE))
            };
            window.draw(lineV, 2, sf::Lines);

            // 横线
            sf::Vertex lineH[] = {
                sf::Vertex(sf::Vector2f(MARGIN, MARGIN + i * CELL_SIZE)),
                sf::Vertex(sf::Vector2f(MARGIN + (BOARD_SIZE - 1) * CELL_SIZE, MARGIN + i * CELL_SIZE))
            };
            window.draw(lineH, 2, sf::Lines);
        }

        // 绘制棋子
        for (int r = 0; r < BOARD_SIZE; ++r) {
            for (int c = 0; c < BOARD_SIZE; ++c) {
                if (board[r][c] != 0) {
                    sf::CircleShape piece(CELL_SIZE * 0.4f);
                    piece.setPosition(MARGIN + c * CELL_SIZE - piece.getRadius(),
                        MARGIN + r * CELL_SIZE - piece.getRadius());
                    if (board[r][c] == 1) {
                        piece.setFillColor(sf::Color::Black);
                    } else {
                        piece.setFillColor(sf::Color::White);
                        piece.setOutlineColor(sf::Color::Black);
                        piece.setOutlineThickness(1.5f);
                    }
                    window.draw(piece);
                }
            }
        }

        window.display();
    }
    return 0;
}