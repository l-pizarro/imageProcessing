#include "general_functions.h"
#define READ 0
#define WRITE 1

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
                break;
            case 'm':
                sscanf(optarg, "%s", mvalue);
                break;
            case 'n':
                sscanf(optarg, "%d", &nvalue);
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