#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

void init_program(int argc, char **argv);
void init_pipeline(int cvalue, int nvalue, char* mvalue, int bflag);
void split_buffer(char** destiny, char* buffer, int items);