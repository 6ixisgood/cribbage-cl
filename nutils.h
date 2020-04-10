#ifndef NUTILS_H
#define NUTILS_H

#include <ncursesw/curses.h>
#include <Card.h>

// struct to hold a bitmap file's data
typedef struct bitmapstruct {
    int width, height;
    unsigned int **data;
} bitmap;

// function definitions 
bitmap* xmpToBitmap(int width, int height, unsigned char* data);

// display a card in given window and position
void displayCardAt(WINDOW * win, Card c, int x, int y);

#endif /* NUTILS_H */