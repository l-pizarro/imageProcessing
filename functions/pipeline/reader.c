#include "../imageProcessing/image_processing.h"
#include "../general/general_functions.h"
#define READ 0
#define WRITE 1

void read_images(int images_to_read)
{
    for (int i=1; i <= images_to_read; i++) {
        // GENERATE THE FILE NAMES
        char filename[100];
        sprintf(filename, "testImages/imagen_%d.png", i);

        FILE * image_readed;
        image_readed = fopen (filename, "r");
        
        if (!image_readed) {
            printf("Error! no abrí archivo");
            abort();
        }

        // SEND FILENAME
        write(STDOUT_FILENO, filename, 100);

        // CREATE THE IMAGE
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
        png_init_io (image->png_ptr, image_readed);
        png_read_png (image->png_ptr, image->info_ptr, 0, 0);
        png_get_IHDR (image->png_ptr, image->info_ptr, & image->width, & image->height, & image->bit_depth,
              & image->color_type, & image->interlace_method, & image->compression_method,
              & image->filter_method);

        image->rows = png_get_rows (image->png_ptr, image->info_ptr);

        // TRANSFORM IMAGE TO MATRIX
        int** image_matrix = imageToInt(image);
        int rows = image->height;
        int cols = png_get_rowbytes (image->png_ptr, image->info_ptr);


        // SEND THE IMAGE TO CONVOLVER
        //First: Send the rows
        char rows_string[50];
        sprintf(rows_string, "%d", rows);
        write(STDOUT_FILENO, rows_string, 50);
        //Second: Send the columns
        char cols_string[50];
        sprintf(cols_string, "%d", cols);
        write(STDOUT_FILENO, cols_string, 50);
        //Third: Send the image matrix
        for (int i=0; i<rows; i++)
        {
            char pixel_value[4];
            for (int j=0; j<cols; j++)
            {
                sprintf(pixel_value, "%d", image_matrix[i][j]);
                write(STDOUT_FILENO, pixel_value, 4);
            }
        }
    }
}

void init_reader()
{
    // RECIVE THE ARGUMENTS FROM FATHER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);

    // SPLIT BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);

    // CONNECT WITH CONVOLVER
    int pipe_reader_convolver[2];
    pipe(pipe_reader_convolver);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_reader_convolver[READ], STDIN_FILENO);
        close(pipe_reader_convolver[WRITE]);
        execl("./convolver", NULL);
    }
    else {
        dup2(pipe_reader_convolver[WRITE], STDOUT_FILENO);
        close(pipe_reader_convolver[READ]);

        // PASS THE ARGUMENTS THOUGH PIPELINE
        write(STDOUT_FILENO, buffer, 100);

        // READ IMAGES AND SEND THEM TO CONVOLVER
        read_images(atoi(arguments[0]));
        wait(&pid);
    }
}

int main()
{
    init_reader();
    return 0;
}
