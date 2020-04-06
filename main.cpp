#define _XOPEN_SOURCE_EXTENDED
// #define CARD_X_MIN 2
// #define CARD_Y_MIN 2
// #define CURSOR_HOP_WIDTH 6
#include <ncursesw/curses.h>
#include <unistd.h>
#include <locale.h>
#include <string>
#include <iostream>
#include "Card.h"
#include "Deck.h"
#include "Board.h"

using namespace std;

int main(int argc, char** args) {
	// set up ncurses
	setlocale(LC_ALL, "en_US.UTF-8");
	initscr();
	noecho();
	refresh();
	curs_set(0);
	start_color();

	// Begin playing game
	Board b;
	b.startGame();
	b.startRound();

	// end ncurses
	sleep(10);
	endwin();
	return 0;
}
