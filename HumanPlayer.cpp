#include <iostream>
#include "HumanPlayer.h"

using namespace std;

HumanPlayer::HumanPlayer(string name) : Player(name) {}


tuple<Card, Card> HumanPlayer::discardToCrib() {
	cout << "Choose 2 cards to discard: ";
	int i1, i2;
	cin >> i1 >> i2;

	// get card from index position given
	Card c1 = this->getCardInHandAt(i1-1);
	Card c2 = this->getCardInHandAt(i2-1);

	// remove from hand
	//cout << "HERE1\n";
	this->removeFromHand(c1);
	//cout << "HERE2\n";
	this->removeFromHand(c2);
	//cout << "HERE3\n";

	return make_tuple(c1, c2);
}


Card HumanPlayer::playCard(int count) {
	// print out remaining play hand
	this->printPlayHand();
	// get next card to play
	std::cout << "Current round count: " << count << "\n";
	std::cout << "Play which card? ";
	int c_i;
	std::cin >> c_i;
	Card c = Card(0, "GO", "");
	if (c_i > 0) {
		c = this->getCardInPlayHandAt(c_i-1);
		this->removeFromPlayHand(c);
	}
	return c;	
}
