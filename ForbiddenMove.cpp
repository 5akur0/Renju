#include "ForbiddenMove.h"
#include <algorithm>
#include <string>

#include "Evaluate.h"

bool isForbiddenMove(int board[16][16], int x, int y)
{
    return false;
}

/*
bool check_direction(int board[16][16], int x, int y, int dx, int dy)
{
    int count = 0;
    for (int i = -5; i <= 0; ++i) {
        count = 0;
        for (int j = 0; j < 6; ++j) {
            int nx = x + (i + j) * dx;
            int ny = y + (i + j) * dy;
            if (nx >= 1 && nx <= 15 && ny >= 1 && ny <= 15 && board[nx][ny] == C_BLACK) {
                count++;
            } else {
                break;
            }
        }
        if (count == 6) {
            return true;
        }
    }
    return false;
}

bool long_connect(int board[16][16], int x, int y)
{
    // 检查水平方向
    if (check_direction(board, x, y, 0, 1))
        return true;
    // 检查垂直方向
    if (check_direction(board, x, y, 1, 0))
        return true;
    // 检查左上到右下方向
    if (check_direction(board, x, y, 1, 1))
        return true;
    // 检查右上到左下方向
    if (check_direction(board, x, y, 1, -1))
        return true;

    return false;
}

bool tt_special_case(std::string& m_str, size_t pos, int t_case)
{
    if (t_case == 1) { // oo111o
        if (pos + 6 < m_str.size()) {
            if (m_str[pos + 6] == '1')
                return true;
        }
    } else { // o111oo
        if (pos > 0) {
            if (m_str[pos - 1] == '1')
                return true;
        }
    }
    return false;
}

bool ff_special_case(std::string& m_str, size_t pos, int f_case)
{
    if (f_case == 1) { // oo111o
        if (pos > 0) {
            if (m_str[pos - 1] == '1')
                return true;
        }
        if (pos + 5 < m_str.size()) {
            if (m_str[pos + 5] == '1')
                return true;
        }
        return false;
    } else if (f_case == 2) {
        if (pos > 0) {
            if (pos + 6 < m_str.size()) {
                if (m_str[pos - 1] == '1' && ((m_str[pos + 5] == 'o' && m_str[pos + 6] == '1') || (m_str[pos + 5] == '0')))
                    return true;
                return false;
            }
            if (pos + 5 < m_str.size()) {
                if (m_str[pos - 1] == '1' && m_str[pos + 5] == '0')
                    return true;
                return false;
            }
            if (m_str[pos - 1] == '1')
                return true;
            return false;
        } else
            return false;
    } else {
        if (pos + 5 < m_str.size()) {
            if (pos - 2 >= 0) {
                if ((m_str[pos - 2] == '1' && m_str[pos - 1] == 'o') || (m_str[pos - 1] == '0') && m_str[pos + 5] == '1')
                    return true;
                return false;
            } else if (pos - 1 >= 0) {
                if (m_str[pos + 5] == '1' && m_str[pos - 1] == '0')
                    return true;
                return false;
            }
            if (m_str[pos + 5] == '1')
                return true;
            return false;
        } else
            return false;
    }
}

bool three_three(int board[16][16], int x, int y)
{
    std::string jt1 = "o1o11o";
    std::string jt2 = "o11o1o";
    std::string ct1 = "oo111o";
    std::string ct2 = "o111oo";
    int three = 0;
    std::string m_str;
    size_t pos;
    int bias = min(w, 4);
    for (int i = last_move - bias; i < last_move + min(width - 1 - w, 4) + 1; i++) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(h, 4);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 4) + width; i += width) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(min(h, 4), min(w, 4));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 4) + width + 1; i += width + 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 4), min(h, 4));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 4)), 4) + width - 1; i += width - 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (this->tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (this->tt_special_case(m_str, pos, 2) == false)
                three++;
        }
    }
    if (m_str.find(jt1) != m_str.npos)
        three++;
    if (m_str.find(jt2) != m_str.npos)
        three++;
    if (three > 1)
        return true;

    return false;
}

bool four_four(int board[16][16], int x, int y)
{
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
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(h, 5);
    for (int i = last_move - bias * width; i < last_move + width * min(width - 1 - h, 5) + width; i += width) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(min(h, 5), min(w, 5));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * min(min(width - 1 - h, width - 1 - w), 5) + width + 1; i += width + 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = min(min(width - 1 - w, 5), min(h, 5));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * min(min(width - 1 - h, min(w, 5)), 5) + width - 1; i += width - 1) {
        if (this->state[i] == -1) {
            m_str.append(1, 'o');
        } else if (this->state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (this->ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (this->ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;
    return false;
}
*/