#include <iostream>
#include <cstring>
#include "reversi.h"
#include <stdlib.h>
#include <time.h>
#include "ai.h"
using namespace std;


int main(){
	srand(time(0));

	string player_continue;
	string player_move;
	string temp_string = "";
	string decision = " ";

	static string move_counter [32];

	char unparsed_move[3];
	char opp_color;
	char temp_char;
	char player_color[20];
	char possible_color[2] = {'B', 'W'};

	bool keep_playing = true;
	bool valid = false;
	bool not_over = true;

	int turn;
	int random;
	int counter;
	int mod_value;
	int x_move;
	int y_move;
	int black_score;
	int white_score;
	int gameNum = 0;

	int heuristic_win = 0;
	int pureMCT_win = 0;
	int draw = 0;

	int no_moves;

	for (int i = 0; i < 32; i ++){
		move_counter[i] = " ";
	}

	//Play 10 games all random goes first
	while (gameNum < 5){
		/** Reset all parameters used for a new game **/
		not_over = true;
		turn = 1;
		Reversi reversi;
		AI ai;
		no_moves = 0;

		random = rand() % 2;
		player_color[0] = possible_color[random];

		if (player_color[0] == 'B'){
			mod_value = 1;
			opp_color = 'W';
			cout << "\n\n\n\nHeuristic is black   PureMCT is white" << endl;
		}else{
			mod_value = 0;
			opp_color = 'B';
			cout << "\n\n\n\nHeuristic is white   PureMCT is black" << endl;
		}


		/** While the game is still in progress **/
		while (not_over){
			// New turn so we display the board and the current score
			cout << "	     Turn: " << turn << endl;
			reversi.display();
			black_score = reversi.get_black_score(reversi.board);
			white_score = reversi.get_white_score(reversi.board);
			cout << "   Black: " << black_score << "		White: " << white_score << endl;

			counter = 0;

			if (turn >= 5){
				if (black_score == 0 || white_score == 0){
					not_over = false;
					continue;
				}
			}
			if (no_moves >= 2){
				not_over = false;
				continue;
			}

			if (turn > 80){
				not_over = false;
				continue;
			}

			/** Players Turn **/
			if (turn % 2 == mod_value){
				// Heuristics's Turn
				int* moves = reversi.valid_move(reversi.board, player_color[0], turn);
				for (int i = 0; i < 8; i ++){
					for (int j = 0; j < 8; j ++){
						if (moves[(i*8) + j] == 1){
							temp_string = "";
							temp_char = char(65 + j);
							temp_string = temp_string + temp_char + to_string(8 - i);
							move_counter[counter] = temp_string;
							counter = counter + 1;
						}
					}
				}
				// If there are no moves we pass
				if (counter == 0){
					no_moves = no_moves + 1;
					turn = turn + 1;
					continue;
				}else{
					no_moves = 0;
				}
				cout << "\nHeuristic MCT valid moves are as follows: ";
				for(int i = 0; i < counter; i ++){
					cout << move_counter[i] << " ";
				}
				cout << "\n";
				decision = ai.heuristicP_MCT(reversi.board, 100000, move_counter, counter, player_color[0], turn);
				cout << "Heuristic MCT has choosen " << decision << endl;
				temp_char = decision[0];
				x_move = int(temp_char) - 65;
				temp_char = decision[1];
				for (int i = 0; i < 8; i ++){
					if (7 - i == 56 - int(temp_char)){
						y_move = 7 - i;
					}
				}
				/** Make move onto the board **/
				reversi.input_move(reversi.board, player_color[0], turn, x_move, y_move);
				turn = turn + 1;
			}else{
				// Pure's Turn
				int* moves = reversi.valid_move(reversi.board, opp_color, turn);
				for (int i = 0; i < 8; i ++){
					for (int j = 0; j < 8; j ++){
						if (moves[(i*8) + j] == 1){
							temp_string = "";
							temp_char = char(65 + j);
							temp_string = temp_string + temp_char + to_string(8 - i);
							move_counter[counter] = temp_string;
							counter = counter + 1;
						}
					}
				}
				// If there are no moves we pass
				if (counter == 0){
					no_moves = no_moves + 1;
					turn = turn + 1;
					continue;
				}else{
					no_moves = 0;
				}
				cout << "\nPure MCT valid moves are as follows: ";
				for(int i = 0; i < counter; i ++){
					cout << move_counter[i] << " ";
				}
				cout << "\n";
				decision = ai.pure_MCT(reversi.board, 100000, move_counter, counter, opp_color, turn);
				cout << "Pure MCT has choosen " << decision << endl;
				temp_char = decision[0];
				x_move = int(temp_char) - 65;
				temp_char = decision[1];
				for (int i = 0; i < 8; i ++){
					if (7 - i == 56 - int(temp_char)){
						y_move = 7 - i;
					}
				}
				/** Make move onto the board **/
				reversi.input_move(reversi.board, opp_color, turn, x_move, y_move);
				turn = turn + 1;
			}
		}

		cout << "\n\n\n\n\nGame Over" << endl;
		cout << "Final Game State..." << endl;
		cout << "	     Turn: " << turn << endl;
		reversi.display();
		black_score = reversi.get_black_score(reversi.board);
		white_score = reversi.get_white_score(reversi.board);
		cout << "   Black: " << black_score << "		White: " << white_score << endl;

		if ((player_color[0] == 'B' && black_score > white_score) || (player_color[0] == 'W' && white_score > black_score)){
			heuristic_win = heuristic_win + 1;
		}else if (white_score == black_score){
			draw = draw + 1;
		}else{
			pureMCT_win = pureMCT_win + 1;
		}

		gameNum = gameNum + 1;
	}

	cout << "Heuristic Wins: " << heuristic_win << "  Pure MCT Wins: " << pureMCT_win << "  Draws: " << draw << endl;
 	return 0;
}