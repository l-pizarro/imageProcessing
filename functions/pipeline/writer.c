#include "../general/general_functions.h"

//Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
//          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
//Funcionamiento: Es la encargada de escribir las conclusiones de la imagen, tanto en un archivo 'results.txt', 
//                como por salida estándar, en caso de ser necesario.
//Salidas: No retorna.
void write_results(int cvalue, int bflag) {
    // OUTPUT FILE
    FILE* output_file;
    output_file = fopen("results.txt", "w");


    fprintf(output_file, "|          image          | nearly black |\n");
    fprintf(output_file, "-------------------------------------------\n");
    if (bflag)
    {
        printf("|          image          | nearly black |\n");
        printf("-------------------------------------------\n");
    }

    for (int result_to_write=0; result_to_write<cvalue; result_to_write++)
    {
        // RECIVE THE ARGUMENTS FROM CLASIFIER
        char classification[2];
        read(STDIN_FILENO, classification, 2);
        char filename[100];
        read(STDIN_FILENO, filename, 100);

        int flag = 0;

        if (! strcmp("1", classification))
            flag = 1;

        if (flag)
            fprintf(output_file, "| %s | %s |\n",filename, "      YES     ");
        else
            fprintf(output_file, "| %s | %s |\n",filename, "     NO      ");

        if (bflag)
        {   
            if (flag)
                printf("| %s | %s |\n",filename, "     YES     ");
            else
                printf("| %s | %s |\n",filename, "     NO     ");
        }

    }
}

//Entradas: No posee entradas.
//Funcionamiento: Recibe los datos desde el pipe, con tal de iniciar la función de escritura.
//Salidas: No retorna.
void init_writer()
{
    // RECIVE THE ARGUMENTS FROM CLASIFIER
    char buffer[100];
    read(STDIN_FILENO, buffer, 100);

    // SPLIT THE BUFFER
    char** arguments = (char**)calloc(4, sizeof(char*));
    split_buffer(arguments, buffer, 4);
    int cvalue = atoi(arguments[0]);
    int bflag   = atoi(arguments[3]);

    write_results(cvalue, bflag);
}

void save_png_to_file (int** bitmap, char *path, int width, int height)
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
    
    fp = fopen (path, "wb");
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
    
    for (y = 0; y < height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
    fclose (fp);
}

//Entradas: No posee entradas.
//Funcionamiento: Función principal.
//Salidas: No retorna.
int main()
{
    // init_writer();
    int** matrix = (int**)calloc(50, sizeof(int*));
    for (int i=0; i<50; i++) matrix[i] = (int*)calloc(50, sizeof(int));

    for (int i=0; i<50; i++){
        for (int j=0; j<50; j++){
            matrix[i][j] = 255;
        }
    }
    save_png_to_file(matrix, "imagen.png", 50, 50);
    return 0;
}
