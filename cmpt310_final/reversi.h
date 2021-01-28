#pragma once

#include <string>

using namespace std;

class Reversi{
public:
	Reversi(); //Standard Constructor
	Reversi(char current_board[8][8]);
	~Reversi(); //Destructor
	char board[8][8]; //Main Atribute


	//Helper Functons
	void display();
	void hint_display(int* moves);
	int * valid_move(char current_board[8][8], char color, int turn);
	void input_move(char current_board[8][8], char color, int turn, int x_move, int y_move);
	int get_black_score(char current_board[8][8]);
	int get_white_score(char current_board[8][8]);
};