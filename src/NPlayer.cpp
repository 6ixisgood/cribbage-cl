#include "NPlayer.h"
#include "nutils.h"

using namespace std;

void NPlayer::setPlayerWindow(WINDOW * win) {
		playerWindow_ = win; 
}

void NPlayer::displayHand(vector<Card> cards) {
	wclear(playerWindow_);
	int x = 0;
	for(vector<Card>::iterator it = cards.begin(); it != cards.end(); ++it, x+=CARD_WIDTH+1) {
		Card c = (*it);
		displayCardAt(playerWindow_, c, x, 0);
	}
	wrefresh(playerWindow_);
}