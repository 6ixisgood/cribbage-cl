#include <assert.h>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "Board.h"
#include "ComputerPlayer.h"
#include "HumanPlayer.h"

using namespace std;

Board::Board() {
}

void Board::startGame() {
	// create deck 
	deck_ = Deck();
	// set dealer
	dealer_ = PLAYER;
		
}

void Board::startRound() {
	// reshuffle deck
	deck_.reshuffle();
	// deal the cards to each player
	this->dealRound();
	// start the 2 card discard
	this->initialDiscard();
	// cut the deck
	this->cutDeck();
	// start the play
	this->startPlay();	
	// start the counting
	this->countHandScores();
	// count the crib

}

void Board::dealRound() {
	// give 6 cards to each player
	for (int i=1; i<=6; i++) {
		assert(!deck_.isEmpty());
		player_.addToHand(deck_.dealOne());
		assert(!deck_.isEmpty());
		computer_.addToHand(deck_.dealOne());
	}
	player_.printHand();
}

void Board::initialDiscard() {
	auto computerDiscard = computer_.discardToCrib();
	auto playerDiscard = player_.discardToCrib();

	// add cards to crib
	crib_.push_back(get<0>(computerDiscard));
	crib_.push_back(get<1>(computerDiscard));
	crib_.push_back(get<0>(playerDiscard));
	crib_.push_back(get<1>(playerDiscard));
}

void Board::startPlay() {
	// copy remaining cards to "playHand"
	player_.setPlayHand(player_.hand());
	computer_.setPlayHand(computer_.hand());
	int turn = dealer_^1;
	while (!player_.isPlayHandEmpty() || !computer_.isPlayHandEmpty()) {
		cout << "Starting round\n";
		int next = this->playRound(turn);
		turn = next;
	}

}

int Board::playRound(unsigned short int turn) {
	bool go[2] = { false, false };
	int count = 0;
	while (count <31) {
		// check to see if both players have said "GO"
		if (go[0] && go[1]) {
			break;
		}
		// check to see if player has alread said "GO" and skip if so
		else if (go[turn]) {
			turn = turn^1;
			continue;
		}
		// remove card from player playHand and add to board playCards
		Card c = players_[turn]->playCard(count);
		std::cout << players_[turn]->name() << " played " << c.toString() << "\n";
		if (c.value() > 0) {
			playCards_.push_back(c);
			// add value to total count
			count += c.value();
			// check for scoring on play
			this->checkPlayCardsScoring(turn, count);
		} else {
			// played a "GO" card
			go[turn] = true;
			// automatically give other player 1 point for last card if this player was the first to say "GO"
			if (!go[turn^1])
				std::cout << players_[turn^1]->name() << " scored 1 point for last card" << "\n";
		}
		// switch player
		turn = turn^1;

	}
	// both players have said "GO"
	playCards_.clear();
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
			if (playCards_[size-1].name().compare(playCards_[size-2].name()) == 0) {
				// player scored 2 points for matching name
				std::cout <<  players_[turn]->name() << " scored 2 points for match" << "\n";
				players_[turn]->advancePosition(2);
			} else if (playCards_[size-1].value() + playCards_[size-2].value() == 15) {
				//player scored 2 points for 15
				players_[turn]->advancePosition(2);
			}
			break;
		//TODO work in runs of 3
	}
	if (count == 31) {
		std::cout << players_[turn]->name() << " scored 1 point for 31" << "\n";
		players_[turn]->advancePosition(1);
	}
}

void Board::cutDeck() {
	starter_ = deck_.dealOne();
}

void Board::countHandScores() {
	int pHand = getHandScore(player_.hand(), player_.name());
	int cHand = getHandScore(computer_.hand(), computer_.name());
	int cribHand = getHandScore(crib_, "crib");

	cout << "Starter: " << starter_.toString() << "\n";
	player_.printHand();
	cout << "Score: " << pHand << "\n";
	computer_.printHand();
	cout << "Score: " << cHand << "\n";
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
	cout << "Scoring for " << playerName << ":\n";
	int score = 0;
	for (vector<vector<Card>>::iterator it = allCombos.begin(); it != allCombos.end(); ++it) {
		vector<Card> comb = (*it);
		int length = comb.size();
		// check for 15
		auto sumLambda = [](int a, const Card& b) -> int { return a + b.value();};
		int sum = accumulate(comb.begin(), comb.end(), 0, sumLambda);
		if (sum == 15) {
			cout << "15 for 2\n";
			score += 2;
		}

		// check for nobs
		if (length == 1) {
			Card c = comb[0];
			if ((c.name().compare("J") == 0) && (c.suit().compare(starter_.suit()) == 0)) {
				cout << "nobs for 1\n";
				score += 1;
			}
		}

		// check for pair
		if (length == 2 && comb[0].name().compare(comb[1].name()) == 0) {
			cout << "two of a kind for 2\n";
			score +=2;
		}

		// check for runs
		if (length == 3) {
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
				cout << "straight for 3\n";
				score +=3;
			}
		}

		// check for flush
		if (length >= 4) {
			bool flush = true;
			string suit = comb[0].suit();
			int i = comb.size();
			while (i--) {
				if (comb[i].suit() != suit) {
					flush = false;
					break;
				}
			}
			if (flush) {
				cout << "flush for " << comb.size() << "\n" ;
				score += comb.size();

			}
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
		sort(v.begin(), v.end(), [](const Card& a, const Card& b) -> bool {
			return a.sortValue() > b.sortValue();		
		});
		// add the vector to vector of all combinations
		combinations.push_back(v);

		return;
	}

	for (int i=start; i <= end && end-i+1 >= r-index; i++) {
		data[index] = cards[i];
		this->combinationUtil(combinations, cards, data, i+1, end, index+1, r);
	}
}



void Board::addToCrib(Card c) {
	crib_.insert(crib_.begin(), c);
}

void Board::emptyCrib() {
	crib_.clear();
}


