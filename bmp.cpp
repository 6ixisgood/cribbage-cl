#include <ncursesw/curses.h>
#include "bmp.h"
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