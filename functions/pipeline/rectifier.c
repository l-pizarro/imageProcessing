#include "../imageProcessing/image_processing.h"
#include "../general/general_functions.h"
#define READ 0
#define WRITE 1

//Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
//          int nvalue -> Corresponde al umbral de negrura sobre el cual concluir tras realizar los filtrados.
//          char* mvalue -> Corresponde al nombre del archivo que contiene la matriz para realizar la convolución.
//          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
//Funcionamiento: Es la encargada de aplicar la rectificación a la imagen.
//Salidas: No retorna.
void rectifiy_images(int cvalue, int nvalue, char *mvalue, int bflag)
{
    for (int image_to_rectify=0; image_to_rectify<cvalue; image_to_rectify++)
    {
        // RECIVE THE ROWS
        char rows[50];
        read(STDIN_FILENO, rows, 50);

        // RECIVE THE COLUMNS
        char cols[50];
        read(STDIN_FILENO, cols, 50);

        // RECIVE THE CONVOLVED IMAGE
        float** convolved_matrix = (float**)calloc(atoi(rows), sizeof(float*));
        for (int i=0; i<atoi(rows); i++)
        {
            convolved_matrix[i] = (float*)calloc(atoi(cols), sizeof(float));
            char pixel_value[20];
            for (int j=0; j<atoi(cols); j++)
            {
                read(STDIN_FILENO, pixel_value, 20);
                convolved_matrix[i][j] = atof(pixel_value);
            }
        }

        // RECIVE THE FILENAME
        char filename[100];
        read(STDIN_FILENO, filename, 100);

        // RECTIFY THE IMAGE
        float** rectified_matrix = rectification(convolved_matrix, atoi(rows), atoi(cols), nvalue, filename, bflag);

        // SEND THE RECTIFIED IMAGE TO THE POOLER
        //First: Send the rows
        write(STDOUT_FILENO, rows, 50);
        //Second: Send the columns
        write(STDOUT_FILENO, cols, 50);
        //Third: Send the rectified matrix
        for (int i=0; i<atoi(rows); i++)
        {
            char rectified_value[20];
            for (int j=0; j<atoi(cols); j++)
            {
                sprintf(rectified_value, "%f", rectified_matrix[i][j]);
                write(STDOUT_FILENO, rectified_value, 20);
            }
        }
        //Fourth: Send the filename
        write(STDOUT_FILENO, filename, 100);
    }

}

//Entradas: No posee entradas.
//Funcionamiento: Recibe los datos desde el pipe, con tal de iniciar la función de 'pooling'.
//Salidas: No retorna.
void init_rectifier()
{
    // RECIVE THE ARGUMENTS FROM CONVOLVER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);

    // SPLIT THE BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);
    int cvalue = atoi(arguments[0]);
    int nvalue = atoi(arguments[1]);
    int bflag   = atoi(arguments[3]);


    // CONNECT WITH POOLER
    int pipe_rectifier_pooler[2];
    pipe(pipe_rectifier_pooler);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_rectifier_pooler[READ], STDIN_FILENO);
        close(pipe_rectifier_pooler[WRITE]);
        execl("./pooler", NULL);
    }
    else {
        dup2(pipe_rectifier_pooler[WRITE], STDOUT_FILENO);
        close(pipe_rectifier_pooler[READ]);

        // PASS THE ARGUMENTS THOUGH PIPELINE
        write(STDOUT_FILENO, buffer, 100);

        // RECTIFY IMAGES AND SEND THEM TO POOLER
        rectifiy_images(cvalue, nvalue, arguments[2], bflag);
        wait(&pid);
    }
}

//Entradas: No posee entradas.
//Funcionamiento: Función principal.
//Salidas: No retorna.
int main()
{
    init_rectifier();
    return 0;
}
