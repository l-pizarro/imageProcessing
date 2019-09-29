#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

void init_program();
void get_inputs (int argc, char **argv, int* cvalue, int* nvalue, int* bflag);
void init_pipeline();
void read_image (char *png_file);