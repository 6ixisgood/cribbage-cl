#ifndef BOARD_H
#define BOARD_H

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
	public:
		Board(Player * p1, Player * p2);
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
		void combinationUtil(std::vector<std::vector<Card>> &combinations, std::vector<Card> cards, 
				Card data[], int start, int end, int index, int r);
		void getCombinations(std::vector<Card> cards);
		virtual void updateLog(std::string s);
		
};

#endif /* BOARD_H */
