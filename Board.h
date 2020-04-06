#ifndef BOARD_H
#define BOARD_H

#define INFO_WINDOW_X 2
#define INFO_WINDOW_Y 0
#define INFO_WINDOW_HEIGHT 1
#define INFO_WINDOW_WIDTH 35
#define PLAYER_WINDOW_X 2
#define PLAYER_WINDOW_Y 1
#define PLAYER_WINDOW_WIDTH INFO_WINDOW_WIDTH
#define PLAYER_WINDOW_HEIGHT 7
#define INFO_WINDOW2_X INFO_WINDOW_X
#define INFO_WINDOW2_Y PLAYER_WINDOW_Y+PLAYER_WINDOW_HEIGHT
#define INFO_WINDOW2_HEIGHT INFO_WINDOW_HEIGHT
#define INFO_WINDOW2_WIDTH INFO_WINDOW_WIDTH
#define PLAY_WINDOW_X 2
#define PLAY_WINDOW_Y PLAYER_WINDOW_Y+PLAYER_WINDOW_HEIGHT+1
#define PLAY_WINDOW_WIDTH PLAYER_WINDOW_WIDTH
#define PLAY_WINDOW_HEIGHT PLAYER_WINDOW_HEIGHT
#define PLAY_WINDOW_COMPUTER_X 1
#define PLAY_WINDOW_PLAYER_X 10
#define STARTER_WINDOW_X PLAYER_WINDOW_WIDTH+PLAYER_WINDOW_X+2
#define STARTER_WINDOW_Y PLAYER_WINDOW_Y
#define STARTER_WINDOW_WIDTH 10
#define STARTER_WINDOW_HEIGHT PLAYER_WINDOW_HEIGHT
#define LOG_WINDOW_X STARTER_WINDOW_X
#define LOG_WINDOW_Y PLAY_WINDOW_Y
#define LOG_WINDOW_WIDTH 35
#define LOG_WINDOW_HEIGHT 13
#define SCORE_WINDOW_X STARTER_WINDOW_X+STARTER_WINDOW_WIDTH
#define SCORE_WINDOW_Y STARTER_WINDOW_Y
#define SCORE_WINDOW_WIDTH 15
#define SCORE_WINDOW_HEIGHT STARTER_WINDOW_HEIGHT

#define WINNING_SCORE_LONG 121
#define WINNING_SCORE_SHORT 61 
#define MAX_ROUND_SCORE 31
#define CARD_WIDTH 5
#define CARD_HEIGHT 5
#define PLAYER 0
#define COMPUTER 1
#define CURSOR '^'

#include <string>
#include <vector>
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"

class Board {
	private: 
		// players
		HumanPlayer player_ = HumanPlayer("Human");
		ComputerPlayer computer_ = ComputerPlayer("Computer");
		Player *players_[2] = { &player_, &computer_};
		std::vector<Card> playCards_;	// the cards in the "play" pile
		Card starter_;	// the starter card
		Deck deck_;	// the deck used
		unsigned short int dealer_;	// keeps track of who is "dealing"
		unsigned int gameNumber_;	// the game which you are on
		std::vector<Card> crib_;	// holds the crib cards
		int cur_x_, cur_y_;	// cursor position
		WINDOW * playerWindow_; // window for the player's current cards
		WINDOW * playWindow_;	// window for the "play"
		WINDOW * scoreWindow_;	// window keeping track of the score
		WINDOW * starterWindow_;	// window for the starter card
		WINDOW * infoWindow_;	// window to update player what to do
		WINDOW * infoWindow2_;	// window to show what play window is doing
		WINDOW * helpWindow_;	// window with what each key does
		WINDOW * logWindow_;	// window displays logs of what is going on in the game 
	public:
		Board();
		void startGame();
		void startRound();
		void dealRound();
		void initialDiscard();	
		void addToCrib(Card c);
		void emptyCrib();		
		void cutStarterCard();
		void displayHelpWindow();
		void startPlay();
		void checkWin();
		int playRound(unsigned short int turn);
		void checkPlayCardsScoring(unsigned short int turn, int count);
		void countHandScores();
		int getHandScore(std::vector<Card> hand, std::string playerName);
		void displayHand(WINDOW * win, std::vector<Card> cards);
		void combinationUtil(std::vector<std::vector<Card>> &combinations, std::vector<Card> cards, 
				Card data[], int start, int end, int index, int r);
		void getCombinations(std::vector<Card> cards);
		void updateInfoWindow(std::string s);
		void updateInfoWindow2(std::string s);
		void updateScoreWindow();
		void updateLogWindow(std::string s);
};

#endif /* BOARD_H */
