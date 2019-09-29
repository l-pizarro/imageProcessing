#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_writer()
{
    char buffer[26];
    read(STDIN_FILENO, buffer, 26);
    printf("WRITER RECIVE: %s", buffer);
   
    // EXIT 
}


int main()
{
    init_writer();
    return 0;
}
