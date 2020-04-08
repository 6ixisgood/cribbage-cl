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
}

void Board::initGame() {
	// create deck 
	deck_ = Deck();
	// set dealer
	dealer_ = PLAYER1;
	gameNumber_ = 1;		
}

void Board::startGameRound() {
	int winningScore = WINNING_SCORE_SHORT;
	while (player1_->position() < winningScore && 
		player2_->position() < winningScore) {
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
		// start the counting
		this->countHandScores();
		// switch dealer
		dealer_ = dealer_^1;
	}
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
		updateLog("Starting round " + to_string(count));
		
		int next = this->playRound(turn);
		turn = next;
		count++;
	}
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
				updateLog(players_[turn]->name() + " scored 2 points for 15");
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
				updateLog(players_[turn]->name() + " scored " + 
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
						updateLog(players_[turn]->name() + " scored " + 
							to_string(length) + " for a straight");
						players_[turn]->advancePosition(length);
						break;
					}
					// remove first card 
					cards.erase(cards.begin());
				}
			}
			break;
	}
	if (count == MAX_ROUND_SCORE) {
		updateLog(players_[turn]->name() + " scored 1 point for 31");
		players_[turn]->advancePosition(1);
	}
}

void Board::countHandScores() {
	// calculate scores locally
	int scores[2];
	scores[PLAYER1] = getHandScore(player1_->hand(), player1_->name());
	scores[PLAYER2] = getHandScore(player2_->hand(), player2_->name());
	
	// add score of non-dealer first and see if they won, locally
	players_[dealer_^1]->advancePosition(scores[dealer_^1]);
	updateLog(players_[dealer_^1]->name() + "'s hand scored " + 
		to_string(scores[dealer_^1]));
	this->checkWin();
	
	// score dealer
	players_[dealer_]->advancePosition(scores[dealer_]);
	updateLog(players_[dealer_]->name() + "'s hand scored " + 
		to_string(scores[dealer_]));
	this->checkWin();

	// do the crib
	players_[dealer_]->displayHand(crib_);
	int cribHand = getHandScore(crib_, CRIB);

	// add the crib's points to the dealer's
	updateLog(players_[dealer_]->name() + "'s crib scored " + to_string(cribHand));
	players_[dealer_]->advancePosition(cribHand);
	this->checkWin();
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

int Board::checkWin() {
	int winner = -1;
	if (player1_->position() >= WINNING_SCORE_LONG) {
		winner = PLAYER1;
	} else if (player2_->position() >= WINNING_SCORE_LONG) {
		winner = PLAYER2;
	}

	return winner;
}

void Board::updateLog(string s) {
	// do nothing rn
	return;
}
