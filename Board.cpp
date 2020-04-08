#include <assert.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <ncursesw/curses.h>
#include <unistd.h>
#include "Board.h"
#include "ComputerPlayer.h"
#include "HumanPlayer.h"

using namespace std;

Board::Board(Player * p1, Player *p2) {
	// create players 
	
	player1_ = p1;
	player2_ = p2;
	players_ = {player1_, player2_};

	// set up the windows 
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);

	playerWindow_ = newwin(PLAYER1_WINDOW_HEIGHT, PLAYER1_WINDOW_WIDTH, 
		PLAYER1_WINDOW_Y, PLAYER1_WINDOW_X);
	playWindow_ = newwin(PLAY_WINDOW_HEIGHT, PLAY_WINDOW_WIDTH, 
		PLAY_WINDOW_Y, PLAY_WINDOW_X);
	starterWindow_ = newwin(STARTER_WINDOW_HEIGHT, STARTER_WINDOW_WIDTH, 
		STARTER_WINDOW_Y, STARTER_WINDOW_X);
	infoWindow_ = newwin(INFO_WINDOW_HEIGHT, INFO_WINDOW_WIDTH,
		INFO_WINDOW_Y, INFO_WINDOW_X);
	infoWindow2_ = newwin(INFO_WINDOW2_HEIGHT, INFO_WINDOW2_WIDTH,
		INFO_WINDOW2_Y, INFO_WINDOW2_X);
	logWindow_ = newwin(LOG_WINDOW_HEIGHT, LOG_WINDOW_WIDTH,
		LOG_WINDOW_Y, LOG_WINDOW_X);
	scrollok(logWindow_, true);
	scoreWindow_ = newwin(SCORE_WINDOW_HEIGHT, SCORE_WINDOW_WIDTH,
		SCORE_WINDOW_Y, SCORE_WINDOW_X);
	helpWindow_ = newwin(1, max_x, max_y-1, 0);
}

void Board::startGame() {
	// create deck 
	deck_ = Deck();
	// set dealer
	dealer_ = PLAYER1;
	// update helper window
	this->displayHelpWindow();
	gameNumber_ = 1;
		
}

void Board::startRound() {
	int winningScore = WINNING_SCORE_SHORT;
	while (player1_->position() < winningScore && 
		player2_->position() < winningScore) {
		// update scoring window
		this->updateScoreWindow();
		// reshuffle deck
		deck_.reshuffle();
		// deal the cards to each player
		this->dealRound();
		// add an empty card to starter window to look like the deck (to be flipped)
		Card(0," "," ").displayCardAt(starterWindow_, 0, 0);
		wrefresh(starterWindow_);
		// start the 2 card discard
		this->initialDiscard();
		// cut the deck
		this->cutStarterCard();
		// start the play
		this->startPlay();	
		// start the counting
		this->countHandScores();
		// switch dealer
		dealer_ = dealer_^1;
		// clear everything that could have cards
		wclear(playerWindow_);
		wrefresh(playerWindow_);
		wclear(starterWindow_);
		wrefresh(starterWindow_);
	}

}

void Board::dealRound() {
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

void Board::initialDiscard() {
	// have computer auto discard
	auto computerDiscard = player2_->discardToCrib();
	// add cards to crib
	crib_.push_back(get<0>(computerDiscard));
	crib_.push_back(get<1>(computerDiscard));

	// display all player cards and allow to choose which to discard
	this->displayHand(playerWindow_, player1_->hand());
	int cursorPosition = 0;

	// draw initial selection arrow
	cur_y_ = CARD_HEIGHT;
	cur_x_ = (int) (CARD_WIDTH/2);
	mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);

	// set up loop for card selection
	int deleted = 0;
	updateInfoWindow("Discard Cards (" + to_string(deleted) + "/2)");
	int cardsLength = player1_->hand().size();

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
				
				case 'S':
					// discard selected card
					this->addToCrib(player1_->hand()[cursorPosition]);
					player1_->removeFromHand(player1_->hand()[cursorPosition]);
					deleted++;

					// redisplay player's cards
					wclear(playerWindow_);
					this->displayHand(playerWindow_, player1_->hand());

					// check to make sure cursor is now in good position
					if (cursorPosition == cardsLength-deleted) {
						cur_x_ -= CARD_WIDTH+1;
						cursorPosition--;
					}
					mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
					wrefresh(playerWindow_);


					// update info bar 
					updateInfoWindow("Discard Cards (" + to_string(deleted) + "/2)");
					break;
				case 'Q':
					break;
			}
		} else {
			// leave selection loop
			break;
		}
	}
}

void Board::startPlay() {
	// copy remaining cards to "playHand"
	player1_->setPlayHand(player1_->hand());
	player2_->setPlayHand(player2_->hand());
	int turn = dealer_^1;
	int count = 1;
	// update info window 2 status
	updateInfoWindow2("The Play");
	// keep playing until all cards are gone
	while (!player1_->isPlayHandEmpty() || !player2_->isPlayHandEmpty()) {
		updateLogWindow("Starting round " + to_string(count));
		
		// clear out play area
		playCards_.clear();
		wclear(playWindow_);
		wrefresh(playWindow_);

		int next = this->playRound(turn);
		turn = next;
		count++;
	}
	// clear the windows
	wclear(playWindow_);
	wclear(playerWindow_);

	// wait for input
	bool input = false;
	while (1) {
		switch (toupper(getch())) {
		case 'G':
			// break out of loop
			input = true;
			break;
		
		default:
			break;
		}
		if (input) break;
	}
}

int Board::playRound(unsigned short int turn) {
	updateInfoWindow("Select a card to play");
	// display all player cards and allow to choose which to play
	this->displayHand(playerWindow_, player1_->playHand());

	// draw initial selection arrow
	cur_y_ = CARD_HEIGHT;
	cur_x_ = (int) (CARD_WIDTH/2);
	mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);

	bool go[2] = { false, false };
	int count = 0;
	while (count <MAX_ROUND_SCORE) {
		// check to see if both players have said "GO"
		if (go[0] && go[1]) {
			break;
		}
		// check to see if player has alread said "GO" and skip if so
		else if (go[turn]) {
			turn = turn^1;
			continue;
		}
		// default "GO" card
		Card c = Card(0, "GO", "");
		// if it's the computer's turn just let it go
		if (turn == PLAYER2 && !players_[turn]->isPlayHandEmpty()) {
			c = players_[turn]->playCard(count);
		} else if (!players_[turn]->isPlayHandEmpty()){
			// for player's turn let them see their cards and select which one to use
			bool removed = false;
			int cursorPosition = 0;
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
										cur_x_ = (int) (CARD_WIDTH/2) + (CARD_WIDTH+1) * (player1_->playHand().size()-1);
										cursorPosition = player1_->playHand().size()-1;
									}
									mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
									break;
								case 'C':
									// arrow right
									mvwaddch(playerWindow_, cur_y_, cur_x_, ' ');
									cur_x_ += CARD_WIDTH+1;
									cursorPosition += 1;
									if (cur_x_ > 2 + (CARD_WIDTH+1) * (player1_->playHand().size()-1)) {
										cur_x_ = (int) (CARD_WIDTH/2);
										cursorPosition = 0;
									}
									mvwaddch(playerWindow_, cur_y_, cur_x_, CURSOR);
									break;
							}
							break;
						
						case 'S':
							// save card selected
							c = player1_->playHand()[cursorPosition];	

							// check if legal play
							if (c.value()+count > MAX_ROUND_SCORE) {
								updateInfoWindow("Can't play that card");
								break;
							}

							player1_->removeFromPlayHand(player1_->playHand()[cursorPosition]);

							// redisplay player's cards
							wclear(playerWindow_);
							this->displayHand(playerWindow_, player1_->playHand());
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
							vector<Card> hand = player1_->playHand();
							int pointsLeft = MAX_ROUND_SCORE - count;
							bool canPlay = false;
							for (vector<Card>::iterator it = hand.begin(); it != hand.end(); ++it) {
								if ((*it).value() <= pointsLeft) {
									canPlay = true;
									break;
								}
							}
							if (canPlay) {
								updateInfoWindow("You must play a card");
							} else {
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
		}

		// remove card from player playHand and add to board playCards
		if (c.value() > 0) {
			updateLogWindow(players_[turn]->name() + " Played " + c.toString());
			// add card to vector of cards played this round
			playCards_.push_back(c);

			// add value to total count
			count += c.value();


			// display card played
			int disp_x = PLAY_WINDOW_PLAYER1_X;
			if (turn == PLAYER2) disp_x = PLAY_WINDOW_PLAYER2_X;
			c.displayCardAt(playWindow_, disp_x, 0);
			// display current count
			mvwprintw(playWindow_, CARD_HEIGHT,
				PLAY_WINDOW_PLAYER1_X-3, "%d", count);
			wrefresh(playWindow_);

			// check for scoring on play
			this->checkPlayCardsScoring(turn, count);
		} else {
			// played a "GO" card
			updateLogWindow(players_[turn]->name() + " said GO");
			go[turn] = true;
			// automatically give other player 1 point for last card if this player was the first to say "GO"
			if (!go[turn^1]) {
				updateLogWindow(players_[turn^1]->name() + " scored 1 point for last card");
				players_[turn^1]->advancePosition(1);
			}
		}
		// show score updates
		this->updateScoreWindow();
		// switch player
		turn = turn^1;

	}
	// both players have said "GO"
	return turn;
}

void Board::checkPlayCardsScoring(unsigned short int turn, int count) {
	int size = playCards_.size();
	switch(size) {
		case 0:
			break;
		case 1:
			break;
		default:
			if (count == 15) {
				//player scored 2 points for 15
				updateLogWindow(players_[turn]->name() + " scored 2 points for 15");
				players_[turn]->advancePosition(2);
			}

			// first check for combos of same rank
			int same = 0;
			int pos = size-1;
			string rank = playCards_[pos].rank();
			while(pos--) {
				if (playCards_[pos].rank().compare(rank) == 0) {
					same++;
				} else {
					break;
				}
			}
			// add points if they got any
			if (same > 0) {
				same++;
				updateLogWindow(players_[turn]->name() + " scored " + 
					to_string(same) + " points for match");
				players_[turn]->advancePosition(same);
			}

			// now check for straights
			if (size >= 3) {
				int posibilities = size - 2;
				vector<Card> cards = playCards_;
				while(posibilities--) {
					// look at last posibilities+2 cards
					// sort cards
					vector<Card> sortedCards = cards;
					sort(sortedCards.begin(), sortedCards.end());
					// check order
					int position = sortedCards.size()-1;
					int highest = sortedCards[position].sortValue();
					bool straight = true;
					int i = 1;
					for(position--; position>=0; i++, position--) {
						if (sortedCards[position].sortValue() != highest-i) {
							straight = false;
							break;
						}
					}
					if (straight) {
						int length = i;
						updateLogWindow(players_[turn]->name() + " scored " + 
							to_string(length) + " for a straight");
						players_[turn]->advancePosition(length);
						break;
					}
					// remove first card 
					cards.erase(cards.begin());
				}
			}
			break;
		//TODO work in runs of 3
	}
	if (count == MAX_ROUND_SCORE) {
		updateLogWindow(players_[turn]->name() + " scored 1 point for 31");
		players_[turn]->advancePosition(1);
	}
}

void Board::cutStarterCard() {
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

void Board::countHandScores() {
	// display the player's hand and computer's hand and update scores 
	this->displayHand(playerWindow_, player1_->hand());
	this->displayHand(playWindow_, player2_->hand());
	// refresh windows
	wrefresh(playerWindow_);
	wrefresh(playWindow_);

	// calculate scores
	int scores[2];
	scores[PLAYER1] = getHandScore(player1_->hand(), player1_->name());
	scores[PLAYER2] = getHandScore(player2_->hand(), player2_->name());

	// update the info windows so you can see the scores no matter the order
	updateInfoWindow(player1_->name() + "'s hand scored " + 
		to_string(scores[PLAYER1]));
	updateInfoWindow2(player2_->name() + "'s hand scored " + 
		to_string(scores[PLAYER2]));
	
	// add score of non-dealer first and see if they won
	players_[dealer_^1]->advancePosition(scores[dealer_^1]);
	updateLogWindow(players_[dealer_^1]->name() + "'s hand scored " + 
		to_string(scores[dealer_^1]));
	updateScoreWindow();
	this->checkWin();
	
	// score dealer
	players_[dealer_]->advancePosition(scores[dealer_]);
	updateLogWindow(players_[dealer_]->name() + "'s hand scored " + 
		to_string(scores[dealer_]));
	updateScoreWindow();
	this->checkWin();

	// wait for user input to score crib
	bool input = false;
	while (1) {
		switch (toupper(getch())) {
		case 'G':
			// break out of loop
			input = true;
			break;
		
		default:
			break;
		}
		if (input) break;
	}

	// clear the player's and computer's hand
	wclear(playerWindow_);
	wclear(playWindow_);
	wrefresh(playWindow_);
	wclear(infoWindow2_);
	wrefresh(infoWindow2_);
	// display the crib and get score
	this->displayHand(playerWindow_, crib_);
	wrefresh(playerWindow_);
	int cribHand = getHandScore(crib_, CRIB);

	// add the crib's points to the dealer's
	updateInfoWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	updateLogWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	players_[dealer_]->advancePosition(cribHand);
	updateScoreWindow();

	this->checkWin();


	// wait for user input to move on
	input = false;
	while (1) {
		switch (toupper(getch())) {
		case 'G':
			// break out of loop
			input = true;
			break;
		
		default:
			break;
		}
		if (input) break;
	}
}

int Board::getHandScore(vector<Card> cards, string playerName) {
	// add the starter to the hand
	cards.push_back(starter_);
	vector<vector<Card>> allCombos;
	for (int r=1; r<=cards.size(); r++) {
		// temp array
		Card data[r];
		// get combos of length r
		this->combinationUtil(allCombos, cards, data, 0, cards.size()-1, 0, r);
	}
	
	// loop through all permutations and look for scoring
	int score = 0;
	// handle 5 card flush case
	bool fiveCardFlush = false;
	// go in reverse to get 5 card first
	for (vector<vector<Card>>::reverse_iterator it = allCombos.rbegin(); it != allCombos.rend(); ++it) {
		vector<Card> comb = (*it);
		int length = comb.size();
		// check for 15
		auto sumLambda = [](int a, const Card& b) -> int { return a + b.value();};
		int sum = accumulate(comb.begin(), comb.end(), 0, sumLambda);
		if (sum == 15) {
			//cout << "15 for 2\n";
			score += 2;
		}

		// check for nobs
		if (length == 1) {
			Card c = comb[0];
			if ((c.rank().compare("J") == 0) && (c.suit().compare(starter_.suit()) == 0)) {
				//cout << "nobs for 1\n";
				score += 1;
			}
		}

		// check for pair
		if (length == 2 && comb[0].rank().compare(comb[1].rank()) == 0) {
			//cout << "two of a kind for 2\n";
			score +=2;
		}

		// check for runs
		if (length >= 3) {
			bool straight = true;
			Card last = comb[comb.size()-1];
			int i=comb.size()-1;
			while (i--) {
				if (comb[i].sortValue()-1 != last.sortValue()) {
					straight = false;
					break;
				}
				last = comb[i];
			}
			if (straight) {
				//cout << "straight for 3\n";
				score += length;
			}
		}

		// check for flush
		if (length >= 4) {
			bool flush = false;
			if (length == 5) {
				flush = true;
				string suit = comb[0].suit();
				int i = length;
				while (i--) {
					if (comb[i].suit() != suit) {
						flush = false;
						break;
					}
				}
				fiveCardFlush = flush;
			} else if (playerName.compare(CRIB) && !(fiveCardFlush)){
				// 4 card flush can't be in crib
				flush = true;
				string suit = comb[0].suit();
				int i = length;
				while (i--) {
					// if starter is in here, it doesn't count
					if (comb[i].suit() != suit ||
						comb[i] == starter_) {
						flush = false;
						break;
					}
				}

			}
			if (flush) score += length;
		}
	
	}
	return score;

}

void Board::combinationUtil(vector<vector<Card>> &combinations, vector<Card> cards, Card data[], 
		int start, int end, int index, int r) {
	// combination of length r reached
	if (index == r) {
		// add combination to own vector
		vector<Card> v(data, data + r);
		// sort cards
		sort(v.begin(), v.end());		
		// add the vector to vector of all combinations
		combinations.push_back(v);

		return;
	}

	for (int i=start; i <= end && end-i+1 >= r-index; i++) {
		data[index] = cards[i];
		this->combinationUtil(combinations, cards, data, i+1, end, index+1, r);
	}
}

void Board::displayHand(WINDOW *win, vector<Card> cards) {
	wclear(win);
	int x = 0;
	for(vector<Card>::iterator it = cards.begin(); it != cards.end(); ++it, x+=CARD_WIDTH+1) {
		Card c = (*it);
		c.displayCardAt(win, x, 0);
	}
}

void Board::displayHelpWindow() {
	wattron(helpWindow_, COLOR_WHITE | A_BOLD);
	wprintw(helpWindow_, "<S> Select Card, <G> Go/Advance");
	wrefresh(helpWindow_);
}

void Board::updateInfoWindow(string s) {
	wclear(infoWindow_);
	mvwprintw(infoWindow_, 0, 0, "%s", s.c_str());
	wrefresh(infoWindow_);
}

void Board::updateInfoWindow2(string s) {
	wclear(infoWindow2_);
	mvwprintw(infoWindow2_, 0, 0, "%s", s.c_str());
	wrefresh(infoWindow2_);
}

void Board::addToCrib(Card c) {
	crib_.insert(crib_.begin(), c);
}

void Board::emptyCrib() {
	crib_.clear();
}

void Board::updateScoreWindow() {
	wclear(scoreWindow_);
	wprintw(scoreWindow_, "Game: %d\nPlayer: %d\nComputer: %d", 
		gameNumber_, player1_->position(), player2_->position());
	wrefresh(scoreWindow_);
}

void Board::updateLogWindow(string s) {
	wprintw(logWindow_, "%s\n", s.c_str());
	wrefresh(logWindow_);
}

bool sortValues(Card &a, Card &b) {
	return a.sortValue() > b.sortValue();
}

void Board::checkWin() {
	bool win = false;

	if (player1_->position() >= WINNING_SCORE_LONG) {
		updateLogWindow(player1_->name() + " won!");
		win = true;
	} else if (player2_->position() >= WINNING_SCORE_LONG) {
		updateLogWindow(player2_->name() + " won!");
		win = true;
	}

	if (win) {
		wclear(playerWindow_);
		wrefresh(playerWindow_);
		wclear(playWindow_);
		wrefresh(playWindow_);
		wclear(infoWindow2_);
		wrefresh(infoWindow2_);
		updateInfoWindow("GAME OVER!");

		getch();
		endwin();
		exit(0);
	}

}

