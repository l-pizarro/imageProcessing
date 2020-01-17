#include "../general/general_functions.h"
#include "../general/barrier_implementation.h"
#include "../imageProcessing/image_processing.h"
#define READ 0
#define WRITE 1

pthread_barrier_t* barriers;
int** matrix_buffer;
float** finalMatrix;
int img_to_read = 1;
int minLevel = 0;
int underLevel = 0;
int total = 0;

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

void reader(ThreadContext* thread){
    int z = 0;
    for (int i = thread->identifier * thread->rowsToRead; i < (thread->identifier + 1) * thread->rowsToRead, z < thread->rowsToRead; i++, z++){
        for (int j = 0; j < thread->colsAmount; j++){
            thread->rowsToWork[z][j] = matrix_buffer[i][j];
        }
    }
}

float** applyConvolution(ThreadContext* thread)
{    
    int** conv_matrix = (int**)calloc(3, sizeof(int*));

    for (int i=0; i<3; i++)
    {
        conv_matrix[i] = (int*)calloc(3, sizeof(int));
    }

    FILE* file_matrix = fopen(thread->filter_filename, "r");

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
    filtered_matrix = (float**)calloc(thread->rowsToRead , sizeof(float*));


    for (int i = 0; i < thread->rowsToRead; i++)
    {
        filtered_matrix[i] = (float*)calloc(thread->colsAmount, sizeof(float));
    }

    for (int i = 1; i < thread->rowsToRead - 1; i++)
    {
        for (int j = 1; j < thread->colsAmount - 1; j++)
        {
            float value = 0;
            value = (thread->rowsToWork[i-1][j-1] * conv_matrix[0][0] + thread->rowsToWork[i-1][j] * conv_matrix[0][1] + thread->rowsToWork[i-1][j+1] * conv_matrix[0][2] + thread->rowsToWork[i][j-1] * conv_matrix[1][0] + thread->rowsToWork[i][j] * conv_matrix[1][1] + thread->rowsToWork[i][j+1] * conv_matrix[1][2] + thread->rowsToWork[i+1][j-1] * conv_matrix[2][0] + thread->rowsToWork[i+1][j] * conv_matrix[2][1] + thread->rowsToWork[i+1][j+1] * conv_matrix[2][2]) / 9;
            filtered_matrix[i][j] = value;
        }
    }

    return filtered_matrix;
}

float** rectification(ThreadContext* thread, float** convolvedMatrix){
    for (int i = 0; i < thread->rowsToRead; i++){
        for (int j = 0; j < thread->colsAmount; j++){
            if (convolvedMatrix[i][j] < 0){
                convolvedMatrix[i][j] = 0;
            }
        }
    }

    return convolvedMatrix;
}

float findMax(float* array, int len) {
    float max = array[0];
    for (int i = 0; i < len; i++) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

float** pooling(ThreadContext* thread, float** rectificatedMatrix, int* pooledRows, int* pooledCols){
    int rows = thread->rowsToRead/3;
    int cols = thread->colsAmount/3;
    
    // Add one more column if its amounth is not multiple of 3
    if (thread->colsAmount%3 != 0) {
        cols = (thread->colsAmount/3) + 1;
    }

    *pooledCols = cols;
    *pooledRows = rows;

    float** pooledSubmatrix = (float**)calloc(rows, sizeof(float*));
    
    for (int i = 0; i < cols; i++) {
        pooledSubmatrix[i] = (float*)calloc(cols, sizeof(float));
    }

    // Find the max value for every 3by3 matrix in rectificatedMatrix
    // This solution can be improved, but for now, this is the fast code: Put all values on a array and find it 
    for (int rowIter = 0; rowIter < rows; rowIter++) {
        for (int colIter = 0; colIter < cols; colIter++) {
            float* nineValues = (float*)calloc(9, sizeof(float));
            nineValues[0] = rectificatedMatrix[(rowIter*3) + 0][(colIter*3 + 0) % thread->colsAmount];
            nineValues[1] = rectificatedMatrix[(rowIter*3) + 0][(colIter*3 + 1) % thread->colsAmount];
            nineValues[2] = rectificatedMatrix[(rowIter*3) + 0][(colIter*3 + 2) % thread->colsAmount];
            nineValues[3] = rectificatedMatrix[(rowIter*3) + 1][(colIter*3 + 0) % thread->colsAmount];
            nineValues[4] = rectificatedMatrix[(rowIter*3) + 1][(colIter*3 + 1) % thread->colsAmount];
            nineValues[5] = rectificatedMatrix[(rowIter*3) + 1][(colIter*3 + 2) % thread->colsAmount];
            nineValues[6] = rectificatedMatrix[(rowIter*3) + 2][(colIter*3 + 0) % thread->colsAmount];
            nineValues[7] = rectificatedMatrix[(rowIter*3) + 2][(colIter*3 + 1) % thread->colsAmount];
            nineValues[8] = rectificatedMatrix[(rowIter*3) + 2][(colIter*3 + 2) % thread->colsAmount];
            pooledSubmatrix[rowIter][colIter] = findMax(nineValues, 9);
            // free(nineValues);
        }
    }

    return pooledSubmatrix;
}

int countUnderLevel(float** pooledMatrix, int rows, int cols) {
    int pixelsUnderLevel = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (pooledMatrix[i][j] < minLevel) {
                pixelsUnderLevel++;
            }
        }
    }
    return pixelsUnderLevel;
}

void fillFinalMatrix(ThreadContext* thread, float** pooled, int rows, int cols){
    int z = 0;
    for (int i = thread->identifier * rows; i < (thread->identifier + 1) * rows; i++, z++){
        for (int j = 0; j < cols; j++){

            // printf("hebra: %d i:%d, j:%d, cols: %d, rows: %d\n", thread->identifier, i, j, cols, rows);
            // printf("hebra %d: EQUIVALENCIA: final i: %d, final j: %d inicial i: %d inicial j: %d\n", thread->identifier, i, j, z, j);
            finalMatrix[i][j] = pooled[z][j];
        }
    }
    printf("\n");
}  

void* syncThreads(void* param){
    ThreadContext* threadContext = (ThreadContext*) param;
    reader(threadContext);
    pthread_barrier_wait(&barriers[0]);
    float** convolvedMatrix = applyConvolution(threadContext);
    printf("Hebra %d: Ya filtró la imagen\n", threadContext->identifier);
    pthread_barrier_wait(&barriers[1]);
    float** rectificated = rectification(threadContext, convolvedMatrix);
    printf("Hebra %d: Ya rectificó la imagen\n", threadContext->identifier);
    pthread_barrier_wait(&barriers[2]);
    int rows;
    int cols;
    float** pooled = pooling(threadContext, rectificated, &rows, &cols);
    printf("Hebra %d: Ya agrupó la imagen [%d][%d]\n", threadContext->identifier, rows, cols);
    pthread_barrier_wait(&barriers[3]);
    int underLevelPixels = countUnderLevel(pooled, rows, cols);
    underLevel += underLevelPixels;
    total += rows*cols;
    // printf("rows: %d cols: \n", );
    fillFinalMatrix(threadContext, pooled, rows, cols);
    printf("Hebra %d: Encontró %d pixeles bajo el umbral, de un total de %d\n", threadContext->identifier, underLevelPixels, rows*cols);
    pthread_barrier_wait(&barriers[4]);
    return NULL;
    //sync all threads
    //all the other stages.
}

void save_png_to_file (float** bitmap, int width, int height)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;

    char filename[100];
    sprintf(filename, "output/out_%d.png", img_to_read);
    
    fp = fopen (filename, "wb");
    if (! fp) {
        perror("fallo apertura archivo");
        exit(1);
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        perror("fallé creando una struct linea 95");
        exit(1);
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        perror("fallé creando una struct linea 101");
        exit(1);
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        perror("error en esta cosa");
        exit(1);
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  width,
                  height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, height * sizeof (png_byte *));
    for (y = 0; y < height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < width; x++) {
            *row++ = bitmap[x][y];
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    // for (y = 0; y < height; y++) {
    //     png_free (png_ptr, row_pointers[y]);
    // }
    // png_free (png_ptr, row_pointers);
    
    fclose (fp);
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
    minLevel = nvalue;

    int rowsToRead = tvalue / hvalue;
    if (rowsToRead % 3 != 0){
        printf("La relación entre buffer y hebras debe ser múltiplo de 3 (%d/%d no es múltiplo de 3)", tvalue, hvalue);
        exit(1);
    }


    pthread_t* thread_array = (pthread_t*)calloc(hvalue, sizeof(pthread_t));
    barriers = (pthread_barrier_t*) calloc(5, sizeof(pthread_barrier_t));

    for (int i = 0; i < 5; i++)
        pthread_barrier_init(&barriers[i], NULL, hvalue);

    notifyTheFiller(tvalue);

    //The master thread has filled the buffer. Now the slaves threads must go to work.

    finalMatrix = (float**)calloc(rowsToRead, sizeof(float*));
    for (int i = 0; i < rowsToRead; i++){
        finalMatrix[i] = (float*)calloc(tvalue/3, sizeof(float));
    }

    // printf("filas: %d columnas: %d\n", rowsToRead, tvalue/3);

    for (int i = 0; i < hvalue; i++){
        ThreadContext* threadContext = (ThreadContext*)calloc(1, sizeof(ThreadContext));
        threadContext->identifier = i;
        threadContext->rowsToRead = rowsToRead;
        threadContext->colsAmount = tvalue;
        threadContext->rowsToWork = (float**)calloc(rowsToRead, sizeof(float*));
        threadContext->filter_filename = (char*)calloc(100, sizeof(float*));
        strcpy(threadContext->filter_filename, mvalue);
        for (int j = 0; j < rowsToRead; j++)
            threadContext->rowsToWork[j] = (float*)calloc(tvalue, sizeof(float));

        pthread_create(&thread_array[i], NULL, syncThreads, (void*) threadContext);
    }

    for (int i = 0; i < hvalue; i++){
        pthread_join(thread_array[i], NULL);
    }

    save_png_to_file(finalMatrix, tvalue/3, rowsToRead);

    printf("La hebra padre determina que un %f%c de los pixeles está por debajo del umbral\n", (underLevel/(total*1.0))*100, 37);
    if (img_to_read < cvalue){
        img_to_read++;
        init_pipeline(cvalue, hvalue, tvalue, nvalue, mvalue, bflag);
    }

    return;
}

//Entradas: char** -> Corresponde a una 'lista' de palabras divididas.
//          char* buffer -> Corresponde a una frase que se desea separar.
//          int items -> Cantidad de separaciones que se le quiere hacer a la frase
//Funcionamiento: Procedimiento que permite separar una frase con espacios, en 'items' cantidad de palabras.
//Salidas: No retorna.
// void split_buffer(char** destiny, char* buffer, int items)
// {
//     char *str = (char*)calloc(strlen(buffer), sizeof(char));
//     strcpy(str, buffer);
//     char *ptr = strtok(str, " ");

//     for (int i=0; i<items; i++)
//     {
//         destiny[i] = (char*)calloc(strlen(ptr), sizeof(char));
//         strcpy(destiny[i], ptr);
//         ptr = strtok(NULL, " ");
//     }
//     free(str);
// }