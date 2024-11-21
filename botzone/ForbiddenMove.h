#ifndef FORBIDDENMOVE_H
#define FORBIDDENMOVE_H
#include "Evaluate.h"
#include <string>
#include <cstddef>

bool isForbiddenMove(int board[16][16], int x, int y);
void make_state(int board[16][16]);
bool long_connect();
bool three_three();
bool four_four();
bool five_connect();
bool tt_special_case(std::string& m_str, size_t pos, int t_case);
bool ff_special_case(std::string& m_str, size_t pos, int f_case);


#endif // FORBIDDENMOVE_H