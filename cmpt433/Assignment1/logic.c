/*******************
 * Name: Jacky Ken Tse
 * Date: January 27, 2021
 * Purpose: A file meant to keep memorize the current round and score
 * *****************/

static int totalScore = 0;
static int userScore = 0;

int LOGIC_getTotalScore(){
    return totalScore;
}

void LOGIC_incrementTotalScore(){
    totalScore++;
}

int LOGIC_getUserScore(){
    return userScore;
}

void LOGIC_incrementUserScore(){
    userScore++;
}