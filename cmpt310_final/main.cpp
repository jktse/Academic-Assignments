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
	char ai_choice[20];

	bool keep_playing = true;
	bool valid = false;
	bool not_over = true;
	bool play_pure = true;

	int turn;
	int counter;
	int mod_value;
	int player_x;
	int player_y;
	int black_score;
	int white_score;
	int no_moves;

	for (int i = 0; i < 32; i ++){
		move_counter[i] = " ";
	}

	cout << "Welcome to Reversi created by Jacky K. Tse" << endl;
	cout << "Please choose the color you wish to start with..." << endl;
	cout << "Note: Black piece always goes first" << endl;

	while (keep_playing){
		/** Reset all parameters used for a new game **/
		turn = 1;
		not_over = true;
		Reversi reversi;
		AI ai;
		no_moves = 0;

		cout << "Type 'H' or h to play against HPMCT otherwise we default to PMCT: ";
		cin >> ai_choice;

		if((ai_choice[0] == 'H' || ai_choice[0] == 'h') && strlen(ai_choice) == 1){
			cout << "Playing against HPMCT... " << endl;
			play_pure = false;
		}else{
			play_pure = true;
			cout << "Playing against PMCT... " << endl;
		}

		/** Let the play choose their color **/
		cout << "Please type 'B' for black piece or 'W' for white piece: ";
		cin >> player_color;
		while ((player_color[0] != 'W' && player_color[0] != 'B') || strlen(player_color) > 1){
			cout << "Sorry, that was an incorrect input..." << endl;
			cout << "Please type 'B' for black piece or 'W' for white piece: ";
			cin >> player_color;
		}

		if (player_color[0] == 'B'){
			mod_value = 1;
			opp_color = 'W';
		}else{
			mod_value = 0;
			opp_color = 'B';
		}


		/** While the game is still in progress **/
		while (not_over){
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

			/** Players Turn **/
			if (turn % 2 == mod_value){
				/** Get the valid turns for the player and display it to the user**/
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
				// New turn so we display the board and the current score
				cout << "\n\n	     Turn: " << turn << endl;
				reversi.hint_display(moves);
				black_score = reversi.get_black_score(reversi.board);
				white_score = reversi.get_white_score(reversi.board);
				cout << "   Black: " << black_score << "		White: " << white_score << endl;

				// If there are no moves we pass
				if (counter == 0){
					cout << "I have no moves this turn" << endl;
					no_moves = no_moves + 1;
					turn = turn + 1;
					continue;
				}else{
					no_moves = 0;
				}

				/** Show the player their valid moves **/
				cout << "Your valid moves are as follows: ";
				for(int i = 0; i < counter; i ++){
					cout << move_counter[i] << " ";
				}
				cout << "\n";
				/** Continues until the player enters a valid move **/
				while(!valid){
					cout << "Please enter a valid move: ";
					cin >> player_move;
					/** Validate the move such that it is a valid one. **/
					for(int i = 0; i < counter; i ++){
						if(move_counter[i] == player_move){
							unparsed_move[0] = player_move[0];
							unparsed_move[1] = player_move[1];
							valid = true;
						}
					}
					if(!valid){
						cout << "You entered an incorrect valid move." << endl;
						cout << "Note valid moves are in the form 'A8'" << endl;
						cout << "Your valid moves are as follows: ";
						for(int i = 0; i < counter; i ++){
							cout << move_counter[i] << " ";
						}
						cout << "\n";
					}
				}
				valid = false;
				/** Parse the input into X and Y values. **/
				temp_char = unparsed_move[0];
				player_x = int(temp_char) - 65;
				temp_char = unparsed_move[1];
				for (int i = 0; i < 8; i ++){
					if (7 - i == 56 - int(temp_char)){
						player_y = 7 - i;
					}
				}
				//cout << player_x << "," << player_y << endl; 
				/** Make move onto the board **/

				reversi.input_move(reversi.board, player_color[0], turn, player_x, player_y);
				turn = turn + 1;
			}else{
				// Ai's Turn
				// For the player to see the Board after their move
				cout << "\n\n	     Turn: " << turn << endl;
				reversi.display();
				black_score = reversi.get_black_score(reversi.board);
				white_score = reversi.get_white_score(reversi.board);
				cout << "   Black: " << black_score << "		White: " << white_score << endl;
				cout << "\n\nAi is thinking" << endl;
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
				if(play_pure){
					decision = ai.pure_MCT(reversi.board, 100000, move_counter, counter, opp_color, turn);
				}else{
					decision = ai.heuristicP_MCT(reversi.board, 100000, move_counter, counter, opp_color, turn);
				}
				cout << "AI has choosen " << decision << endl;
				temp_char = decision[0];
				player_x = int(temp_char) - 65;
				temp_char = decision[1];
				for (int i = 0; i < 8; i ++){
					if (7 - i == 56 - int(temp_char)){
						player_y = 7 - i;
					}
				}
				/** Make move onto the board **/
				reversi.input_move(reversi.board, opp_color, turn, player_x, player_y);
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
			cout << "Player Color: " << player_color[0] << endl;
			cout << "Congratulations You Won!!" << endl;
		}else if (white_score == black_score){
			cout << "Player Color: " << player_color[0] << endl;
			cout << "It was a draw in the end." << endl;
		}else{
			cout << "Player Color: " << player_color[0] << endl;
			cout << "Unfortunately you lost.." << endl;
		}

		cout << "\n\n\nWould you like to play again? " << endl;
		cout << "Type 'Y' or 'y' to continue otherwise anything other input will terminate the program: ";
		cin >> player_continue;

		if (player_continue != "Y" && player_continue != "y"){
			keep_playing = false;
		}
	}

	return 0;
}