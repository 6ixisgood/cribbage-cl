// struct to hold a bitmap file's data
typedef struct bitmapstruct {
    int width, height;
    unsigned int **data;
} bitmap;

// function definitions 
bitmap* xmpToBitmap(int width, int height, unsigned char* data);