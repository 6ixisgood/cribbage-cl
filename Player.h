#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <tuple>
#include "Card.h"

class Player {
	private:
		std::string name_;
		std::vector<Card> hand_;
		std::vector<Card> playHand_;
		unsigned int position_;
	public:
		Player(std::string name);
		std::string name() { return name_; }
		int position() { return position_; }
		std::vector<Card> hand() { return hand_; }
		std::vector<Card> playHand() { return playHand_; }
		void setPlayHand(std::vector<Card> cards);
		bool isPlayHandEmpty();
		int advancePosition(int spaces);
		Card getCardInHandAt(int index);
		Card getCardInPlayHandAt(int index);
		void addToHand(Card c);
		void removeFromHand(Card c);
		void removeFromPlayHand(Card c);
		void printHand();
		void printPlayHand();
		virtual std::tuple<Card, Card> discardToCrib() = 0;
		virtual Card playCard(int count) = 0;

};

#endif /* PLAYER_H */
