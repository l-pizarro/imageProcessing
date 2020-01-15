#include "../general/general_functions.h"
#include "../imageProcessing/image_processing.h"
#define READ 0
#define WRITE 1


int** matrix_buffer;
int img_to_read = 1;


//Entradas: int argc -> Corresponde a la cantidad argumentos enviados a través de la entrada estándar
//          char** argv -> Corresponde a un arreglo con los argumentos enviados a través de la entrada estándar
//Funcionamiento: Es la encargada de recibir los datos desde la ejecución del programa, para comenzar el pipeline
//Salidas: No retorna.
void init_program(int argc, char **argv)
{
	int     cvalue = 0;
    int     hvalue = 0;
    int     tvalue = 0;
    int     nvalue = 0;
    int     bflag   = 0;
    char    mvalue[50];

    int command;
    opterr = 0;

    while ((command = getopt(argc, argv, "c:h:t:m:n:b")) != -1) {
        switch (command) {
            case 'c':
                sscanf(optarg, "%d", &cvalue);
                if (cvalue <= 0){
                    perror("Se debe analizar al menos 1 imagen. (El valor de C no puede ser menor a 0)");
                    exit(1);
                }
                break;
            case 'h':
                sscanf(optarg, "%d", &hvalue);
                if (hvalue <= 0){
                    perror("La cantidad de hebras debe ser mayor a 0");
                    exit(1);
                }
                break;
            case 't':
                sscanf(optarg, "%d", &tvalue);
                if (tvalue <= 0 || tvalue % 3 != 0){
                    perror("El valor del buffer no puede ser menor a 0");
                    exit(1);
                }
                break;
            case 'm':
                sscanf(optarg, "%s", mvalue);
                break;
            case 'n':
                sscanf(optarg, "%d", &nvalue);
                if (nvalue < 0 || nvalue > 100){
                    perror("El umbral debe ser un valor entre 0 y 100");
                    exit(1);
                }
                break;
            case 'b':
                bflag = 1;
                break;
            case '?':
                if (optopt == 'c' || optopt == 'm' || optopt == 'n') {
                    fprintf(stderr, "Opcion -%c requiere un argumento.\n", optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Opcion desconocida '-%c'.\n", optopt);
                }
                else {
                    fprintf(stderr, "Opción con caracter desconocido '\\x%x'.\n", optopt);
                }
            default:
                abort();
        }
    }

    if (hvalue > tvalue){
        perror("No pueden haber más hebras que filas en el buffer.");
        exit(1);
    }
	init_pipeline(cvalue, hvalue, tvalue, nvalue, mvalue, bflag);
}

void * fillBuffer(void* tvalue){
    int size = (int) tvalue;

    printf("Tengo que llenar el buffer, con este tamaño: %d\n", size);

    char filename[100];
    sprintf(filename, "testImages/imagen_%d.png", img_to_read);

    FILE * image_readed;
    image_readed = fopen (filename, "r");
    
    if (!image_readed) {
        printf("Error! no abrí archivo");
        abort();
    }

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
    int i = 0;
    int j = 0;

    while (i < size && i < rows){
        j = 0;
        while (j < size && j < cols){
            matrix_buffer[i][j] = image_matrix[i][j];
            j++;
        }
        i++;
    }

    return NULL;
}

void notifyTheFiller(int tvalue){
    matrix_buffer = (int**)calloc(tvalue, sizeof(int*));
    for (int i=0; i<tvalue; i++)
        matrix_buffer[i] = (int*)calloc(tvalue, sizeof(int));

    pthread_t master;

    pthread_create(&master, NULL, fillBuffer, (void *) tvalue);

    pthread_join(master, NULL);
}

void* syncThreads(void* rowsToRead){
    int designatedRows = (int) rowsToRead;
    printf("Yo soy una hebra y debo leer %d filas\n", designatedRows);
    //read
    //sync all threads
    //Convolution
    //sync all threads
    //all the other stages.
}

//Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
//          int nvalue -> Corresponde al umbral de negrura sobre el cual concluir tras realizar los filtrados.
//          char* mvalue -> Corresponde al nombre del archivo que contiene la matriz para realizar la convolución.
//          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
//Funcionamiento: Es la encargada de comenzar el pipeline, entregando los parámetros necesarios para que la primera
//                etapa sea capaz de leer.
//Salidas: No retorna.

void init_pipeline(int cvalue, int hvalue, int tvalue, int nvalue, char* mvalue, int bflag)
{   

    int rowsToRead = tvalue / hvalue;
    if (rowsToRead % 3 != 0){
        printf("La relación entre buffer y hebras debe ser múltiplo de 3 (%d/%d no es múltiplo de 3)", tvalue, hvalue);
        exit(1);
    }

    notifyTheFiller(tvalue);

    //The master thread has filled the buffer. Now the slaves threads must go to work.

    pthread_t* thread_array = (pthread_t*)calloc(hvalue, sizeof(pthread_t));



    for (int i = 0; i < hvalue; i++){
        pthread_create(&thread_array[i], NULL, syncThreads, (void*) rowsToRead);
    }

    for (int i = 0; i < hvalue; i++){
        pthread_join(thread_array[i], NULL);
    }

    for (int i = 0; i < tvalue; i++){
        free(matrix_buffer[i]);
    }
    free(matrix_buffer);

    if (img_to_read < cvalue){
        img_to_read++;
        init_pipeline(cvalue, hvalue, tvalue, nvalue, mvalue, bflag);
    }

    // for (int i = 0; i < tvalue; i++){
    //     for (int j = 0; j < tvalue; j++){
    //         printf("%d ", matrix_buffer[i][j]);
    //     }
    //     printf("\n");
    // }

    return;



    // char buffer[100];
    // sprintf(buffer, "%d %d %s %d\n", cvalue, nvalue, mvalue, bflag);
    // int pipe_father_reader[2];
    // pipe(pipe_father_reader);

    // int pid = fork();

    // if (pid == 0) {
    //     dup2(pipe_father_reader[READ], STDIN_FILENO);
    //     close(pipe_father_reader[WRITE]);
    //     execl("./reader", NULL);
    // }
    // else {
    //     dup2(pipe_father_reader[WRITE], STDOUT_FILENO);
    //     close(pipe_father_reader[READ]);
    //     write(STDOUT_FILENO, buffer, 100);
    //     wait(&pid);
    // }
}

//Entradas: char** -> Corresponde a una 'lista' de palabras divididas.
//          char* buffer -> Corresponde a una frase que se desea separar.
//          int items -> Cantidad de separaciones que se le quiere hacer a la frase
//Funcionamiento: Procedimiento que permite separar una frase con espacios, en 'items' cantidad de palabras.
//Salidas: No retorna.
void split_buffer(char** destiny, char* buffer, int items)
{
    char *str = (char*)calloc(strlen(buffer), sizeof(char));
    strcpy(str, buffer);
    char *ptr = strtok(str, " ");

    for (int i=0; i<items; i++)
    {
        destiny[i] = (char*)calloc(strlen(ptr), sizeof(char));
        strcpy(destiny[i], ptr);
        ptr = strtok(NULL, " ");
    }
    free(str);
}