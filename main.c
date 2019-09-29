#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

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

void writter(float** classified_image, int rows, int columns, char* imageName, int isBlack){
    FILE* f;
    f = fopen("results.txt", "a");

    if (! f){
        perror("Error creando imagen.");
        exit(2);
    }

    fprintf(f, "############## RESULTADOS DE LA IMAGEN %s ##############\n\n", imageName);

    if (isBlack == 1)
        fprintf(f, "YES, %s is nearly black.\n\n", imageName);
    else
        fprintf(f, "NO, %s isn't nearly black.\n\n", imageName);

    fprintf(f, "LA MATRIZ SOBRE LA CUAL SE REALIZÓ EL ANÁLISIS ES LA SIGUIENTE: \n\n");


    for (int i = 0; i < rows; i++){
        for (int j = 0; j < columns; j++) fprintf(f, "%f ", classified_image[i][j]);
        fprintf(f, "\n");
    }

    fprintf(f, "############## FIN RESULTADOS DE LA IMAGEN %s ##############\n\n", imageName);

    fclose(f);
    
}

// With pipes, the function would be without parameters. This is the fifth stage of the pipeline
void classify(float** pooled_image, int rows, int columns, int threshold, char* imageName, int b){
    int blackPixels = 0;

    for (int i = 0; i < rows; i++) for (int j = 0; j < columns; j++) if (pooled_image[i][j] == 0) blackPixels++;

    int isBlack = 0;

    if (floor(blackPixels / (rows * columns)) < threshold) isBlack = 1;

    //Now the imageName and the 'isBlack' flag must be sent to the sixth stage of the pipeline
    if (b == 1){
        if (isBlack == 1)
            printf("YES, %s is nearly black.\n", imageName);
        else
            printf("NO, %s isn't nearly black.\n", imageName);
    }

    //Now the imageName, the 'isBlack' flag, and the pooled_image must be sent to the sixth and last stage of pipeline
    writter(pooled_image, rows, columns, imageName, isBlack);
}

// With pipes, the function would be without parameters. This is the fourth stage of the pipeline
// IMPORTANT: I'LL ASSUME THAT THE POOLING MATRIX WILL ALWAYS BE A 2x2 MATRIX, BECAUSE ISN'T SPECIFIED
void pooling(float** rectificated_matrix, int rows, int columns, int threshold, char* imageName, int b){
    float** pooled_image;
    pooled_image = (float**)calloc(floor(rows/2), sizeof(float*));
    
    for (int i = 0; i < floor(rows/2); i++) pooled_image[i] = (float*)calloc(floor(columns/2), sizeof(float));

    if (rows % 2 != 0) rows--;
    if (columns % 2 != 0) columns--;

    for (int i = 0; i < rows; i = i + 2){
        for (int j = 0; j < columns; j = j + 2){
            float max = rectificated_matrix[i][j];

            if (rectificated_matrix[i][j+1] > max) max = rectificated_matrix[i][j+1];
            if (rectificated_matrix[i+1][j] > max) max = rectificated_matrix[i+1][j];
            if (rectificated_matrix[i+1][j+1] > max) max = rectificated_matrix[i+1][j+1];

            pooled_image[i/2][j/2] = max;
        }
    }

    // now pooled_image should be sent to the 5th stage of the pipeline.
    classify(pooled_image, floor(rows/2), floor(columns/2), threshold, imageName, b);
}

// With pipes, the function would be without parameters. This is the third stage of the pipeline
void rectification(float** filtered_matrix, int rows, int columns, int threshold, char* imageName, int b){
    for (int i = 0; i < rows; i++) for (int j = 0; j < columns; j++) if (filtered_matrix[i][j] < 0) filtered_matrix[i][j] = 0;

    pooling(filtered_matrix, rows, columns, threshold, imageName, b);
    // now filtered_matrix should be sent to 4th stage of pipeline.
}

// With pipes, the function would be without parameters. This is the second stage of the pipeline.
void applyConvolution(int** image, int rows, int columns, char* filename, int threshold, char* imageName, int b){
    
    // REQUIRED DATA FROM PIPES.
    // char* filename is the name of the file that contains the 3x3 matrix with the convolution rules.
    // int rows, columns. The dimensions of the image to process.
    // int** image. The image to be filtered by the convolution matrix.

    int** conv_matrix;
    conv_matrix = (int**)calloc(3, sizeof(int*));

    for (int i=0; i<3; i++) conv_matrix[i] = (int*)calloc(3, sizeof(int));


    printf("filename: %s\n", filename);
    FILE* file_matrix = fopen(filename, "r");

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

    float** filtered_matrix;
    filtered_matrix = (float**)calloc(rows , sizeof(float*));


    for (int i = 0; i < rows; i++) filtered_matrix[i] = (float*)calloc(columns, sizeof(float));

    for (int i = 1; i < rows - 1; i++){
        for (int j = 1; j < columns - 1; j++){
            float value = 0;
            value = (image[i-1][j-1] * conv_matrix[0][0] + image[i-1][j] * conv_matrix[0][1] + image[i-1][j+1] * conv_matrix[0][2] + image[i][j-1] * conv_matrix[1][0] + image[i][j] * conv_matrix[1][1] + image[i][j+1] * conv_matrix[1][2] + image[i+1][j-1] * conv_matrix[2][0] + image[i+1][j] * conv_matrix[2][1] + image[i+1][j+1] * conv_matrix[2][2]) / 9;
            filtered_matrix[i][j] = value;
        }
    }

    rectification(filtered_matrix, rows, columns, threshold, imageName, b);
    // return filtered_matrix; //This should be sent to third stage of pipeline
}

int main(int argc, char *argv[]) {
    char    mfile[50];  //Maybe we should find a cleaner way to do this
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