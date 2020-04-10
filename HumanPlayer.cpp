#define CURSOR '^'

#include <iostream>
#include "HumanPlayer.h"
#include "Card.h"

using namespace std;

HumanPlayer::HumanPlayer(string name) : NPlayer(name) {}

vector<Card> HumanPlayer::discardCards() {
	// display player's hand
	this->displayHand(hand_);
	// draw initial selection arrow
	int cur_y_ = CARD_HEIGHT;
	int cur_x_ = (int) (CARD_WIDTH/2);
	mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
	int cursorPosition = 0;

	// set up loop for card selection
	int deleted = 0;
	//TODO updateInfoWindow("Discard Cards (" + to_string(deleted) + "/2)");
	int cardsLength = hand_.size();

	vector<Card> discard;

	while (1) {
		if (deleted < 2) {
			switch(toupper(wgetch(playerWindow_))) {
				case '\033':
					getch();
					switch(getchar()) {
						case 'D':
							// arrow left
							mvwaddch(playerWindow_, cur_y_, cur_x_, ' ');
							cur_x_ -= CARD_WIDTH+1;
							cursorPosition -= 1;
							if (cur_x_ < 0) {
								cur_x_ = (int) (CARD_WIDTH/2) + (CARD_WIDTH+1) * (cardsLength-deleted-1);
								cursorPosition = cardsLength-deleted-1;
							}
							mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
							break;
						case 'C':
							// arrow right
							mvwaddch(playerWindow_, cur_y_, cur_x_, ' ');
							cur_x_ += CARD_WIDTH+1;
							cursorPosition += 1;
							if (cur_x_ > 2 + (CARD_WIDTH+1) * (cardsLength-deleted-1)) {
								cur_x_ = (int) (CARD_WIDTH/2);
								cursorPosition = 0;
							}
							mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
							break;
					}
					break;
				
				case 'S': {
					// discard selected card
					Card c = hand_[cursorPosition];
					discard.push_back(c);
					// this->addToCrib(c);
					this->removeFromHand(c);
					deleted++;

					// redisplay player's cards
					wclear(playerWindow_);
					this->displayHand(hand_);

					// check to make sure cursor is now in good position
					if (cursorPosition == cardsLength-deleted) {
						cur_x_ -= CARD_WIDTH+1;
						cursorPosition--;
					}
					mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
					wrefresh(playerWindow_);


					// update info bar 
					// TODO updateInfoWindow("Discard Cards (" + to_string(deleted) + "/2)");
					break;
				}
				case 'Q':
					break;
			}
		} else {
			// leave selection loop
			break;
		}
	}
	return discard;
}


Card HumanPlayer::playCard(int count, int max) {
	// draw initial selection arrow
	int cur_y_ = CARD_HEIGHT;
	int cur_x_ = (int) (CARD_WIDTH/2);
	mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
	int cursorPosition = 0;

	// for player's turn let them see their cards and select which one to use
	Card c = Card(0, "GO", "");
	bool removed = false;
	while (1) {
		if (!removed) {
			switch(toupper(wgetch(playerWindow_))) {
				case '\033':
					getch();
					switch(getchar()) {
						case 'D':
							// arrow left
							mvwaddch(playerWindow_, cur_y_, cur_x_, ' ');
							cur_x_ -= CARD_WIDTH+1;
							cursorPosition -= 1;
							if (cur_x_ < 0) {
								cur_x_ = (int) (CARD_WIDTH/2) + (CARD_WIDTH+1) * (playHand_.size()-1);
								cursorPosition = playHand_.size()-1;
							}
							mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
							break;
						case 'C':
							// arrow right
							mvwaddch(playerWindow_, cur_y_, cur_x_, ' ');
							cur_x_ += CARD_WIDTH+1;
							cursorPosition += 1;
							if (cur_x_ > 2 + (CARD_WIDTH+1) * (playHand_.size()-1)) {
								cur_x_ = (int) (CARD_WIDTH/2);
								cursorPosition = 0;
							}
							mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
							break;
					}
					break;
				
				case 'S':
					// save card selected
					c = playHand_[cursorPosition];	

					// check if legal play
					if (c.value()+count > max) {
						//TODO updateInfoWindow("Can't play that card");
						break;
					}

					// remove card from hand
					this->removeFromPlayHand(c);

					// redisplay player's cards
					wclear(playerWindow_);
					this->displayHand(playHand_);
					wrefresh(playerWindow_);

					// place selection cursor back to begining 
					cur_x_ = (int) (CARD_WIDTH/2);
					mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
					cursorPosition = 0;

					// get out of loop
					removed = true;
					break;
				case 'G': {
					// player says "GO"
					// make sure that the player doesn't have any more cards to play
					vector<Card> hand = playHand_;
					int pointsLeft = max - count;
					bool canPlay = false;
					for (vector<Card>::iterator it = hand.begin(); it != hand.end(); ++it) {
						if ((*it).value() <= pointsLeft) {
							canPlay = true;
							break;
						}
					}
					if (canPlay) {
						//TODO updateInfoWindow("You must play a card");
					} else {
						c = Card(0, "GO", "");
						removed = true;
					}
					break;
				}
				default:
					break;
			}
		} else {
			// leave selection loop
			break;
		}
	}
	return c;	
}
