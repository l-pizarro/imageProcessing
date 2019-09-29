#include "../imageProcessing/image_processing.h"
#include "../general/general_functions.h"
#define READ 0
#define WRITE 1

void classify_images(int cvalue, int nvalue, char *mvalue, int bflag)
{
    for (int image_to_clasify=0; image_to_clasify<cvalue; image_to_clasify++)
    {
        // RECIVE THE ROWS
        char rows[50];
        read(STDIN_FILENO, rows, 50);

        // RECIVE THE COLUMNS
        char cols[50];
        read(STDIN_FILENO, cols, 50);

        // RECIVE THE POOLED IMAGE
        float** pooled_matrix = (float**)calloc(atoi(rows), sizeof(float*));
        for (int i=0; i<atoi(rows); i++)
        {
            pooled_matrix[i] = (float*)calloc(atoi(cols), sizeof(float));
            char pixel_value[20];
            for (int j=0; j<atoi(cols); j++)
            {
                read(STDIN_FILENO, pixel_value, 20);
                pooled_matrix[i][j] = atof(pixel_value);
            }
        }

        // RECIVE THE FILENAME
        char filename[100];
        read(STDIN_FILENO, filename, 100);

        // CLASIFY THE IMAGE
        int clasification = classify(pooled_matrix, atoi(rows), atoi(cols), nvalue, filename, bflag);

        char class_string[2];
        sprintf(class_string, "%d", clasification);

        //SEND THE WRITER THE CLASIFICATION AND FILENAME
        write(STDOUT_FILENO, class_string, 2);
        write(STDOUT_FILENO, filename, 100);

    }
}

void init_clasifier()
{
    // RECIVE THE ARGUMENTS FROM POLLER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);

    // SPLIT THE BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);
    int cvalue = atoi(arguments[0]);
    int nvalue = atoi(arguments[1]);
    int bflag   = atoi(arguments[3]);

    // CONNECT WITH WRITER
    int pipe_clasifier_write[2];
    pipe(pipe_clasifier_write);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_clasifier_write[READ], STDIN_FILENO);
        close(pipe_clasifier_write[WRITE]);
        execl("./writer", NULL);
    }
    else {
        dup2(pipe_clasifier_write[WRITE], STDOUT_FILENO);
        close(pipe_clasifier_write[READ]);

        // PASS THE ARGUMENTS THOUGH PIPELINE
        write(STDOUT_FILENO, buffer, 100);

        // CLASIFY IMAGES AND SEND THEM TO WRITER
        classify_images(cvalue, nvalue, arguments[2], bflag);

        wait(&pid);
    }
}


int main()
{
    init_clasifier();
    return 0;
}
