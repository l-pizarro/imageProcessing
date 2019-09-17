#include "functions/general/general_functions.h"

int main(int argc, char *argv[]) {
    char    mfile[50];  //Maybe we should find a cleaner way to do this
	int     cvalue   = 0;
    int     nvalue   = 0;
    int     bflag   = 0;

    // int     cvalue  = 0;
    // int     nvalue  = 0;

    // char    mvalue[50];

    int c;
    // opterr = 0;

    while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
        switch (c) {
            case 'c':
            	sscanf(optarg, "%d", &cvalue);
                break;
            case 'm':
            	//Here must go a strcpy. I'll do this later.
                break;
            case 'n':
            	sscanf(optarg, "%d", &nvalue);
                break;
            case 'b':
            	bflag = 1;
                break;
            case '?':
                if (optopt == 'h') {
                    fprintf(stderr, "Opcion -%c requiere un argumento.\n", optopt);
                }
                else if (isprint(optopt)) {
                    fprintf(stderr, "Opcion desconocida '-%c'.\n", optopt);
                }
                else {
                    fprintf(
                            stderr,
                            "Opción con caracter desconocido '\\x%x'.\n",
                            optopt
                        );
                }
            default:
                abort();
        }
    }

    init_program(cvalue, mfile, nvalue, bflag);
    return 1;
}
