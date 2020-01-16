// #include "../imageProcessing/image_processing.h"
// #include "../general/general_functions.h"
// #define READ 0
// #define WRITE 1

// //Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
// //          int nvalue -> Corresponde al umbral de negrura sobre el cual concluir tras realizar los filtrados.
// //          char* mvalue -> Corresponde al nombre del archivo que contiene la matriz para realizar la convolución.
// //          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
// //Funcionamiento: Es la encargada de aplicar la convolución a la imagen.
// //Salidas: No retorna.
// void convolve_images(int cvalue, int nvalue, char *mvalue, int bflag)
// {
//     for (int image_to_convolve=0; image_to_convolve<cvalue; image_to_convolve++) {
//         // RECIVE THE FILENAME FROM READER
//         char filename[100];
//         read(STDIN_FILENO, filename, 100);

//         // RECIVE THE ROWS
//         char rows[50];
//         read(STDIN_FILENO, rows, 50);

//         // RECIVE THE COLUMNS
//         char cols[50];
//         read(STDIN_FILENO, cols, 50);

//         // RECIVE THE IMAGE
//         int** image_matrix = (int**)calloc(atoi(rows), sizeof(int*));
//         for (int i=0; i<atoi(rows); i++)
//         {
//             image_matrix[i] = (int*)calloc(atoi(cols), sizeof(int));
//             char pixel_value[4];
//             for (int j=0; j<atoi(cols); j++)
//             {
//                 read(STDIN_FILENO, pixel_value, 4);
//                 image_matrix[i][j] = atoi(pixel_value);
//             }
//         }

//         // APPLY CONVOLUTION
//         float** convolved_matrix = applyConvolution(image_matrix, atoi(rows), atoi(cols), mvalue, nvalue, filename, bflag);

//         // SEND THE CONVOLVED IMAGE TO THE RECTIFIER
//         //First: Send the rows
//         write(STDOUT_FILENO, rows, 50);
//         //Second: Send the columns
//         write(STDOUT_FILENO, cols, 50);
//         //Third: Send the convolved matrix
//         for (int i=0; i<atoi(rows); i++)
//         {
//             char convolved_value[20];
//             for (int j=0; j<atoi(cols); j++)
//             {
//                 sprintf(convolved_value, "%f", convolved_matrix[i][j]);
//                 write(STDOUT_FILENO, convolved_value, 20);
//             }
//         }
//         //Fourth: Send the filename
//         write(STDOUT_FILENO, filename, 100);

//     }
// }

// //Entradas: No posee entradas.
// //Funcionamiento: Recibe los datos desde el pipe, con tal de iniciar la función de convolución.
// //Salidas: No retorna.
// void init_convolver()
// {
//     // RECIVE THE ARGUMENTS FROM POOLER
//     char buffer[100];
//     read(STDIN_FILENO, buffer, 100);

//     // SPLIT THE BUFFER
//     char** arguments = (char**)calloc(4, sizeof(char*));
//     split_buffer(arguments, buffer, 4);
//     int cvalue = atoi(arguments[0]);
//     int nvalue = atoi(arguments[1]);
//     int bflag   = atoi(arguments[3]);

//     // CONNECT WITH RECTIFIER
//     int pipe_convolver_rectifier[2];
//     pipe(pipe_convolver_rectifier);

//     int pid = fork();

//     if (pid == 0) {
//         dup2(pipe_convolver_rectifier[READ], STDIN_FILENO);
//         close(pipe_convolver_rectifier[WRITE]);
//         execl("./rectifier", NULL);
//     }
//     else {
//         dup2(pipe_convolver_rectifier[WRITE], STDOUT_FILENO);
//         close(pipe_convolver_rectifier[READ]);

//         // PASS THE ARGUMENTS THOUGH PIPELINE
//         write(STDOUT_FILENO, buffer, 100);

//         // CONVOLVE IMAGES AND SEND THEM TO RECTIFIER
//         convolve_images(cvalue, nvalue, arguments[2], bflag);

//         wait(&pid);
//     }
// }

//Entradas: No posee entradas.
//Funcionamiento: Función principal.
//Salidas: No retorna.
int main()
{
    // init_convolver();
    return 0;
}
