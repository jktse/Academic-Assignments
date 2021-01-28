/*
*	game.cpp - Implementation of the game class
*
*	Class Description: Contains information about the state of the game
*							methods to determin the available moves
*
*/

#include <iostream>
#include <string>
#include <vector>
#include "reversi.h"

using namespace std;


//Default Contructor
//Description: Created a the default game state. Where the board is all empty
Reversi::Reversi(){
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			board[i][j] = '*';
		}
	}

}

//Contructor that is meant to create a reversi game object in a specific state
Reversi::Reversi(char current_board[8][8]){
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			board[i][j] = current_board[i][j];
		}
	}
}

//Destructor: Destroys the game object when it goes out of scope
Reversi::~Reversi(){
}

//Prints the state of the board to the user
void Reversi::display(){
	cout << "	  ";
	for (int i = 0; i < 8; i ++){
		cout << char(65 + i) << " ";
	}
	cout << "\n";
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (j == 0){
				cout << "	" << 8 - i << " ";
			}
			cout << board[i][j] << " ";
		}
		cout << "\n";
	}
}

//Prints the state of the board with some hints
void Reversi::hint_display(int* moves){
	cout << "	  ";
	for (int i = 0; i < 8; i ++){
		cout << char(65 + i) << " ";
	}
	cout << "\n";
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (j == 0){
				cout << "	" << 8 - i << " ";
			}
			if (moves[(i*8) + j] == 1){
				cout << "! ";
			}else{
				cout << board[i][j] << " ";
			}
		}
		cout << "\n";
	}
}

// Returns an array containing location for valid moves
int* Reversi::valid_move(char current_board[8][8], char color, int turn){
	static int moves[64];
	int store_x;
	int store_y;
	char opp_color;
	bool marked;
	if (color == 'B'){
		opp_color = 'W';
	}else{
		opp_color = 'B';
	}
	/** Reset the moves array **/
	for (int i = 0; i < 64; i ++){
		moves[i] = 0;
	}
	/** First 4 turns we are forced to only place in the center **/
	if (turn < 5){
		for (int i = 3; i < 5; i ++){
			for (int j = 3; j < 5; j ++){
				if (current_board[i][j] == '*'){
					moves[(i*8) + j] = 1;
				}
			}
		}
	}else{
		/** Go through the entire board and find empty spots **/
		for (int y = 0; y < 8; y ++){
			for (int x = 0; x < 8; x ++){
				/** Found an empty spot **/
				if(current_board[y][x] == '*'){
					marked = false;
					/** Check if there are opponents color around it 
						Check in 3 by 3 square around the empty spot**/
					for (int y_diff = -1; y_diff < 2; y_diff ++){
						for (int x_diff = -1; x_diff < 2; x_diff ++){
							if(marked == true){
								break;
							}
							/** If we are checking outside the board we we will pass on it
								OR if we are checking 0,0 (current square)**/
							if ((x_diff == 0 && y_diff == 0) || x + x_diff < 0 || x + x_diff >= 8 || y + y_diff < 0 || y + y_diff >= 8){
								continue;
							}
							/** If we find a opposite neighbour **/
							if(current_board[y + y_diff][x + x_diff] == opp_color){
								/** Continue in the x_diff and y_diff until we hit a edge, blank or our own color **/
								store_x = x + x_diff;
								store_y = y + y_diff;
								for (int k = 0; k < 10; k++){
									store_x = store_x + x_diff;
									store_y = store_y + y_diff;
									/** If we go out of the board we stop **/
									if(store_x < 0 || store_x >= 8 ||store_y < 0 || store_y >= 8){
										break;
									}

									/** We Ran into a blank, thus not valid **/
									if(current_board[store_y][store_x] == '*'){
										break;
									}

									/** We run into our own color, thus store this as a move and check others**/
									if(current_board[store_y][store_x] == color){
										moves[(y*8) + x] = 1;
										marked = true;
										break;
									}
								}
							}

						}
					}
				}
			}
		}
	}
	return moves;
}

//Plays a valid move onto the boad and flips all pieces in between.
void Reversi::input_move(char current_board[8][8], char color, int turn, int x_move, int y_move){
	char opp_color;
	int store_x;
	int store_y;
	if (color == 'B'){
		opp_color = 'W';
	}else{
		opp_color = 'B';
	}
	// There should not be any flips while turn is less than 5
	if (turn < 5){
		current_board[y_move][x_move] = color;
	}else{
		// We now check to see which tile we flip.
		current_board[y_move][x_move] = color;
		// Check all its neighbours for an oposite color
		// We check in a 3 by 3 square around the input
		for (int y_diff = -1; y_diff < 2; y_diff ++){
			for (int x_diff = -1; x_diff < 2; x_diff ++){
				/** If we are checking outside the board we we will pass on it
					OR if we are checking 0,0 (current square)**/
				if ((x_diff == 0 && y_diff == 0) || x_move + x_diff < 0 || x_move + x_diff >= 8 || y_move + y_diff < 0 || y_move + y_diff >= 8){
					continue;
				}
				/** If we find a opposite neighbour **/
				if(current_board[y_move + y_diff][x_move + x_diff] == opp_color){
					/** Continue in the x_diff and y_diff until we hit a edge, blank or our own color **/
					store_x = x_move + x_diff;
					store_y = y_move + y_diff;
					for (int k = 0; k < 10; k++){
						store_x = store_x + x_diff;
						store_y = store_y + y_diff;
						/** If we go out of the board we stop, no flipping **/
						if(store_x < 0 || store_x >= 8 ||store_y < 0 || store_y >= 8){
							break;
						}

						/** We Ran into a blank, thus no flipping **/
						if(current_board[store_y][store_x] == '*'){
							break;
						}

						/** We run into our own color, thus we back track and flip all the colors**/
						if(current_board[store_y][store_x] == color){
							// cout << "Match" << endl;
							while ((store_x != x_move) || (store_y != y_move)){
								// cout << "Backtracking" << endl;
								store_x = store_x - x_diff;
								store_y = store_y - y_diff;
								current_board[store_y][store_x] = color;
							}
							break;
						}
					}
				}

			}
		}
	}
	
}

//Gets the scores for the black piece
int Reversi::get_black_score(char current_board[8][8]){
	int score = 0;
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (current_board[i][j] == 'B'){
				score = score + 1;
			}
		}
	}
	return score;
}

//Gets the scores for the white piece
int Reversi::get_white_score(char current_board[8][8]){
	int score = 0;
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (current_board[i][j] == 'W'){
				score = score + 1;
			}
		}
	}
	return score;
}
