#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

typedef struct imageStorage
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;
    png_bytepp rows;
} ImageStorage;

ImageStorage* createImageStorage();

int** imageToInt(ImageStorage* image);

void writter(float** classified_image, int rows, int columns, char* imageName, int isBlack);

int classify(float** pooled_image, int rows, int columns, int threshold, char* imageName, int b);

float** pooling(float** rectificated_matrix, int rows, int columns, int threshold, char* imageName, int b);

float** rectification(float** filtered_matrix, int rows, int columns, int threshold, char* imageName, int b);

float** applyConvolution(int** image, int rows, int columns, char* filename, int threshold, char* imageName, int b);