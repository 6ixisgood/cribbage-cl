#include <string>
#include <ncursesw/curses.h>
#include "Card.h"
using namespace std;

Card::Card() {}

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

/**
 * Draws card in given window at given position
 */
void Card::displayCardAt(WINDOW * win, int x, int y) {
	// create color pairs and set
	init_pair(2, COLOR_RED, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_WHITE, COLOR_WHITE);
	if (this->suit_.compare("H") == 0|| this->suit_.compare("D") == 0) {
		wattron(win, COLOR_PAIR(2));
	} else if (this->suit_.compare("C") == 0 || this->suit_.compare("S") == 0){
		wattron(win, COLOR_PAIR(3));
	} else {
		wattron(win, COLOR_PAIR(4));
	}

	// draw card
	string space = " ";
	if (this->toString().size() > 2) space = "";
	mvwprintw(win, y+0, x, ""); waddch(win, ACS_ULCORNER); waddch(win, ACS_HLINE);
		waddch(win, ACS_HLINE); waddch(win, ACS_HLINE); waddch(win, ACS_URCORNER);
	mvwprintw(win, y+1, x, ""); waddch(win, ACS_VLINE); 
		wprintw(win, "%s%s", this->toString().c_str(), space.c_str()); waddch(win, ACS_VLINE);
	mvwprintw(win, y+2, x, ""); waddch(win, ACS_VLINE); 
		wprintw(win, "   "); waddch(win, ACS_VLINE);
	mvwprintw(win, y+3, x, ""); waddch(win, ACS_VLINE); 
		wprintw(win, "%s%s", space.c_str(), this->toString().c_str()); waddch(win, ACS_VLINE);
	mvwprintw(win, y+4, x, ""); waddch(win, ACS_LLCORNER); waddch(win, ACS_HLINE);
		waddch(win, ACS_HLINE); waddch(win, ACS_HLINE); waddch(win, ACS_LRCORNER);

	// turn off color pair
	if (this->suit_.compare("H") == 0|| this->suit_.compare("D") == 0) {
		wattroff(win, COLOR_PAIR(2));
	} else if (this->suit_.compare("C") == 0 || this->suit_.compare("S") == 0){
		wattroff(win, COLOR_PAIR(3));
	} else {
		wattroff(win, COLOR_PAIR(4));
	}
}
