#include "general_functions.h"
#define READ 0
#define WRITE 1


//Entradas: int argc -> Corresponde a la cantidad argumentos enviados a través de la entrada estándar
//          char** argv -> Corresponde a un arreglo con los argumentos enviados a través de la entrada estándar
//Funcionamiento: Es la encargada de recibir los datos desde la ejecución del programa, para comenzar el pipeline
//Salidas: No retorna.
void init_program(int argc, char **argv)
{
	int     cvalue = 0;
    int     nvalue = 0;
    int     bflag   = 0;
    char    mvalue[50];

    int command;
    opterr = 0;

    while ((command = getopt(argc, argv, "c:m:n:b")) != -1) {
        switch (command) {
            case 'c':
                sscanf(optarg, "%d", &cvalue);
                if (cvalue <= 0){
                    perror("Se debe analizar al menos 1 imagen. (El valor de C no puede ser menor a 0)");
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
	init_pipeline(cvalue, nvalue, mvalue, bflag);
}

//Entradas: int cvalue -> Corresponde a la cantidad de imágenes a analizar en el pipeline
//          int nvalue -> Corresponde al umbral de negrura sobre el cual concluir tras realizar los filtrados.
//          char* mvalue -> Corresponde al nombre del archivo que contiene la matriz para realizar la convolución.
//          int bflag -> Corresponde a una bandera que señaliza si los resultados se deben mostrar por salida estándar.
//Funcionamiento: Es la encargada de comenzar el pipeline, entregando los parámetros necesarios para que la primera
//                etapa sea capaz de leer.
//Salidas: No retorna.

void init_pipeline(int cvalue, int nvalue, char* mvalue, int bflag)
{   
    char buffer[100];
    sprintf(buffer, "%d %d %s %d\n", cvalue, nvalue, mvalue, bflag);
    int pipe_father_reader[2];
    pipe(pipe_father_reader);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_father_reader[READ], STDIN_FILENO);
        close(pipe_father_reader[WRITE]);
        execl("./reader", NULL);
    }
    else {
        dup2(pipe_father_reader[WRITE], STDOUT_FILENO);
        close(pipe_father_reader[READ]);
        write(STDOUT_FILENO, buffer, 100);
        wait(&pid);
    }
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