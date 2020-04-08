#ifndef BOARD_H
#define BOARD_H
// player 1 info window
#define PLAYER1_INFO_WINDOW_X 2
#define PLAYER1_INFO_WINDOW_Y 0
#define PLAYER1_INFO_WINDOW_HEIGHT 1
#define PLAYER1_INFO_WINDOW_WIDTH 35
// player 1 card window
#define PLAYER1_WINDOW_X 2
#define PLAYER1_WINDOW_Y 1
#define PLAYER1_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
#define PLAYER1_WINDOW_HEIGHT 7
// the play window
#define PLAY_WINDOW_X 2
#define PLAY_WINDOW_Y PLAYER1_WINDOW_Y+PLAYER1_WINDOW_HEIGHT
#define PLAY_WINDOW_WIDTH PLAYER1_WINDOW_WIDTH
#define PLAY_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
#define PLAY_WINDOW_PLAYER2_X 1
#define PLAY_WINDOW_PLAYER1_X 10
// player 2 info window
#define PLAYER2_INFO_WINDOW_X PLAYER1_INFO_WINDOW_X
#define PLAYER2_INFO_WINDOW_Y PLAY_WINDOW_Y+PLAY_WINDOW_HEIGHT
#define PLAYER2_INFO_WINDOW_HEIGHT PLAYER1_INFO_WINDOW_HEIGHT
#define PLAYER2_INFO_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
// player 2 card window
#define PLAYER2_WINDOW_X PLAYER1_WINDOW_X
#define PLAYER2_WINDOW_Y PLAYER2_INFO_WINDOW_Y+PLAYER2_INFO_WINDOW_HEIGHT
#define PLAYER2_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
#define PLAYER2_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
// starter card window
#define STARTER_WINDOW_X PLAYER1_WINDOW_WIDTH+PLAYER1_WINDOW_X+2
#define STARTER_WINDOW_Y PLAYER1_WINDOW_Y
#define STARTER_WINDOW_WIDTH 10
#define STARTER_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
// log window
#define LOG_WINDOW_X STARTER_WINDOW_X
#define LOG_WINDOW_Y PLAY_WINDOW_Y
#define LOG_WINDOW_WIDTH 35
#define LOG_WINDOW_HEIGHT 13
// score area window
#define SCORE_WINDOW_X STARTER_WINDOW_X+STARTER_WINDOW_WIDTH
#define SCORE_WINDOW_Y STARTER_WINDOW_Y
#define SCORE_WINDOW_WIDTH 15
#define SCORE_WINDOW_HEIGHT STARTER_WINDOW_HEIGHT

#define WINNING_SCORE_LONG 121
#define WINNING_SCORE_SHORT 61 
#define MAX_ROUND_SCORE 31
#define PLAYER1 0
#define PLAYER2 1

#define CRIB "CRIB"

#include <string>
#include <vector>
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"

class Board {
	protected: 
		// players
		Player * player1_;
		Player * player2_;
		std::vector<Player*> players_;
		std::vector<Card> playCards_;	// the cards in the "play" pile
		Card starter_;	// the starter card
		Deck deck_;	// the deck used
		unsigned short int dealer_;	// keeps track of who is "dealing"
		unsigned int gameNumber_;	// the game which you are on
		std::vector<Card> crib_;	// holds the crib cards
		int cur_x_, cur_y_;	// cursor position
		WINDOW * player1Window_; // window for the player 1's current cards
		WINDOW * player2Window_; // window for the player 2's current cards
		WINDOW * playWindow_;	// window for the "play"
		WINDOW * scoreWindow_;	// window keeping track of the score
		WINDOW * starterWindow_;	// window for the starter card
		WINDOW * player1InfoWindow_;	// window to update player 1 what to do
		WINDOW * player2InfoWindow_;	// window to show what player 2 is doing
		WINDOW * helpWindow_;	// window with what each key does
		WINDOW * logWindow_;	// window displays logs of what is going on in the game 
	public:
		Board(Player * p1, Player * p2);
		virtual void initGame();
		virtual void startGameRound();
		virtual void dealRound() = 0;
		virtual void initialDiscard() = 0;	
		virtual void cutStarterCard() = 0;
		void startPlay();
		virtual int checkWin();
		virtual int playRound(unsigned short int turn) = 0;
		void checkPlayCardsScoring(unsigned short int turn, int count);
		virtual void countHandScores();
		int getHandScore(std::vector<Card> hand, std::string playerName);
		void displayHand(WINDOW * win, std::vector<Card> cards);
		void combinationUtil(std::vector<std::vector<Card>> &combinations, std::vector<Card> cards, 
				Card data[], int start, int end, int index, int r);
		void getCombinations(std::vector<Card> cards);
		virtual void updateLog(std::string s);
		
};

#endif /* BOARD_H */
