#include "../general/general_functions.h"

void write_results(int cvalue, int bflag) {
    // OUTPUT FILE
    FILE* output_file;
    output_file = fopen("results.txt", "w");


    fprintf(output_file, "| image | nearly black |\n");
    if (bflag)
    {
        printf("| image | nearly black |\n");
    }

    for (int result_to_write=0; result_to_write<cvalue; result_to_write++)
    {
        // RECIVE THE ARGUMENTS FROM CLASIFIER
        char classification[2];
        read(STDIN_FILENO, classification, 2);
        char filename[100];
        read(STDIN_FILENO, filename, 100);

        fprintf(output_file, "| %s | %s |\n",filename, classification);
        if (bflag)
        {
            printf("| %s | %s |\n",filename, classification);
        }

    }
}

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


int main()
{
    init_writer();
    return 0;
}