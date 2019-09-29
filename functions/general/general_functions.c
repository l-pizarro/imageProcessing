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
	init_pipeline();
}




void init_pipeline()
{   
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
        write(STDOUT_FILENO, "It's a msg from father\n", 24);
        wait(&pid);
    }
}