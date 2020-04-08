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

	player1Window_ = newwin(PLAYER1_WINDOW_HEIGHT, PLAYER1_WINDOW_WIDTH, 
		PLAYER1_WINDOW_Y, PLAYER1_WINDOW_X);
	playWindow_ = newwin(PLAY_WINDOW_HEIGHT, PLAY_WINDOW_WIDTH, 
		PLAY_WINDOW_Y, PLAY_WINDOW_X);
	player2Window_ = newwin(PLAYER2_WINDOW_HEIGHT, PLAYER2_WINDOW_WIDTH,
		PLAYER2_WINDOW_Y, PLAYER2_WINDOW_X);
	starterWindow_ = newwin(STARTER_WINDOW_HEIGHT, STARTER_WINDOW_WIDTH, 
		STARTER_WINDOW_Y, STARTER_WINDOW_X);
	player1InfoWindow_ = newwin(PLAYER1_INFO_WINDOW_HEIGHT, PLAYER1_INFO_WINDOW_WIDTH,
		PLAYER1_INFO_WINDOW_Y, PLAYER1_INFO_WINDOW_X);
	player2InfoWindow_ = newwin(PLAYER2_INFO_WINDOW_HEIGHT, PLAYER2_INFO_WINDOW_WIDTH,
		PLAYER2_INFO_WINDOW_Y, PLAYER2_INFO_WINDOW_X);
	logWindow_ = newwin(LOG_WINDOW_HEIGHT, LOG_WINDOW_WIDTH,
		LOG_WINDOW_Y, LOG_WINDOW_X);
	scrollok(logWindow_, true);
	scoreWindow_ = newwin(SCORE_WINDOW_HEIGHT, SCORE_WINDOW_WIDTH,
		SCORE_WINDOW_Y, SCORE_WINDOW_X);
	helpWindow_ = newwin(1, max_x, max_y-1, 0);

	player1_->setPlayerWindow(player1Window_);
	player2_->setPlayerWindow(player2Window_);
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
		wclear(player1Window_);
		wrefresh(player1Window_);
		wclear(starterWindow_);
		wrefresh(starterWindow_);
	}

}


void Board::initialDiscard() {
	// display blanks for p2
	// display blank cards
	Card b = Card(0, " ", " ");
	vector<Card> blanks {b, b, b, b, b, b};
	player2_->displayHand(blanks);
	// ask each player to discard 2 cards
	vector<Card> p1Discard = players_[0]->discardCards();
	vector<Card> p2Discard = players_[1]->discardCards();
	
	// check lenghts
	assert(p1Discard.size() == 2);
	assert(p2Discard.size() == 2);

	// add each to crib
	crib_.push_back(p1Discard[0]);
	crib_.push_back(p1Discard[1]);
	crib_.push_back(p2Discard[0]);
	crib_.push_back(p2Discard[1]);

}

void Board::startPlay() {
	// copy remaining cards to "playHand"
	// both player's cards should be updated now
	player1_->setPlayHand(player1_->hand());
	player2_->setPlayHand(player2_->hand());
	int turn = dealer_^1;
	int count = 1;

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
	wclear(player1Window_);
	wrefresh(playWindow_);
	wrefresh(player1Window_);

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
	updatePlayer1InfoWindow("Select a card to play");
	// display all player cards and allow to choose which to play
	player1_->displayHand(player1_->playHand());
	// display all blanks for player2
	Card b = Card(0, " ", " ");
	vector<Card> blanks {b, b, b, b};


	bool go[2] = { false, false };
	int count = 0;
	while (count < MAX_ROUND_SCORE) {
		// check to see if both players have said "GO"
		if (go[0] && go[1]) {
			break;
		}
		// check to see if player has alread said "GO" and skip if so
		else if (go[turn]) {
			turn = turn^1;
			continue;
		}

		// player plays their card
		Card c = players_[turn]->playCard(count, MAX_ROUND_SCORE);

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

void Board::countHandScores() {
	// display the player 1 hand  
	player1_->displayHand(player1_->hand());
	// display player 2's hand
	player2_->displayHand(player2_->hand());
	// calculate scores locally
	int scores[2];
	scores[PLAYER1] = getHandScore(player1_->hand(), player1_->name());
	scores[PLAYER2] = getHandScore(player2_->hand(), player2_->name());

	// update the info windows so you can see the scores no matter the order
	updatePlayer1InfoWindow(player1_->name() + "'s hand scored " + 
		to_string(scores[PLAYER1]));
	updatePlayer2InfoWindow(player2_->name() + "'s hand scored " + 
		to_string(scores[PLAYER2]));
	
	// add score of non-dealer first and see if they won, locally
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
	wclear(player1Window_);
	wclear(playWindow_);
	wrefresh(playWindow_);
	wclear(player2InfoWindow_);
	wrefresh(player2InfoWindow_);
	// display the crib and get score
	players_[dealer_]->displayHand(crib_);
	wrefresh(player1Window_);
	int cribHand = getHandScore(crib_, CRIB);

	// add the crib's points to the dealer's
	if (dealer_ == PLAYER1) {
		updatePlayer1InfoWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	} else {
		updatePlayer2InfoWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	}
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

void Board::displayHelpWindow() {
	wattron(helpWindow_, COLOR_WHITE | A_BOLD);
	wprintw(helpWindow_, "<S> Select Card, <G> Go/Advance");
	wrefresh(helpWindow_);
}

void Board::updatePlayer1InfoWindow(string s) {
	wclear(player1InfoWindow_);
	mvwprintw(player1InfoWindow_, 0, 0, "%s", s.c_str());
	wrefresh(player1InfoWindow_);
}

void Board::updatePlayer2InfoWindow(string s) {
	wclear(player2InfoWindow_);
	mvwprintw(player2InfoWindow_, 0, 0, "%s", s.c_str());
	wrefresh(player2InfoWindow_);
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
		wclear(player1Window_);
		wrefresh(player1Window_);
		wclear(playWindow_);
		wrefresh(playWindow_);
		wclear(player2InfoWindow_);
		wrefresh(player2InfoWindow_);
		updatePlayer1InfoWindow("GAME OVER!");

		getch();
		endwin();
		exit(0);
	}

}

