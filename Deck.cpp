#include <assert.h>
#include <algorithm>
#include <random>
#include "Deck.h"

bool Deck::isEmpty() {
	return deck_.empty();
}
Card Deck::dealOne() {
	assert(!this->isEmpty());
	Card dealt = deck_.top();
	deck_.pop();
	return dealt;
}

void Deck::reshuffle() {
	// remove all from current stack
	while (!deck_.empty()) {
		deck_.pop();
	}

	// shuffle vector of cards and push to empty stack
	std::shuffle(std::begin(cards_), std::end(cards_), std::random_device {});
	for(std::vector<Card>::iterator it = cards_.begin(); it != cards_.end(); ++it) {
		deck_.push(*it);
	}
}
