#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void input (int argc, char **argv) {
    int     cflag   = 0;
    int     mflag   = 0;
    int     nflag   = 0;
    int     bflag   = 0;
    int     cvalue = 0;
    int     nvalue = 0;

    char    mvalue[50];

    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
        switch (c) {
            case 'c':
                break;
            case 'm':
                break;
            case '?':
                return 1;
            default:
                abort();
        }
    }
}