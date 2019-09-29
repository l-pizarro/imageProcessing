#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_reader()
{
    char buffer[24];
    read(STDIN_FILENO, buffer, 24);
    printf("READER RECIVE: %s", buffer);

    // CONNECT WITH CONVOLVER
    int pipe_reader_convolver[2];
    pipe(pipe_reader_convolver);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_reader_convolver[READ], STDIN_FILENO);
        close(pipe_reader_convolver[WRITE]);
        execl("./convolver", NULL);
    }
    else {
        dup2(pipe_reader_convolver[WRITE], STDOUT_FILENO);
        close(pipe_reader_convolver[READ]);
        write(STDOUT_FILENO, "It's a msg from reader\n", 24);
        wait(&pid);
    }
}

int main()
{
    init_reader();
    return 0;
}
