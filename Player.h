#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <tuple>
#include <ncursesw/curses.h>
#include "Card.h"

class Player {
	protected:
		std::string name_;
		std::vector<Card> hand_;
		std::vector<Card> playHand_;
		int position_;
		WINDOW * playerWindow_; // window where player can interact with hand
	public:
		Player(std::string name);
		std::string name() { return name_; }
		int position() { return position_; }
		void setPosition(int pos);
		std::vector<Card> hand() { return hand_; }
		std::vector<Card> playHand() { return playHand_; }
		void setPlayHand(std::vector<Card> cards);
		bool isPlayHandEmpty();
		void emptyHand();
		void emptyPlayHand();
		int advancePosition(int spaces);
		Card getCardInHandAt(int index);
		Card getCardInPlayHandAt(int index);
		void addToHand(Card c);
		void removeFromHand(Card c);
		void removeFromPlayHand(Card c);
		void printHand();
		void printPlayHand();

		void setPlayerWindow(WINDOW * win);
		WINDOW* handWindow() { return playerWindow_; }

		virtual std::vector<Card> discardCards() = 0;
		virtual Card playCard(int count, int max) = 0;
		void displayHand(std::vector<Card> hand);

};

#endif /* PLAYER_H */
