#include<vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <queue>
#include "jsoncpp/json.h"
using namespace std;
#define BOARD_LEN 15
class Board
{
private:
    unsigned int boardLen = BOARD_LEN;

public:
    const int EMPTY = -1;
    const int BLACK = 1;
    const int WHITE = 0;
    const int notOver = 2;
    const int flatFlag = 3;
    unsigned int numPieces = 0;
    int undoState[225];
    int currentPlayer = BLACK;
    int preAction = -1;
    std::vector<int> state;
    std::vector<int> availableAction;
    int board[BOARD_LEN][BOARD_LEN] = { EMPTY };
    Board();
    Board(const Board* board);
    ~Board();
    void reset();
    bool inBoard(int x, int y);
    std::vector<int> getAvailable();
    void doAction(int action);
    void undo();
    int getNumPipces();
    int isGameOver();
    int win_end();
    bool long_connect();
    bool tt_special_case(std::string& m_str, size_t pos, int t_case);
    bool ff_special_case(std::string& m_str, size_t pos, int f_case);
    bool three_three();
    bool four_four();
    bool five_connect();
};

int Board::win_end() {
    int last_player = this->currentPlayer ^ 1;
    if (last_player == BLACK) {
        if (long_connect()) return WHITE;
        if (five_connect()) return BLACK;
        if (three_three()) return WHITE;
        if (four_four()) return WHITE;
    }
    else {
        if (this->five_connect()) return WHITE;
    }
    return notOver;
}

bool Board::long_connect() {
    int h = this->preAction / BOARD_LEN;
    int w = this->preAction % BOARD_LEN;
    int last_move = this->preAction;
    int ret;

    int bias = std::min(w, 5);
    for (int i = last_move - bias; i < last_move + 1; i++) {
        if (BOARD_LEN - 1 - i % BOARD_LEN < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6; k++) {
            if (this->state[k] != (this->currentPlayer ^ 1)) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) return true;
    }
    bias = std::min(h, 5);
    for (int i = last_move - bias * BOARD_LEN; i < last_move + BOARD_LEN; i += BOARD_LEN) {
        if (BOARD_LEN - 1 - i / BOARD_LEN < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * BOARD_LEN; k += BOARD_LEN) {
            if (this->state[k] != (this->currentPlayer ^ 1)) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) return true;
    }
    bias = std::min(std::min(h, 5), std::min(w, 5));
    for (int i = last_move - bias * BOARD_LEN - bias; i < last_move + BOARD_LEN + 1; i += BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i / BOARD_LEN < 5) || (BOARD_LEN - 1 - i % BOARD_LEN < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * BOARD_LEN + 6; k += BOARD_LEN + 1) {
            if (this->state[k] != (this->currentPlayer ^ 1)) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) return true;
    }
    bias = std::min(std::min(BOARD_LEN - 1 - h, 5), std::min(w, 5));
    for (int i = last_move + bias * BOARD_LEN - bias; i > last_move - BOARD_LEN + 1; i += (-(BOARD_LEN) + 1)) {
        if ((BOARD_LEN - 1 - i % BOARD_LEN < 5) || (i / BOARD_LEN < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k > i - 6 * BOARD_LEN + 6; k += (-(BOARD_LEN) + 1)) {
            if (this->state[k] != (this->currentPlayer ^ 1)) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) return true;
    }
    return false;
}

bool Board::tt_special_case(std::string& m_str, size_t pos, int t_case) {
    if (t_case == 1) { //oo111o
        if (pos + 6 < m_str.size()) {
            if (m_str[pos + 6] == '1') return true;
        }
    }
    else { //o111oo
        if (pos > 0) {
            if (m_str[pos - 1] == '1') return true;
        }
    }
    return false;
}

bool Board::ff_special_case(std::string& m_str, size_t pos, int f_case) {
    if (f_case == 1) { //oo111o
        if (pos > 0) {
            if (m_str[pos - 1] == '1') return true;
        }
        if (pos + 5 < m_str.size()) {
            if (m_str[pos + 5] == '1') return true;
        }
        return false;
    }
    else if (f_case == 2) {
        if (pos > 0) {
            if (pos + 6 < m_str.size()) {
                if (m_str[pos - 1] == '1' && ((
                                                  m_str[pos + 5] == 'o' && m_str[pos + 6] == '1') || (
                                                  m_str[pos + 5] == '0'))) return true;
                return false;
            }
            if (pos + 5 < m_str.size()) {
                if (m_str[pos - 1] == '1' && m_str[pos + 5] == '0') return true;
                return false;
            }
            if (m_str[pos - 1] == '1') return true;
            return false;
        }
        else return false;
    }
    else {
        if (pos + 5 < m_str.size()) {
            if (pos - 2 >= 0) {
                if ((m_str[pos - 2] == '1' && m_str[pos - 1] == 'o') || (m_str[pos - 1] == '0') && m_str[pos + 5] == '1')
                    return true;
                return false;
            }
            else if (pos - 1 >= 0) {
                if (m_str[pos + 5] == '1' && m_str[pos - 1] == '0') return true;
                return false;
            }
            if (m_str[pos + 5] == '1') return true;
            return false;
        }
        else return false;
    }
}

bool Board::three_three() {
    int h = this->preAction / BOARD_LEN;
    int w = this->preAction % BOARD_LEN;
    int last_move = this->preAction;
    int width = BOARD_LEN;
    string jt1 = "o1o11o";
    string jt2 = "o11o1o";
    string ct1 = "oo111o";
    string ct2 = "o111oo";
    int three = 0;
    string m_str;
    size_t pos;
    int bias = min(w, 4);
    for (int i = last_move - bias; i < last_move + min(width - 1 - w, 4) + 1; i++) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false) three++;
    }
    else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false) three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos) three++;
    if (m_str.find(jt2) != m_str.npos) three++;
    if (three > 1) return true;

    m_str.clear();
    bias = min(h, 4);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 4) + width; i += width) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false) three++;
    }
    else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false) three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos) three++;
    if (m_str.find(jt2) != m_str.npos) three++;
    if (three > 1) return true;

    m_str.clear();
    bias = min(min(h, 4), min(w, 4));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 4) + width + 1; i += width + 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false) three++;
    }
    else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false) three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos) three++;
    if (m_str.find(jt2) != m_str.npos) three++;
    if (three > 1) return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 4), min(h, 4));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 4)), 4) + width - 1; i += width - 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false) three++;
    }
    else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false) three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos) three++;
    if (m_str.find(jt2) != m_str.npos) three++;
    if (three > 1) return true;

    return false;
}

bool Board::four_four() {
    int h = this->preAction / BOARD_LEN;
    int w = this->preAction % BOARD_LEN;
    int last_player = this->currentPlayer ^ 1;
    int last_move = this->preAction;
    int width = BOARD_LEN;
    size_t pos;
    string jf1 = "111o1";
    string jf2 = "1o111";
    string jf3 = "11o11";
    string cf1 = "o1111";
    string cf2 = "1111o";
    int four = 0;
    string m_str;
    int bias;

    bias = min(w, 5);
    for (int i = last_move - bias; i < last_move + min(width - 1 - w, 5) + 1; i++) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false) four++;
    }
    else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false) four++;
        }
    }
    if (four > 1) return true;

    m_str.clear();
    bias = min(h, 5);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 5) + width; i += width) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false) four++;
    }
    else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false) four++;
        }
    }
    if (four > 1) return true;

    m_str.clear();
    bias = min(min(h, 5), min(w, 5));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 5) + width + 1; i += width + 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false) four++;
    }
    else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false) four++;
        }
    }
    if (four > 1) return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 5), min(h, 5));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 5)), 5) + width - 1; i += width - 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        }
        else if (this->state[i] == 0) {
            m_str.append(1, '0');
        }
        else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false) four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false) four++;
    }
    else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false) four++;
        }
    }
    if (four > 1) return true;
    return false;
}

bool Board::five_connect() {
    int h = this->preAction / BOARD_LEN;
    int w = this->preAction % BOARD_LEN;
    int last_player = this->currentPlayer ^ 1;
    int last_move = this->preAction;
    int i, j;
    int ret;
    if (this->preAction == -1) return false;
    int bias = min(w, 4);
    for (i = last_move - bias; i < last_move + 1; i++) {
        if (BOARD_LEN - 1 - i % (BOARD_LEN) < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5; j++) {

            if (this->state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = min(h, 4);
    for (i = last_move - bias * BOARD_LEN; i < last_move + BOARD_LEN; i += BOARD_LEN) {
        if (BOARD_LEN - 1 - i / BOARD_LEN < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * BOARD_LEN; j += BOARD_LEN) {

            if (this->state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = min(min(h, 4), min(w, 4));
    for (i = last_move - bias * BOARD_LEN - bias; i < last_move + BOARD_LEN + 1; i += BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i / BOARD_LEN < 4) || (BOARD_LEN - 1 - i % BOARD_LEN < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * BOARD_LEN + 5; j += BOARD_LEN + 1) {

            if (this->state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = std::min(std::min(BOARD_LEN - 1 - h, 4), std::min(w, 4));
    for (i = last_move + bias * BOARD_LEN - bias; i > last_move - BOARD_LEN + 1; i = i - BOARD_LEN + 1) {
        if ((BOARD_LEN - 1 - i % BOARD_LEN < 4) || (i / BOARD_LEN < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j > i - 5 * BOARD_LEN + 5; j = j - BOARD_LEN + 1) {
            if (this->state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    return false;
}


const int BLACK = 1; //棋盘为黑
const int WHITE = -1; // 棋盘为白
const int EMPTY = 0; // 棋盘为空
