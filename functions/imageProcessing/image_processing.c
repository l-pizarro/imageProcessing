#include "image_processing.h"

ImageStorage* createImageStorage()
{
    return (ImageStorage*)calloc(1, sizeof(ImageStorage));
}

int** imageToInt(ImageStorage* image)
{
    png_bytepp  rows            = png_get_rows (image->png_ptr, image->info_ptr);
    int         rowbytes        = png_get_rowbytes (image->png_ptr, image->info_ptr);
    int**       image_matrix    = (int**)calloc(image->height, sizeof(int*));

    for (int i = 0; i < image->height; i++)
    {
        image_matrix[i] = (int*)calloc(rowbytes, sizeof(int));
    }

    for (int i = 0; i < image->height; i++)
    {
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


void writter(float** classified_image, int rows, int columns, char* imageName, int isBlack)
{
    FILE* output_file;
    output_file = fopen("results.txt", "a");

    if (! output_file)
    {
        perror("Error creando imagen.");
        exit(2);
    }

    fprintf(output_file, "############## RESULTADOS DE LA IMAGEN %s ##############\n\n", imageName);

    if (isBlack == 1)
    {
        fprintf(output_file, "YES, %s is nearly black.\n\n", imageName);
    }
    else
    {
        fprintf(output_file, "NO, %s isn't nearly black.\n\n", imageName);
    }

    fprintf(output_file, "LA MATRIZ SOBRE LA CUAL SE REALIZÓ EL ANÁLISIS ES LA SIGUIENTE: \n\n");

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < columns; j++) fprintf(output_file, "%f ", classified_image[i][j]);
        fprintf(output_file, "\n");
    }

    fprintf(output_file, "############## FIN RESULTADOS DE LA IMAGEN %s ##############\n\n", imageName);
    fclose(output_file); 
}


void classify(float** pooled_image, int rows, int columns, int threshold, char* imageName, int b)
{
    int blackPixels = 0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (pooled_image[i][j] == 0)
            {
                blackPixels++;
            }
        }
    }

    int isBlack = 0;

    if (floor(blackPixels / (rows * columns)) < threshold)
    {
        isBlack = 1;
    }

    //Now the imageName and the 'isBlack' flag must be sent to the sixth stage of the pipeline
    if (b == 1)
    {
        if (isBlack == 1)
        {
            printf("YES, %s is nearly black.\n", imageName);
        }
        else
        {
            printf("NO, %s isn't nearly black.\n", imageName);
        }
    }

    //Now the imageName, the 'isBlack' flag, and the pooled_image must be sent to the sixth and last stage of pipeline
    writter(pooled_image, rows, columns, imageName, isBlack);
}


// With pipes, the function would be without parameters. This is the fourth stage of the pipeline
// IMPORTANT: I'LL ASSUME THAT THE POOLING MATRIX WILL ALWAYS BE A 2x2 MATRIX, BECAUSE ISN'T SPECIFIED
void pooling(float** rectificated_matrix, int rows, int columns, int threshold, char* imageName, int b)
{
    float** pooled_image = (float**)calloc(floor(rows/2), sizeof(float*));
    
    for (int i = 0; i < floor(rows/2); i++)
    {
        pooled_image[i] = (float*)calloc(floor(columns/2), sizeof(float));
    }

    if (rows % 2 != 0) rows--;
    if (columns % 2 != 0) columns--;

    for (int i = 0; i < rows; i = i + 2)
    {
        for (int j = 0; j < columns; j = j + 2)
        {
            float max = rectificated_matrix[i][j];
            if (rectificated_matrix[i][j+1] > max)      max = rectificated_matrix[i][j+1];
            if (rectificated_matrix[i+1][j] > max)      max = rectificated_matrix[i+1][j];
            if (rectificated_matrix[i+1][j+1] > max)    max = rectificated_matrix[i+1][j+1];
            pooled_image[i/2][j/2] = max;
        }
    }
    // now pooled_image should be sent to the 5th stage of the pipeline.
    classify(pooled_image, floor(rows/2), floor(columns/2), threshold, imageName, b);
}


// With pipes, the function would be without parameters. This is the third stage of the pipeline
void rectification(float** filtered_matrix, int rows, int columns, int threshold, char* imageName, int b)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            if (filtered_matrix[i][j] < 0)
            {
                filtered_matrix[i][j] = 0;
            }
        }
    }

    pooling(filtered_matrix, rows, columns, threshold, imageName, b);
    // now filtered_matrix should be sent to 4th stage of pipeline.
}


void applyConvolution(int** image, int rows, int columns, char* filename, int threshold, char* imageName, int b)
{    
    // REQUIRED DATA FROM PIPES.
    // char* filename is the name of the file that contains the 3x3 matrix with the convolution rules.
    // int rows, columns. The dimensions of the image to process.
    // int** image. The image to be filtered by the convolution matrix.

    int** conv_matrix = (int**)calloc(3, sizeof(int*));

    for (int i=0; i<3; i++)
    {
        conv_matrix[i] = (int*)calloc(3, sizeof(int));
    }

    //printf("filename: %s\n", filename);
    FILE* file_matrix = fopen(filename, "r");

    if (! file_matrix){
        perror("Error opening file. Quitting...");
        
        for (int i = 0; i < 3; i++)
        {
            free(conv_matrix[i]);
        }
        free(conv_matrix);
        exit(1);
    }

    int row = 0;

    while (! feof(file_matrix))
    {
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


    for (int i = 0; i < rows; i++)
    {
        filtered_matrix[i] = (float*)calloc(columns, sizeof(float));
    }

    for (int i = 1; i < rows - 1; i++)
    {
        for (int j = 1; j < columns - 1; j++)
        {
            float value = 0;
            value = (image[i-1][j-1] * conv_matrix[0][0] + image[i-1][j] * conv_matrix[0][1] + image[i-1][j+1] * conv_matrix[0][2] + image[i][j-1] * conv_matrix[1][0] + image[i][j] * conv_matrix[1][1] + image[i][j+1] * conv_matrix[1][2] + image[i+1][j-1] * conv_matrix[2][0] + image[i+1][j] * conv_matrix[2][1] + image[i+1][j+1] * conv_matrix[2][2]) / 9;
            filtered_matrix[i][j] = value;
        }
    }

    rectification(filtered_matrix, rows, columns, threshold, imageName, b);
    // return filtered_matrix; //This should be sent to third stage of pipeline
}
