#include <iostream>
#include "ComputerPlayer.h"

using namespace std;

ComputerPlayer::ComputerPlayer(string name) : Player(name) {}

vector<Card> ComputerPlayer::discardCards() {
	// just take the first two cards out of the computer's hand
	Card c1 = this->getCardInHandAt(0);
	Card c2 = this->getCardInHandAt(1);

	this->removeFromHand(c1);	
	this->removeFromHand(c2);

	return vector<Card> {c1, c2};
}

Card ComputerPlayer::playCard(int count, int max) {
	// loop through cards and play first one that will work with current count
	Card c = Card(0, "GO", "");
	// create iterator to loop over
	vector<Card> v = playHand_;
	for (vector<Card>::iterator it = v.begin(); it != v.end(); it++) {
		if ((*it).value() <= max - count && (*it).value() !=0) {
			c = *(it);
			this->removeFromPlayHand((*it));
			break;
		}
	}

	// display all blanks 
	Card b = Card(0, " ", " ");
	vector<Card> blanks;
	for (int i=0; i<playHand_.size(); i++) {
		blanks.push_back(b);
	}
	this->displayHand(blanks);
	
	return c;
}

