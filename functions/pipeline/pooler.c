#include "../imageProcessing/image_processing.h"
#include "../general/general_functions.h"
#define READ 0
#define WRITE 1

//Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
//          int nvalue -> Corresponde al umbral de negrura sobre el cual concluir tras realizar los filtrados.
//          char* mvalue -> Corresponde al nombre del archivo que contiene la matriz para realizar la convolución.
//          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
//Funcionamiento: Es la encargada de aplicar el pooling a la imagen.
//Salidas: No retorna.
void pool_images(int cvalue, int nvalue, char *mvalue, int bflag)
{
    for (int image_to_poll=0; image_to_poll<cvalue; image_to_poll++)
    {
        // RECIVE THE ROWS
        char rows[50];
        read(STDIN_FILENO, rows, 50);

        // RECIVE THE COLUMNS
        char cols[50];
        read(STDIN_FILENO, cols, 50);


        // RECIVE THE RECTIFIED IMAGE
        float** rectified_matrix = (float**)calloc(atoi(rows), sizeof(float*));
        for (int i=0; i<atoi(rows); i++)
        {
            rectified_matrix[i] = (float*)calloc(atoi(cols), sizeof(float));
            char pixel_value[20];
            for (int j=0; j<atoi(cols); j++)
            {
                read(STDIN_FILENO, pixel_value, 20);
                rectified_matrix[i][j] = atof(pixel_value);
            }
        }


        // RECIVE THE FILENAME
        char filename[100];
        read(STDIN_FILENO, filename, 100);

        // POLL THE IMAGE
        float** pooled_matrix = pooling(rectified_matrix, atoi(rows), atoi(cols), nvalue, filename, bflag);

        int new_rows = atoi(rows)/2;
        int new_cols = atoi(cols)/2;

        char new_rows_string[50];
        char new_cols_string[50];

        sprintf(new_rows_string, "%d", new_rows);
        sprintf(new_cols_string, "%d", new_cols);

        // SEND THE POOLED IMAGE TO THE POLLER
        //First: Send the rows
        write(STDOUT_FILENO, new_rows_string, 50);
        //Second: Send the columns
        write(STDOUT_FILENO, new_cols_string, 50);
        //Third: Send the rectified matrix
        for (int i=0; i<new_rows; i++)
        {
            char pooled[20];
            for (int j=0; j<new_cols; j++)
            {
                sprintf(pooled, "%f", pooled_matrix[i][j]);
                write(STDOUT_FILENO, pooled, 20);
            }
        }
        //Fourth: Send the filename
        write(STDOUT_FILENO, filename, 100);
    }
}

//Entradas: No posee entradas.
//Funcionamiento: Recibe los datos desde el pipe, con tal de iniciar la función de 'pooling'.
//Salidas: No retorna.
void init_poller()
{
    // RECIVE THE ARGUMENTS FROM RECTIFIER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);

    // SPLIT THE BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);
    int cvalue = atoi(arguments[0]);
    int nvalue = atoi(arguments[1]);
    int bflag   = atoi(arguments[3]);


    //CONNECT WITH CLASIFIER
    int pipe_pooler_clasifier[2];
    pipe(pipe_pooler_clasifier);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_pooler_clasifier[READ], STDIN_FILENO);
        close(pipe_pooler_clasifier[WRITE]);
        execl("./clasifier", NULL);
    }
    else {
        dup2(pipe_pooler_clasifier[WRITE], STDOUT_FILENO);
        close(pipe_pooler_clasifier[READ]);

        // PASS THE ARGUMENTS THOUGH PIPELINE
        write(STDOUT_FILENO, buffer, 100);

        // POOL IMAGES AND SEND THEM TO CLASIFIER
        pool_images(cvalue, nvalue, arguments[2], bflag);
        wait(&pid);
    }
}

//Entradas: No posee entradas.
//Funcionamiento: Función principal.
//Salidas: No retorna.
int main()
{
    init_poller();
    return 0;
}
