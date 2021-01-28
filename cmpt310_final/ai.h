#pragma once

#include <string>
#include "reversi.h"

using namespace std;

class AI{
public:
	string pure_MCT(char current_board[8][8], int playout, string valid_moves[32], int counter, char color, int turn);
	string heuristicP_MCT(char current_board[8][8], int playout, string valid_moves[32], int counter, char color, int turn);
};