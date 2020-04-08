#include <assert.h>
#include "LocalBoard.h"

using namespace std;

LocalBoard::LocalBoard(Player * p1, Player *p2) : Board(p1, p2) {

}

void LocalBoard::dealRound() {
    // clear out player's hands and crib
	player1_->emptyHand();
	player1_->emptyPlayHand();
	player2_->emptyHand();
	player2_->emptyPlayHand();
	crib_.clear();

	// give 6 cards to each player
	for (int i=1; i<=6; i++) {
		assert(!deck_.isEmpty());
		player1_->addToHand(deck_.dealOne());
		assert(!deck_.isEmpty());
		player2_->addToHand(deck_.dealOne());
	}
	assert(player1_->hand().size() == 6);
	assert(player2_->hand().size() == 6);
}

/** 
 * Have the computer auto discard the first 2 cards it got
*/
void LocalBoard::player2InitialDiscard() {
    // have computer auto discard
	auto computerDiscard = player2_->discardToCrib();
	// add cards to crib
	crib_.push_back(get<0>(computerDiscard));
	crib_.push_back(get<1>(computerDiscard));
}

Card LocalBoard::player2PlayCard(int count) {
    return player2_->playCard(count);
}

void LocalBoard::cutStarterCard() {
	// get the starter
	starter_ = deck_.dealOne();

	// 2 points for dealer if jack
	if (starter_.suit().compare("J") == 0) {
		updateLogWindow(players_[dealer_]->name() + " scored 2 for his heels");
		players_[dealer_]->advancePosition(2);
	}
	
	// display it in the starter window
	starter_.displayCardAt(starterWindow_, 0, 0);
	wrefresh(starterWindow_);
}

void LocalBoard::displayPlayer2Hand() {
    this->displayHand(playWindow_, player2_->hand());
    wrefresh(playWindow_);
}
