#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

int imagesWaitingSecondStage = 0;

typedef struct imageStorage {
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
}ImageStorage;

ImageStorage* createImageStorage(){
    return (ImageStorage*)calloc(1, sizeof(ImageStorage));
}

int main(int argc, char *argv[]) {
    char    mfile[50];  //Maybe we should find a cleaner way to do this
    int     cvalue   = 0;
    int     n   = 0;
    int     b   = 0;

    // int     c  = 0;
    // int     n  = 0;

    // char    mvalue[50];

    int c;
    // opterr = 0;

    while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
        switch (c) {
            case 'c':
                sscanf(optarg, "%d", &cvalue);
                break;
            case 'm':
                //Here must go a strcpy. I'll do this later.
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

    printf("valor de c: %d\n", cvalue);
    printf("valor de n: %d\n", n);
    printf("valor de b: %d\n", b);

    for(int i=1; i<=c; i++){
        char aux1[30] = "testImages/imagen_";
        char filename[35];
        char iAsString[100];
        sprintf(iAsString, "%d", i);

        // printf("Imagenes a analizar: %s\n", strcat(strcat(aux1, iAsString), ".png"));

        strcpy(filename, strcat(strcat(aux1, iAsString), ".png"));

        printf("Archivo a analizar: %s \n", filename);
        
        FILE * fp;
        fp = fopen (filename, "r");
        
        if (! fp) {
            printf("Error! no abrí archivo");
            abort();
        }
        
        ImageStorage*  image = createImageStorage();

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

        imagesWaitingSecondStage++;

        pid_t pid;
        int fd[2];
        pipe(fd);

        if (i == 1){
            pid = fork();
        }

        if (pid < 0){
            perror("Error in Fork!");
            exit(1);
        } else if (pid == 0){
            while (1){
                if (imagesWaitingSecondStage > 0){
                    // Request Image through pipe
                    // ConvolutionFunction
                    // ImagesWaiting-- after dispatch the third stage
                }
            }
        } else {
            // ImagesWaitingSecondSage++;
            // Send Image through pipe
        }
    }

    return 0;
}