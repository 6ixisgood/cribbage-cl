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
#include "LocalBoard.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"
#include "nutils.h"
#include "splash.xbm"

using namespace std;

void drawSplashScreen();

int main(int argc, char** args) {
	// set up ncurses
	setlocale(LC_ALL, "en_US.UTF-8");
	initscr();
	noecho();
	refresh();
	curs_set(0);
	start_color();

	// show splash screen
	drawSplashScreen();
	getch();
	clear();
	refresh();

	// create players
	HumanPlayer p1 = HumanPlayer("You");
	ComputerPlayer p2 = ComputerPlayer("Computer");
	// Begin playing game
	LocalBoard lb = LocalBoard(&p1, &p2);
	Board *b = &lb;
	b->startGameRound();

	// end ncurses
	sleep(10);
	endwin();
	return 0;
}

void drawSplashScreen() {
	bitmap *b = xmpToBitmap(splash_width, splash_height, splash_bits);
	init_pair(0, COLOR_RED, COLOR_BLACK);
	init_pair(1, COLOR_BLACK, COLOR_RED);
	for(int x = 0; x < b->width; x++) {
		for (int y = 0; y < b->height; y++) {
			if (b->data[x][y]) {
				attron(COLOR_PAIR(0));
				mvaddch(y, x, ' ');
				attroff(COLOR_PAIR(0));
			}
			else {
				attron(COLOR_PAIR(1));
				mvaddch(y, x, ' ');
				attroff(COLOR_PAIR(1));
			}
		}
	}
	attron(COLOR_PAIR(0));
	mvprintw(12, 30, "Press Any Key To Start");
	attroff(COLOR_PAIR(0));
	refresh();
}
