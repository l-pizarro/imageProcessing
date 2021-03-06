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

//Entradas: No posee entradas.
//Funcionamiento: Función principal.
//Salidas: No retorna.
int main()
{
    init_writer();
    return 0;
}
