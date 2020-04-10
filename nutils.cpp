#include "nutils.h"

using namespace std;

// function to read in info from given xmp file 
bitmap* xmpToBitmap(int width, int height, unsigned char* data) {
    bitmap *bm = new bitmap;
    
    // set width and height values
    bm->width = width;
    bm->height = height;

    // set the size of the data
    bm->data = new unsigned int*[width];
    for (int i=0; i<width; i++) {
        bm->data[i] = new unsigned int[height];
    }

    // parse through the original data and place a 0 or 1 for each pixel
    int byte = 0;
    for (int y = 0; y < height; y++) {
        for (int x=0; x < width;) {
            int b = data[byte++];
            for (int z=0; z<8 && x<width; z++) {
                unsigned int c = (b & (1 << z)) ? 0 : 1;
                bm->data[x++][y] = c;
            }
        }
    }
    return bm;
}

/**
 * Draws card in given window at given position
 */
void displayCardAt(WINDOW * win, Card c, int x, int y) {
	// create color pairs and set
	init_pair(2, COLOR_RED, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_BLACK, COLOR_RED);
	if (c.suit().compare("H") == 0|| c.suit().compare("D") == 0) {
		wattron(win, COLOR_PAIR(2));
	} else if (c.suit().compare("C") == 0 || c.suit().compare("S") == 0){
		wattron(win, COLOR_PAIR(3));
	} else {
		wattron(win, COLOR_PAIR(4));
	}

	// draw card
	if (c.value() > 0) {
		string space = " ";
		if (c.toString().size() > 2) space = "";
		mvwprintw(win, y+0, x, ""); waddch(win, ACS_ULCORNER); waddch(win, ACS_HLINE);
			waddch(win, ACS_HLINE); waddch(win, ACS_HLINE); waddch(win, ACS_URCORNER);
		mvwprintw(win, y+1, x, ""); waddch(win, ACS_VLINE); 
			wprintw(win, "%s%s", c.toString().c_str(), space.c_str()); waddch(win, ACS_VLINE);
		mvwprintw(win, y+2, x, ""); waddch(win, ACS_VLINE); 
			wprintw(win, "   "); waddch(win, ACS_VLINE);
		mvwprintw(win, y+3, x, ""); waddch(win, ACS_VLINE); 
			wprintw(win, "%s%s", space.c_str(), c.toString().c_str()); waddch(win, ACS_VLINE);
		mvwprintw(win, y+4, x, ""); waddch(win, ACS_LLCORNER); waddch(win, ACS_HLINE);
			waddch(win, ACS_HLINE); waddch(win, ACS_HLINE); waddch(win, ACS_LRCORNER);
	} else {
		// draw back side of card
		mvwprintw(win, y+0, x, "* * *");
		mvwprintw(win, y+1, x, " * * ");
		mvwprintw(win, y+2, x, "* * *");  
		mvwprintw(win, y+3, x, " * * "); 
		mvwprintw(win, y+4, x, "* * *"); 
	}

	// turn off color pair
	if (c.suit().compare("H") == 0|| c.suit().compare("D") == 0) {
		wattroff(win, COLOR_PAIR(2));
	} else if (c.suit().compare("C") == 0 || c.suit().compare("S") == 0){
		wattroff(win, COLOR_PAIR(3));
	} else {
		wattroff(win, COLOR_PAIR(4));
	}
}