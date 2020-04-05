#include <iostream>
#include "ComputerPlayer.h"

using namespace std;

ComputerPlayer::ComputerPlayer(string name) : Player(name) {}

tuple<Card, Card> ComputerPlayer::discardToCrib() {
	// just take the first two cards out of the computer's hand
	Card c1 = this->getCardInHandAt(0);
	Card c2 = this->getCardInHandAt(1);

	//cout << "HERE1\n";
	this->removeFromHand(c1);	
	//cout << "HERE2\n";
	this->removeFromHand(c2);
	//cout << "HERE3\n";
	return make_tuple(c1, c2);

}

Card ComputerPlayer::playCard(int count) {
	// loop through cards and play first one that will work with current count
	Card c = Card(0, "GO", "");
	// create iterator to loop over
	vector<Card> v = this->playHand();
	for (vector<Card>::iterator it = v.begin(); it != v.end(); it++) {
		if ((*it).value() <= 31 - count && (*it).value() !=0) {
			c = *(it);
			this->removeFromPlayHand((*it));
			break;
		}
	}
	return c;
}
