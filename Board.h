#ifndef BOARD_H
#define BOARD_H

#define PLAYER 0
#define COMPUTER 1

#include <string>
#include <vector>
#include "Player.h"
#include "Deck.h"
#include "Card.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"

class Board {
	private: 
		HumanPlayer player_ = HumanPlayer("Human");
		ComputerPlayer computer_ = ComputerPlayer("Computer");
		Player *players_[2] = { &player_, &computer_};
		std::vector<Card> playCards_;
		Card starter_;
		Deck deck_;
		unsigned short int dealer_;
		
		unsigned int roundNumber_;
		std::vector<Card> crib_;
	public:
		Board();
		void startGame();
		void startRound();
		void dealRound();
		void initialDiscard();	
		void cutDeck();
		void startPlay();
		int playRound(unsigned short int turn);
		void checkPlayCardsScoring(unsigned short int turn, int count);
		void addToCrib(Card c);
		void emptyCrib();		
		void countHandScores();
		int getHandScore(std::vector<Card> hand, std::string playerName);
		void combinationUtil(std::vector<std::vector<Card>> &combinations, std::vector<Card> cards, 
				Card data[], int start, int end, int index, int r);
		void getCombinations(std::vector<Card> cards);
		


};

#endif /* BOARD_H */
