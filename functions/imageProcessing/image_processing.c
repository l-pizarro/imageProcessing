#include "image_processing.h"

//Entradas: No posee entradas
//Funcionamiento: Permite crear una estructura capaz de almacenar un archivo PNG.
//Salidas: ImageStorage* es un puntero a una estructura destinada a almacenar un archivo PNG.
ImageStorage* createImageStorage()
{
    return (ImageStorage*)calloc(1, sizeof(ImageStorage));
}

//Entradas: ImageStorage* puntero a una estructura con una imagen almacenada en formato PNG.
//Funcionamiento: Recorre la imagen PNG y la transforma en una matriz de enteros, donde cada celda está en RGB.
//Salidas: int** matriz de enteros que representa a la imagen en PNG.
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

//Entradas: float** classified_image -> corresponde a la imagen ya clasificada.
//          int rows -> cantidad de filas de la imagen clasificada.
//          int columns -> cantidad de columnas de la imagen clasificada.
//          char* imageName -> nombre de la imagen analizada.
//          int isBlack -> bandera que indica el resultado del análisis. 1 si es casi negra, 0 sino.
//Funcionamiento: Recibe los resultados de una imagen en particular. Escribe las conclusiones en 'results.txt'
//Salidas: No retorna.
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

//Entradas: float** pooled_image -> corresponde a la imagen a clasificar.
//          int rows -> cantidad de filas de la imagen a clasificar.
//          int columns -> cantidad de columnas de la imagen a clasificar.
//          int threshold -> valor que indica el umbral de negrura, sobre el cual clasificar la imagen
//          char* imageName -> nombre de la imagen a clasificar.
//          int b -> bandera que indica si se debe mostrar el resultado por salida estándar
//Funcionamiento: Recibe una imagen desde 'pooling'. Se recorren sus pixeles y se calcula el porcentaje de pixeles negros.
//Salidas: int que representa la clasificación. 1 si la imagen es casi negra, 0 sino.
int classify(float** pooled_image, int rows, int columns, int threshold, char* imageName, int b)
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

    if (((blackPixels / (rows * columns * 1.0)) * 100) >= threshold) isBlack = 1;

    if (b == 1){
        if (isBlack == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


//Entradas: float** rectificated_matrix -> corresponde a la imagen a realizar 'pooling'.
//          int rows -> cantidad de filas de la imagen a realizar 'pooling'.
//          int columns -> cantidad de columnas de la imagen a realizar 'pooling'.
//          int threshold -> valor que indica el umbral de negrura, sobre el cual clasificar la imagen en la siguiente etapa.
//          char* imageName -> nombre de la imagen a realizar 'pooling'.
//          int b -> bandera que indica si se debe mostrar el resultado por salida estándar.
//Funcionamiento: Utilizando una matriz de 2x2, se buscar el mayor de entre 4 valores, para crear una matriz nueva con estos
//                valores.
//Salidas: flaot** matriz de flotantes que representan la imagen con los mayores números de cada sección.
// float** pooling(float** rectificated_matrix, int rows, int columns, int threshold, char* imageName, int b)
// {
//     float** pooled_image = (float**)calloc(floor(rows/2), sizeof(float*));
    
//     for (int i = 0; i < floor(rows/2); i++)
//     {
//         pooled_image[i] = (float*)calloc(floor(columns/2), sizeof(float));
//     }

//     if (rows % 2 != 0) rows--;
//     if (columns % 2 != 0) columns--;

//     for (int i = 0; i < rows; i = i + 2)
//     {
//         for (int j = 0; j < columns; j = j + 2)
//         {
//             float max = rectificated_matrix[i][j];
//             if (rectificated_matrix[i][j+1] > max)      max = rectificated_matrix[i][j+1];
//             if (rectificated_matrix[i+1][j] > max)      max = rectificated_matrix[i+1][j];
//             if (rectificated_matrix[i+1][j+1] > max)    max = rectificated_matrix[i+1][j+1];
//             pooled_image[i/2][j/2] = max;
//         }
//     }

//     return pooled_image;
// }


//Entradas: float** filtered_matrix -> corresponde a la imagen a rectificar.
//          int rows -> cantidad de filas de la imagen a rectificar.
//          int columns -> cantidad de columnas de la imagen a rectificar.
//          int threshold -> valor que indica el umbral de negrura, sobre el cual clasificar la imagen en la siguiente etapa.
//          char* imageName -> nombre de la imagen a rectificar.
//          int b -> bandera que indica si se debe mostrar el resultado por salida estándar.
//Funcionamiento: Se recorre la imagen. Si los valores son negativos, se establecen a 0.
//Salidas: flaot** matriz de flotantes que representan la imagen rectificada.
// float** rectification(float** filtered_matrix, int rows, int columns, int threshold, char* imageName, int b)
// {
//     for (int i = 0; i < rows; i++)
//     {
//         for (int j = 0; j < columns; j++)
//         {
//             if (filtered_matrix[i][j] < 0)
//             {
//                 filtered_matrix[i][j] = 0;
//             }
//         }
//     }

//     return filtered_matrix;
// }

//Entradas: float** image -> corresponde a la representación de la imagen en PNG.
//          int rows -> cantidad de filas de la imagen a realizar convolución.
//          int columns -> cantidad de columnas de la imagen a realizar convolución.
//          char* filename -> corresponde al archivo con la matriz de convolución.
//          int threshold -> valor que indica el umbral de negrura, sobre el cual clasificar la imagen en la siguiente etapa.
//          char* imageName -> nombre de la imagen a realizar convolución.
//          int b -> bandera que indica si se debe mostrar el resultado por salida estándar.
//Funcionamiento: Utilizando una matriz de 2x2, se buscar el mayor de entre 4 valores, para crear una matriz nueva con estos
//                valores.
//Salidas: flaot** matriz de flotantes que representan la imagen con los mayores números de cada sección.
// float** applyConvolution(int** image, int rows, int columns, char* filename, int threshold, char* imageName, int b)
// {    
//     int** conv_matrix = (int**)calloc(3, sizeof(int*));

//     for (int i=0; i<3; i++)
//     {
//         conv_matrix[i] = (int*)calloc(3, sizeof(int));
//     }

//     FILE* file_matrix = fopen(filename, "r");

//     if (! file_matrix){
//         perror("Error opening file. Quitting...");
        
//         for (int i = 0; i < 3; i++)
//         {
//             free(conv_matrix[i]);
//         }
//         free(conv_matrix);
//         exit(1);
//     }

//     int row = 0;

//     while (! feof(file_matrix))
//     {
//         int a, b, c;
//         fscanf(file_matrix, "%d %d %d", &a, &b, &c);
//         conv_matrix[row][0] = a;
//         conv_matrix[row][1] = b;
//         conv_matrix[row][2] = c;
//         row++;
//     }

//     fclose(file_matrix);

//     float** filtered_matrix;
//     filtered_matrix = (float**)calloc(rows , sizeof(float*));


//     for (int i = 0; i < rows; i++)
//     {
//         filtered_matrix[i] = (float*)calloc(columns, sizeof(float));
//     }

//     for (int i = 1; i < rows - 1; i++)
//     {
//         for (int j = 1; j < columns - 1; j++)
//         {
//             float value = 0;
//             value = (image[i-1][j-1] * conv_matrix[0][0] + image[i-1][j] * conv_matrix[0][1] + image[i-1][j+1] * conv_matrix[0][2] + image[i][j-1] * conv_matrix[1][0] + image[i][j] * conv_matrix[1][1] + image[i][j+1] * conv_matrix[1][2] + image[i+1][j-1] * conv_matrix[2][0] + image[i+1][j] * conv_matrix[2][1] + image[i+1][j+1] * conv_matrix[2][2]) / 9;
//             filtered_matrix[i][j] = value;
//         }
//     }

//     return filtered_matrix;
// }
