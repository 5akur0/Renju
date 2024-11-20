#include "ForbiddenMove.h"
#include <algorithm>
#include <string>

#include "Evaluate.h"

int state[225];
int flag;

bool isForbiddenMove(int board[16][16], int x, int y)
{
    board[x][y] = C_BLACK;
    make_state(board);
    flag = (x - 1) * 15 + y - 1;
    board[x][y] = C_NONE;
    if (long_connect()) {
        return true;
    }
    if (five_connect()) {
        return false;
    }
    if (three_three() || four_four()) {
        return true;
    }
    return false;
}

void make_state(int board[16][16])
{
    for (int i = 0; i < 225; ++i) {
        if (board[i / 15 + 1][i % 15 + 1] == C_BLACK) {
            state[i] = 1;
        } else if (board[i / 15 + 1][i % 15 + 1] == C_WHITE) {
            state[i] = -1;
        } else {
            state[i] = 0;
        }
    }
}

bool long_connect()
{
    int h = flag / 15;
    int w = flag % 15;
    int last_move = flag;
    int ret;

    int bias = std::min(w, 5);
    for (int i = last_move - bias; i < last_move + 1; i++) {
        if (15 - 1 - i % 15 < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6; k++) {
            if (state[k] != C_BLACK) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(h, 5);
    for (int i = last_move - bias * 15; i < last_move + 15; i += 15) {
        if (15 - 1 - i / 15 < 5) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * 15; k += 15) {
            if (state[k] != C_BLACK) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(std::min(h, 5), std::min(w, 5));
    for (int i = last_move - bias * 15 - bias; i < last_move + 15 + 1; i += 15 + 1) {
        if ((15 - 1 - i / 15 < 5) || (15 - 1 - i % 15 < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k < i + 6 * 15 + 6; k += 15 + 1) {
            if (state[k] != C_BLACK) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
    bias = std::min(std::min(15 - 1 - h, 5), std::min(w, 5));
    for (int i = last_move + bias * 15 - bias; i > last_move - 15 + 1; i += (-(15) + 1)) {
        if ((15 - 1 - i % 15 < 5) || (i / 15 < 5)) {
            break;
        }
        ret = 0;
        for (int k = i; k > i - 6 * 15 + 6; k += (-(15) + 1)) {
            if (state[k] != C_BLACK) {
                ret = 1;
                break;
            }
        }
        if (ret == 0)
            return true;
    }
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

bool three_three()
{
    int h = flag / 15;
    int w = flag % 15;
    int last_move = flag;
    int width = 15;
    std::string jt1 = "o1o11o";
    std::string jt2 = "o11o1o";
    std::string ct1 = "oo111o";
    std::string ct2 = "o111oo";
    int three = 0;
    std::string m_str;
    size_t pos;
    int bias = std::min(w, 4);
    for (int i = last_move - bias; i < last_move + std::min(width - 1 - w, 4) + 1; i++) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
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
    bias = std::min(h, 4);
    for (int i = last_move - bias * width; i < last_move + width * std::min(width - 1 - h, 4) + width; i += width) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
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
    bias = std::min(std::min(h, 4), std::min(w, 4));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * std::min(std::min(width - 1 - h, width - 1 - w), 4) + width + 1; i += width + 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
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
    bias = std::min(std::min(width - 1 - w, 4), std::min(h, 4));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * std::min(std::min(width - 1 - h, std::min(w, 4)), 4) + width - 1; i += width - 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(ct1);
    if (pos != m_str.npos) {
        if (tt_special_case(m_str, pos, 1) == false)
            three++;
    } else {
        pos = m_str.find(ct2);
        if (pos != m_str.npos) {
            if (tt_special_case(m_str, pos, 2) == false)
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

bool four_four()
{
    int h = flag / 15;
    int w = flag % 15;
    int last_player = C_BLACK;
    int last_move = flag;
    int width = 15;
    size_t pos;
    std::string jf1 = "111o1";
    std::string jf2 = "1o111";
    std::string jf3 = "11o11";
    std::string cf1 = "o1111";
    std::string cf2 = "1111o";
    int four = 0;
    std::string m_str;
    int bias;

    bias = std::min(w, 5);
    for (int i = last_move - bias; i < last_move + std::min(width - 1 - w, 5) + 1; i++) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = std::min(h, 5);
    for (int i = last_move - bias * width; i < last_move + width * std::min(width - 1 - h, 5) + width; i += width) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = std::min(std::min(h, 5), std::min(w, 5));
    for (int i = last_move - bias * width - bias; i < last_move + (width + 1) * std::min(std::min(width - 1 - h, width - 1 - w), 5) + width + 1; i += width + 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;

    m_str.clear();
    bias = std::min(std::min(width - 1 - w, 5), std::min(h, 5));
    for (int i = last_move - bias * (width - 1); i < last_move + (width - 1) * std::min(std::min(width - 1 - h, std::min(w, 5)), 5) + width - 1; i += width - 1) {
        if (state[i] == -1) {
            m_str.append(1, 'o');
        } else if (state[i] == 0) {
            m_str.append(1, '0');
        } else {
            m_str.append(1, '1');
        }
    }
    pos = m_str.find(jf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf2);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(jf3);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 1) == false)
            four++;
    }
    pos = m_str.find(cf1);
    if (pos != m_str.npos) {
        if (ff_special_case(m_str, pos, 2) == false)
            four++;
    } else {
        pos = m_str.find(cf2);
        if (pos != m_str.npos) {
            if (ff_special_case(m_str, pos, 3) == false)
                four++;
        }
    }
    if (four > 1)
        return true;
    return false;
}

bool five_connect()
{
    int h = flag / 15;
    int w = flag % 15;
    int last_player = C_BLACK;
    int last_move = flag;
    int i, j;
    int ret;
    if (flag == -1)
        return false;
    int bias = std::min(w, 4);
    for (i = last_move - bias; i < last_move + 1; i++) {
        if (15 - 1 - i % (15) < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5; j++) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = std::min(h, 4);
    for (i = last_move - bias * 15; i < last_move + 15; i += 15) {
        if (15 - 1 - i / 15 < 4) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * 15; j += 15) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = std::min(std::min(h, 4), std::min(w, 4));
    for (i = last_move - bias * 15 - bias; i < last_move + 15 + 1; i += 15 + 1) {
        if ((15 - 1 - i / 15 < 4) || (15 - 1 - i % 15 < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j < i + 5 * 15 + 5; j += 15 + 1) {

            if (state[j] != last_player) {
                ret = 1;
                break;
            }
        }
        if (ret == 0) {
            return true;
        }
    }
    bias = std::min(std::min(15 - 1 - h, 4), std::min(w, 4));
    for (i = last_move + bias * 15 - bias; i > last_move - 15 + 1; i = i - 15 + 1) {
        if ((15 - 1 - i % 15 < 4) || (i / 15 < 4)) {
            break;
        }
        ret = 0;
        for (j = i; j > i - 5 * 15 + 5; j = j - 15 + 1) {
            if (state[j] != last_player) {
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