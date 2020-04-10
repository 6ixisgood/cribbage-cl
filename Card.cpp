#include <string>
#include "Card.h"
using namespace std;

Card::Card() : value_(0), rank_(""), suit_("") {}

Card::Card(int value, string rank, string suit) : value_(value), rank_(rank), suit_(suit) {}

Card::Card(const Card& origin) : value_(origin.value_), rank_(origin.rank_), suit_(origin.suit_) {}

void Card::setValue(int value) { value_ = value; }

void Card::setRank(string rank) { rank_ = rank; }

void Card::setSuit(string suit) { suit_ = suit; }

int Card::sortValue() const {
	return sortValue_.at(rank_);
}

bool Card::operator==(const Card& rhs) const {
	return ((this->rank_).compare(rhs.rank_) == 0  && 
			(this->suit_).compare(rhs.suit_) == 0);
}

Card& Card::operator=(const Card& rhs) {
	rank_ = rhs.rank_;
	value_ = rhs.value_;
	suit_ = rhs.suit_;

	return *this;
}