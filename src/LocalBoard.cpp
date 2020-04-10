#include <assert.h>
#include "LocalBoard.h"
#include "nutils.h"

using namespace std;

LocalBoard::LocalBoard(NPlayer * p1, NPlayer *p2) : Board(p1, p2) {
	// create players
	nplayer1_ = p1;
	nplayer2_ = p2;
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

	nplayer1_->setPlayerWindow(player1Window_);
	nplayer2_->setPlayerWindow(player2Window_);

	//update helper window
	this->displayHelpWindow();
}

void LocalBoard::startGameRound() {
	int winningScore = WINNING_SCORE_SHORT;
	while (player1_->position() < winningScore && 
		player2_->position() < winningScore) {
		// update scoring window
		this->updateScoreWindow();
		// reshuffle deck
		deck_.reshuffle();
		// deal the cards to each player
		this->dealRound();
		// start the 2 card discard
		this->initialDiscard();
		// cut the deck
		this->cutStarterCard();
		// start the play
		this->startPlay();	
		// wrap up the play
		this->endPlay();
		// start the counting
		this->countHandScores();
		// switch dealer
		dealer_ = dealer_^1;
		// clear everything that could have cards
		wclear(player1InfoWindow_);wrefresh(player1InfoWindow_);
		wclear(player1Window_);wrefresh(player1Window_);
		wclear(player2Window_);wrefresh(player2Window_);
		wclear(playWindow_);wrefresh(playWindow_);
		wclear(player2InfoWindow_);wrefresh(player2InfoWindow_);
	}
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

	// add an empty card to starter window to look like the deck (to be flipped)
	displayCardAt(starterWindow_, Card(), 0, 0);
	wrefresh(starterWindow_);
}

void LocalBoard::initialDiscard() {
	// display blanks for p2
	// display blank cards
	Card b = Card(0, " ", " ");
	vector<Card> blanks {b, b, b, b, b, b};
	this->displayHand(player2Window_, blanks);
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


void LocalBoard::cutStarterCard() {
	// get the starter
	starter_ = deck_.dealOne();

	// 2 points for dealer if jack
	if (starter_.suit().compare("J") == 0) {
		updateLog(players_[dealer_]->name() + " scored 2 for his heels");
		players_[dealer_]->advancePosition(2);
	}
	
	// display it in the starter window
	displayCardAt(starterWindow_, starter_, 0, 0);
	wrefresh(starterWindow_);
}

int LocalBoard::playRound(unsigned short int turn) {
	// clear play area
	wclear(playWindow_);wrefresh(playWindow_);

	updatePlayer1InfoWindow("Select a card to play");
	// display all player cards and allow to choose which to play
	this->displayHand(player1Window_, player1_->playHand());
	// display all blanks for player2
	Card b = Card(0, " ", " ");
	vector<Card> blanks {b, b, b, b};


	bool go[2] = { false, false };
	int count = 0;
	while (count < MAX_ROUND_SCORE) {
		// check to see if both players have said "GO" or both are out of cards
		if ((go[0] && go[1]) || 
			(player1_->isPlayHandEmpty() && player2_->isPlayHandEmpty())) {
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
			updateLog(players_[turn]->name() + " Played " + c.toString());
			// add card to vector of cards played this round
			playCards_.push_back(c);

			// add value to total count
			count += c.value();

			// display card played
			int disp_x = PLAY_WINDOW_PLAYER1_X;
			if (turn == PLAYER2) disp_x = PLAY_WINDOW_PLAYER2_X;
			displayCardAt(playWindow_, c, disp_x, 0);
			// display current count
			mvwprintw(playWindow_, CARD_HEIGHT,
				PLAY_WINDOW_PLAYER1_X-3, "%d", count);
			wrefresh(playWindow_);

			// check for scoring on play
			this->checkPlayCardsScoring(turn, count);
		} else {
			// played a "GO" card
			updateLog(players_[turn]->name() + " said GO");
			go[turn] = true;
			// automatically give other player 1 point for last card if this player was the first to say "GO"
			if (!go[turn^1]) {
				updateLog(players_[turn^1]->name() + " scored 1 point for last card");
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

void LocalBoard::endPlay() {
	// clear the windows
	wclear(playWindow_);wrefresh(playWindow_);
	wclear(player1Window_);wrefresh(player1Window_);

	// wait for input
	updatePlayer1InfoWindow("Press <G> to advance");
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

void LocalBoard::countHandScores() {
	// display the player 1 hand  
	this->displayHand(player1Window_, player1_->hand());
	// display player 2's hand
	this->displayHand(player2Window_, player2_->hand());
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
	updateLog(players_[dealer_^1]->name() + "'s hand scored " + 
		to_string(scores[dealer_^1]));
	updateScoreWindow();
	this->checkWin();
	
	// score dealer
	players_[dealer_]->advancePosition(scores[dealer_]);
	updateLog(players_[dealer_]->name() + "'s hand scored " + 
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
	wclear(player1InfoWindow_);wrefresh(player1InfoWindow_);
	wclear(player1Window_);wrefresh(player1Window_);
	wclear(player2Window_);wrefresh(player2Window_);
	wclear(playWindow_);wrefresh(playWindow_);
	wclear(player2InfoWindow_);wrefresh(player2InfoWindow_);

	// display the crib and get score
	if (dealer_ == PLAYER1) {
		this->displayHand(player1Window_, crib_);
	} else {
		this->displayHand(player2Window_, crib_);
	}
	wrefresh(player1Window_);
	int cribHand = getHandScore(crib_, CRIB);

	// add the crib's points to the dealer's
	if (dealer_ == PLAYER1) {
		updatePlayer1InfoWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	} else {
		updatePlayer2InfoWindow(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	}
	updateLog(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
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

void LocalBoard::displayHelpWindow() {
	wattron(helpWindow_, COLOR_WHITE | A_BOLD);
	wprintw(helpWindow_, "<S> Select Card, <G> Go/Advance");
	wrefresh(helpWindow_);
}

void LocalBoard::updatePlayer1InfoWindow(string s) {
	wclear(player1InfoWindow_);
	mvwprintw(player1InfoWindow_, 0, 0, "%s", s.c_str());
	wrefresh(player1InfoWindow_);
}

void LocalBoard::updatePlayer2InfoWindow(string s) {
	wclear(player2InfoWindow_);
	mvwprintw(player2InfoWindow_, 0, 0, "%s", s.c_str());
	wrefresh(player2InfoWindow_);
}

void LocalBoard::updateScoreWindow() {
	wclear(scoreWindow_);
	wprintw(scoreWindow_, "Game: %d\nPlayer: %d\nComputer: %d", 
		gameNumber_, player1_->position(), player2_->position());
	wrefresh(scoreWindow_);
}

void LocalBoard::updateLog(string s) {
	wprintw(logWindow_, "%s\n", s.c_str());
	wrefresh(logWindow_);
}

int LocalBoard::checkWin() {
	int winner = Board::checkWin();
	if (winner >= 0) {
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
	return winner;
}

void LocalBoard::displayHand(WINDOW * win, vector<Card> cards) {
	wclear(win);
	int x = 0;
	for(vector<Card>::iterator it = cards.begin(); it != cards.end(); ++it, x+=CARD_WIDTH+1) {
		Card c = (*it);
		displayCardAt(win, c, x, 0);
	}
	wrefresh(win);
}