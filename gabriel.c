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

int main(int argc, char *argv[]) {
    char    mfile[50];  
    int     cvalue   = 0;
    int     n   = 0;
    int     b   = 0;

    int c;

    while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
        switch (c) {
            case 'c':
                sscanf(optarg, "%d", &cvalue);
                break;
            case 'm':
                strcpy(mfile, optarg);
                break;
            case 'n':
                sscanf(optarg, "%d", &n);
                break;
            case 'b':
                b = 1;
                break;
            case '?':
                if (optopt == 'h') {
                    fprintf(stderr, "Opcion -%c requiere un argumento.\n", optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Opcion desconocida '-%c'.\n", optopt);
                }
                else {
                    fprintf(
                            stderr,
                            "Opción con caracter desconocido '\\x%x'.\n",
                            optopt
                        );
                }
            default:
                abort();
        }
    }

    printf("nombre del archivo convolución: %s\n", mfile);
    printf("valor de c: %d\n", cvalue);
    printf("valor de n: %d\n", n);
    printf("valor de b: %d\n", b);

    //Here begins the cycle to read images.

    for(int i=1; i<=cvalue; i++){
        char aux1[30] = "testImages/imagen_";
        char filename[35];
        char iAsString[100];
        sprintf(iAsString, "%d", i);

        strcpy(filename, strcat(strcat(aux1, iAsString), ".png"));

        printf("Archivo a analizar: %s \n", filename);
        
        FILE * fp;
        fp = fopen (filename, "r");
        
        if (! fp) {
            printf("Error! no abrí archivo");
            abort();
        }
        
        ImageStorage*  image = createImageStorage(); //Structure creation to save image.

        image->png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        
        if (! image->png_ptr) {
            printf("Error! no hice el png ptr");
            abort();
        }
        
        image->info_ptr = png_create_info_struct (image->png_ptr);
        
        if (! image->png_ptr) {
            printf("Error! no hice el info ptr");
            abort();
        }

        png_init_io (image->png_ptr, fp);
        png_read_png (image->png_ptr, image->info_ptr, 0, 0);
        png_get_IHDR (image->png_ptr, image->info_ptr, & image->width, & image->height, & image->bit_depth,
              & image->color_type, & image->interlace_method, & image->compression_method,
              & image->filter_method);
        
        image->rows = png_get_rows (image->png_ptr, image->info_ptr);

        int** image_matrix = imageToInt(image);
        int rows, columns;
        rows = image->height;
        columns = png_get_rowbytes (image->png_ptr, image->info_ptr);

        applyConvolution(image_matrix, rows, columns, mfile, n, filename, b);
    }

    return 0;
}
