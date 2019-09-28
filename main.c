#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

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

int** imageToInt(ImageStorage* image){
    png_bytepp rows = png_get_rows (image->png_ptr, image->info_ptr);

    int rowbytes;
    rowbytes = png_get_rowbytes (image->png_ptr, image->info_ptr);

    int** image_matrix;
    image_matrix = (int**)calloc(image->height, sizeof(int*));
    for (int i = 0; i < image->height; i++) image_matrix[i] = (int*)calloc(rowbytes, sizeof(int));

    for (int i = 0; i < image->height; i++) {
        png_bytep row;
        row = rows[i];
        for (int j = 0; j < rowbytes; j++) {
            png_byte pixel;
            pixel = row[j];
            
            image_matrix[i][j] = pixel;

        }
    }

    return image_matrix;
}

// With pipes, the function would be without parameters.
void applyConvolution(ImageStorage* imageStructure){
    // Here I assume the data coming from the pipes.
    // char* filename is the name of the file that contains the 3x3 matrix with the convolution rules.
    // int rows, columns. The dimensions of the image to process.
    // int** image. The image to be filtered by the convolution matrix.


    int** image = imageToInt(imageStructure); //This should come through pipe
    int rows, columns; // This should come through pipe

    rows = imageStructure->height;
    columns = png_get_rowbytes (imageStructure->png_ptr, imageStructure->info_ptr);


    // THE COMMENTED LINES BELOW ARE WERE MADE ONLY FOR TESTING
    // printf("################ PRINT DE LA IMAGEN filas %d Columnas%d ################\n", rows, columns);

    // for (int i = 0; i < rows; i++){
    //     for (int j = 0; j < columns; j++) printf("%d ", image[i][j]);
    //     printf("\n");
    // }

    // printf("################ FIN PRINT DE LA IMAGEN ################\n");

    int** conv_matrix;
    conv_matrix = (int**)calloc(3, sizeof(int*));

    for (int i=0; i<3; i++) conv_matrix[i] = (int*)calloc(3, sizeof(int));

    FILE* file_matrix = fopen("test.txt", "r"); // test.txt should be replaced by 'filename', coming from a pipe.

    if (! file_matrix){
        perror("Error opening file. Quitting...");
        
        for (int i = 0; i < 3; i++) free(conv_matrix[i]);
        free(conv_matrix);
        exit(1);
    }

    int row = 0;

    while (! feof(file_matrix)){
        int a, b, c;

        fscanf(file_matrix, "%d %d %d", &a, &b, &c);

        conv_matrix[row][0] = a;
        conv_matrix[row][1] = b;
        conv_matrix[row][2] = c;
        row++;
    }

    fclose(file_matrix);

    int** filtered_matrix;
    filtered_matrix = (int**)calloc(rows , sizeof(int*));
    for (int i = 0; i < rows; i++) filtered_matrix[i] = (int*)calloc(columns, sizeof(int));

    for (int i = 1; i < rows - 1; i++){
        for (int j = 1; j < columns - 1; j++){
            float value = 0;
            value = (image[i-1][j-1] * conv_matrix[0][0] + image[i-1][j] * conv_matrix[0][1] + image[i-1][j+1] * conv_matrix[0][2] + image[i][j-1] * conv_matrix[1][0] + image[i][j] * conv_matrix[1][1] + image[i][j+1] * conv_matrix[1][2] + image[i+1][j-1] * conv_matrix[2][0] + image[i+1][j] * conv_matrix[2][1] + image[i+1][j+1] * conv_matrix[2][2]) / 9;
            filtered_matrix[i][j] = value;
        }
    }

    // return filtered_matrix; //This should be sent to third stage of pipeline

    // THE NEXT LINES ARE MADE ONLY FOR TESTING
    // printf("################ PRINT DE LA CONVOLUCIÓN ################\n");
    // for (int i = 0; i < rows; i++){
    //     for (int j = 0; j < columns; j++) printf("%d ", filtered_matrix[i][j]);
    //     printf("\n");
    // }
    // printf("################ FIN PRINT DE LA CONVOLUCIÓN ################\n");
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

        applyConvolution(image);

    }

    return 0;
}