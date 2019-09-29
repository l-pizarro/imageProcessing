#include "../imageProcessing/image_processing.h"
#include "../general/general_functions.h"
#define READ 0
#define WRITE 1

void init_convolver()
{
    // RECIVE THE ARGUMENTS FROM READER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);
    printf("CONVOLVER RECIEVE BUFFER: %s", buffer);

    // SPLIT THE BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);
    int cvalue = atoi(arguments[0]);
    int nvalue = atoi(arguments[1]);
    int bflag   = atoi(arguments[3]);

    for (int image_to_read=0; image_to_read<cvalue; image_to_read++) {
        // RECIVE THE FILENAME FROM READER
        char filename[100];
        read(STDIN_FILENO, filename, 100);
        printf("CONVOLVER RECIEVE FILENAME: %s\n", filename);

        // RECIVE THE ROWS
        char rows[50];
        read(STDIN_FILENO, rows, 50);
        printf("CONVOLVER RECIEVE ROWS: %s\n", rows);

        // RECIVE THE COLUMNS
        char cols[50];
        read(STDIN_FILENO, cols, 50);
        printf("CONVOLVER RECIEVE COLS: %s\n", cols);

        // RECIVE THE IMAGE
        int** image_matrix = (int**)calloc(atoi(rows), sizeof(int*));
        for (int i=0; i<atoi(rows); i++)
        {
            image_matrix[i] = (int*)calloc(atoi(cols), sizeof(int*));
            char pixel_value[4];
            for (int j=0; j<atoi(cols); j++)
            {
                read(STDIN_FILENO, pixel_value, 4);
                image_matrix[i][j] = atoi(pixel_value);
            }
        }

        // APPLY CONVOLUTION
        applyConvolution(image_matrix, atoi(rows), atoi(cols), arguments[2], nvalue, filename, bflag);
    }


    // CONNECT WITH RECTIFIER
    // int pipe_convolver_rectifier[2];
    // pipe(pipe_convolver_rectifier);

    // int pid = fork();

    // if (pid == 0) {
    //     dup2(pipe_convolver_rectifier[READ], STDIN_FILENO);
    //     close(pipe_convolver_rectifier[WRITE]);
    //     execl("./rectifier", NULL);
    // }
    // else {
    //     dup2(pipe_convolver_rectifier[WRITE], STDOUT_FILENO);
    //     close(pipe_convolver_rectifier[READ]);
    //     write(STDOUT_FILENO, "It's a msg from convolver\n", 27);
    //     wait(&pid);
    // }
}


int main()
{
    init_convolver();
    return 0;
}
