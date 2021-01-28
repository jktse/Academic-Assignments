#include <iostream>
#include <string>
#include <array>
#include <chrono>
#include <math.h>
#include "ai.h"
#include "reversi.h"

struct heuristic_return {
	char state;
	int ai_mobility;
	int player_mobility;
};

char random_playout(char current_board[8][8], char ai_color, int turn, bool is_ai){
	bool state = false;

	string temp_string;

	char temp_char;
	char ret_state;
	char player_color;

	int white_score;
	int black_score;
	int y_move;
	int x_move;
	int counter = 0;

	static string move_counter [32];


	if (ai_color == 'B'){
		player_color = 'W';
	}else{
		player_color = 'B';
	}

	Reversi reversi(current_board);
	/***
	cout << "	     Turn: " << turn << endl;
	reversi.display();
	black_score = reversi.get_black_score(reversi.board);
	white_score = reversi.get_white_score(reversi.board);
	cout << "   Black: " << black_score << "		White: " << white_score << endl;
	***/
	int* ai_moves = reversi.valid_move(reversi.board, ai_color, turn);
	int* player_moves = reversi.valid_move(reversi.board, player_color, turn);
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (ai_moves[(i*8) + j] == 1){
				state = true;
			}
			if(player_moves[(i*8) + j] == 1){
				state = true;
			}
		}
	}
	if (state == false){
		//Game is over
		black_score = reversi.get_black_score(reversi.board);
		white_score = reversi.get_white_score(reversi.board);
		if ((ai_color == 'B' && black_score > white_score) || (ai_color == 'W' && white_score > black_score)){
			return 'W';
		}else if (white_score == black_score){
			return 'D';
		}else{
			return 'L';
		}
	}

	// Simulating the AI's turn
	if (is_ai){
		// Ai's Turn
		string decision;
		int random;
		int* ai_moves = reversi.valid_move(reversi.board, ai_color, turn);
		for (int i = 0; i < 8; i ++){
			for (int j = 0; j < 8; j ++){
				if (ai_moves[(i*8) + j] == 1){
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
			turn = turn + 1;
			ret_state = random_playout(reversi.board, ai_color, turn + 1, false);
			return ret_state;
		}else{
			random = rand() % counter;
			decision = move_counter[random];
			temp_char = decision[0];
			x_move = int(temp_char) - 65;
			temp_char = decision[1];
			for (int i = 0; i < 8; i ++){
				if (7 - i == 56 - int(temp_char)){
					y_move = 7 - i;
				}
			}
			reversi.input_move(reversi.board, ai_color, turn, x_move, y_move);
			ret_state = random_playout(reversi.board, ai_color, turn + 1, false);
			return ret_state;
		}
	}else{
		// Simulating the Player's Turn
		string decision;
		int random;
		int* player_moves = reversi.valid_move(reversi.board, player_color, turn);
		for (int i = 0; i < 8; i ++){
			for (int j = 0; j < 8; j ++){
				if (player_moves[(i*8) + j] == 1){
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
			turn = turn + 1;
			ret_state = random_playout(reversi.board, ai_color, turn + 1, true);
			return ret_state;
		}else{
			random = rand() % counter;
			decision = move_counter[random];
			temp_char = decision[0];
			x_move = int(temp_char) - 65;
			temp_char = decision[1];
			for (int i = 0; i < 8; i ++){
				if (7 - i == 56 - int(temp_char)){
					y_move = 7 - i;
				}
			}
			reversi.input_move(reversi.board, player_color, turn, x_move, y_move);
			ret_state = random_playout(reversi.board, ai_color, turn + 1, true);
			return ret_state;
		}
	}

}

// Main decision algorithm what will start random playouts to get win, lose, draw information.
string AI::pure_MCT(char current_board[8][8], int playout, string valid_moves[32], int counter, char color, int turn){
	int win;
	int draw;
	int lose;
	int x_move;
	int y_move;
	int max_score = 0;
	int current_score;
	int plays;

	char ret_val;
	char temp_char;

	double max_seconds;

	std::chrono::duration<double> elapsed_seconds;

	string decision;
	string choice = "";

	// Go through each possible moves
	// We give only a 4.8 second turn limit to account for other funciton times.
	max_seconds = 4.8/counter;
	// cout << "Max seconds: " << max_seconds << endl;
	//cout << "Turn: " << turn << endl;
	for (int i = 0; i < counter; i ++){
		plays = 0;
		win = 0;
		draw = 0;
		lose = 0;
		Reversi reversi(current_board);
		//Get the move
		decision = valid_moves[i];
		temp_char = decision[0];
		x_move = int(temp_char) - 65;
		temp_char = decision[1];
		for (int i = 0; i < 8; i ++){
			if (7 - i == 56 - int(temp_char)){
				y_move = 7 - i;
			}
		}
		reversi.input_move(reversi.board, color, turn, x_move, y_move);
		// For now based on number of playouts
		auto start = std::chrono::steady_clock::now();
		elapsed_seconds = start-start;
		while (plays < playout && elapsed_seconds.count() < max_seconds){
			ret_val = random_playout(reversi.board, color, turn + 1, false);
			//cout << "Return Value: " << ret_val << endl;
			if (ret_val == 'W'){
				win = win + 1;
			}else if (ret_val == 'L'){
				lose = lose + 1;
			}else{
				draw = draw + 1;
			}
			auto end = std::chrono::steady_clock::now();
			elapsed_seconds = end-start;
			plays = plays + 1;
		}
		//cout << plays << " playouts took " << elapsed_seconds.count() << " seconds" << endl;
	
		current_score = (win + draw);
		//cout << "Win: " << win << " Lose: " << lose << " Draw: " << draw << " Final Total: " << current_score << endl;
		if (current_score > max_score){
			//cout << "Bigger" << endl;
			choice = valid_moves[i];
			max_score = current_score;
		}
	}
	//cout << "P made a decision" << endl;
	return choice;
}

// An improved random playout that discourages the placement that will give corners away.
heuristic_return Hrandom_playout(char current_board[8][8], char ai_color, int turn, bool is_ai, int ai_mobility, int player_mobility){
	bool state = false;

	string temp_string;

	char temp_char;
	char player_color;

	heuristic_return ret_state;

	int white_score;
	int black_score;
	int y_move;
	int x_move;
	int num_moves = 0;
	int counter = 0;

	

	static string move_counter [32];

	if (ai_color == 'B'){
		player_color = 'W';
	}else{
		player_color = 'B';
	}

	Reversi reversi(current_board);
	/***
	cout << "	     Turn: " << turn << endl;
	reversi.display();
	black_score = reversi.get_black_score(reversi.board);
	white_score = reversi.get_white_score(reversi.board);
	cout << "   Black: " << black_score << "		White: " << white_score << endl;
	***/
	int* ai_moves = reversi.valid_move(reversi.board, ai_color, turn);
	int* player_moves = reversi.valid_move(reversi.board, player_color, turn);
	for (int i = 0; i < 8; i ++){
		for (int j = 0; j < 8; j ++){
			if (ai_moves[(i*8) + j] == 1){
				state = true;
			}
			if(player_moves[(i*8) + j] == 1){
				state = true;
			}
		}
	}
	if (state == false){
		//Game is over
		black_score = reversi.get_black_score(reversi.board);
		white_score = reversi.get_white_score(reversi.board);
		if ((ai_color == 'B' && black_score > white_score) || (ai_color == 'W' && white_score > black_score)){
			ret_state.state = 'W';
			ret_state.ai_mobility = ai_mobility;
			ret_state.player_mobility = player_mobility;
			return ret_state;
		}else if (white_score == black_score){
			ret_state.state = 'D';
			ret_state.ai_mobility = ai_mobility;
			ret_state.player_mobility = player_mobility;
			return ret_state;
		}else{
			ret_state.state = 'L';
			ret_state.ai_mobility = ai_mobility;
			ret_state.player_mobility = player_mobility;
			return ret_state;
		}
	}

	// Simulating the AI's turn
	if (is_ai){
		// Ai's Turn
		string decision;
		int random;
		int* ai_moves = reversi.valid_move(reversi.board, ai_color, turn);
		for (int i = 0; i < 8; i ++){
			for (int j = 0; j < 8; j ++){
				if (ai_moves[(i*8) + j] == 1){
					num_moves = num_moves + 1;
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
			turn = turn + 1;
			ret_state = Hrandom_playout(reversi.board, ai_color, turn + 1, false, ai_mobility, player_mobility);
			return ret_state;
		}else{
			random = rand() % counter;
			decision = move_counter[random];
			if (decision == "B2" || decision == "G2" || decision == "B7" || decision == "G7"){
				random = rand() % counter;
				decision = move_counter[random];
				if (decision == "B2" || decision == "G2" || decision == "B7" || decision == "G7"){
					random = rand() % counter;
					decision = move_counter[random];
				}
			}
			temp_char = decision[0];
			x_move = int(temp_char) - 65;
			temp_char = decision[1];
			for (int i = 0; i < 8; i ++){
				if (7 - i == 56 - int(temp_char)){
					y_move = 7 - i;
				}
			}
			reversi.input_move(reversi.board, ai_color, turn, x_move, y_move);
			ret_state = Hrandom_playout(reversi.board, ai_color, turn + 1, false, ai_mobility + num_moves, player_mobility);
			return ret_state;
		}
	}else{
		// Simulating the Player's Turn
		string decision;
		int random;
		int* player_moves = reversi.valid_move(reversi.board, player_color, turn);
		for (int i = 0; i < 8; i ++){
			for (int j = 0; j < 8; j ++){
				if (player_moves[(i*8) + j] == 1){
					num_moves = num_moves + 1;
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
			turn = turn + 1;
			ret_state = Hrandom_playout(reversi.board, ai_color, turn + 1, true, ai_mobility, player_mobility);
			return ret_state;
		}else{
			random = rand() % counter;
			decision = move_counter[random];
			if (decision == "B2" || decision == "G2" || decision == "B7" || decision == "G7"){
				random = rand() % counter;
				decision = move_counter[random];
				if (decision == "B2" || decision == "G2" || decision == "B7" || decision == "G7"){
					random = rand() % counter;
					decision = move_counter[random];
				}
			}
			temp_char = decision[0];
			x_move = int(temp_char) - 65;
			temp_char = decision[1];
			for (int i = 0; i < 8; i ++){
				if (7 - i == 56 - int(temp_char)){
					y_move = 7 - i;
				}
			}
			reversi.input_move(reversi.board, player_color, turn, x_move, y_move);
			ret_state = Hrandom_playout(reversi.board, ai_color, turn + 1, true, ai_mobility, player_mobility + num_moves);
			return ret_state;
		}
	}
}

// Pure Montecarlo Tree Search with additional heuristics to choose from.
// Additional information used is how many open spaces we have on our next move, and prevent bad moves from happening.
string AI::heuristicP_MCT(char current_board[8][8], int playout, string valid_moves[32], int counter, char color, int turn){
	int win;
	int draw;
	int lose;
	int x_move;
	int y_move;
	int num_opp;
	int num_ai;
	int new_num_ai;

	float max_score = -9999.0;
	float current_score;
	float win_draw;
	float innerScore;
	float greed;
	float position;
	float mobility;

	int plays;
	int num_available;
	int player_mobility;
	int ai_mobility;

	heuristic_return ret_val;

	char temp_char;

	double max_seconds;

	std::chrono::duration<double> elapsed_seconds;

	string decision;
	string choice = "";
	// Go through each possible moves
	max_seconds = 4.8/counter;
	// cout << "Max seconds: " << max_seconds << endl;
	//cout << "Turn: " << turn << endl;
	for (int i = 0; i < counter; i ++){
		plays = 0;
		win = 0;
		draw = 0;
		lose = 0;
		player_mobility = 0;
		ai_mobility = 0;
		// For now based on number of playouts
		Reversi reversi(current_board);
		if (color == 'B'){
			num_ai = reversi.get_black_score(reversi.board);
			num_opp = reversi.get_white_score(reversi.board);
		}else{
			num_ai = reversi.get_white_score(reversi.board);
			num_opp = reversi.get_black_score(reversi.board);
		}
		//Get the move
		decision = valid_moves[i];
		temp_char = decision[0];
		x_move = int(temp_char) - 65;
		temp_char = decision[1];
		for (int i = 0; i < 8; i ++){
			if (7 - i == 56 - int(temp_char)){
				y_move = 7 - i;
			}
		}
		reversi.input_move(reversi.board, color, turn, x_move, y_move);
		if (color == 'B'){
			new_num_ai = reversi.get_black_score(reversi.board);
		}else{
			new_num_ai = reversi.get_white_score(reversi.board);
		}
		auto start = std::chrono::steady_clock::now();
		elapsed_seconds = start-start;
		while (plays < playout && elapsed_seconds.count() < max_seconds){
			ret_val = Hrandom_playout(reversi.board, color, turn + 1, false, counter, 0);
			//cout << "Return Value: " << ret_val << endl;
			if (ret_val.state == 'W'){
				win = win + 1;
			}else if (ret_val.state == 'L'){
				lose = lose + 1;
			}else{
				draw = draw + 1;
			}
			ai_mobility = ai_mobility + ret_val.ai_mobility;
			player_mobility = player_mobility + ret_val.player_mobility;
			auto end = std::chrono::steady_clock::now();
			elapsed_seconds = end-start;
			plays = plays + 1;
		}
		//cout << plays << " playouts took " << elapsed_seconds.count() << " seconds" << endl;
		win_draw = (((float)win + (float)draw)/(float)plays);

		// Encourages plays that maximizes mobility early one, but fades away as the game progresses.
		if(turn < 20){
			if(ai_mobility > player_mobility){
				mobility = sqrt(((float)ai_mobility - (float)player_mobility)/((float)ai_mobility + (float)player_mobility));
			}else if (player_mobility > ai_mobility){
				mobility = -1*(sqrt(((float)player_mobility - (float)ai_mobility)/((float)ai_mobility + (float)player_mobility)));
			}else{
				mobility = 0;
			}
		}else{
			mobility = 0;
		}

		//Encourage Play in the center of the board
		innerScore = win_draw * (1/(pow((float)x_move - 3.5, 2) + pow((float)y_move - 3.5, 2)));

		//Encourages less flips early on in the game, despite a lower score, eventually we will be able to flip the board in our favour.
		if (turn < 5){
			greed = 0;
		}else{
			greed = win_draw * ((float)new_num_ai - (float)num_ai)/pow((float)num_ai+(float)num_opp,4.0);
		}

		position = 0;
		// If the play is a corner piece we want to alot of weight as that makes the board more "stable"
		if (decision == "A1" || decision == "H1" || decision == "A8" || decision == "H8"){
			//cout << "Has corner choice" << endl;
			position = win_draw * 1.5;
		}
		// Want to play in A-square a bit more than B-square (This is A-square)
		if (decision == "C1" || decision == "F1" || decision == "A3" || decision == "A6" || decision == "H3" || decision == "H6" || decision == "C8" || decision == "F8"){
			//cout << "Has A-square choice" << endl;
			current_score = current_score * 1.2;
		}
		// Want to play in A-square a bit more than B-square (This is B-square)
		if (decision == "D1" || decision == "E1" || decision == "A4" || decision == "A5" || decision == "H4" || decision == "H5" || decision == "D8" || decision == "E8"){
			//cout << "Has B-square choice" << endl;
			current_score = current_score * 1.1;
		}
		// Avoid playing in the C-Squares if possible.
		if (decision == "B1" || decision == "G1" || decision == "A2" || decision == "H2" || decision == "B8" || decision == "G8" || decision == "A7" || decision == "H7"){
			//cout << "Has c-sqares choice" << endl;
			position = win_draw * (-0.55);
		}
		// Want to avoid playing in the x-square for sure.
		if (decision == "B2" || decision == "G2" || decision == "B7" || decision == "G7"){
			//cout << "Has x-sqares choice" << endl;
			position = win_draw * (-100.0);
		}

		current_score = win_draw + position + mobility + innerScore - greed;

		// If 100% win is detected play that instead
		if (win == plays){
			current_score = 1000.0;
		}

		//cout << "\nDecision: " << decision << endl;
		//cout << "Win: " << win << " Lose: " << lose << " Draw: " << draw << " Win%: " << win_draw << endl;
		//cout << "Inner Score: " << innerScore << endl;
		//cout << "Greed: " << greed << endl;
		//cout << "Position: " << position << endl;
		//cout << "Mobility: " << mobility << endl;
		//cout << "Final Calculation: " << current_score << endl;
		if (current_score > max_score){
			//cout << "Bigger" << endl;
			choice = valid_moves[i];
			max_score = current_score;
		}
	}
	//cout << "H made a decision" << endl;
	return choice;
}
