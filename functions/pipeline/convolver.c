#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_convolver()
{
    char buffer[24];
    read(STDIN_FILENO, buffer, 24);
    printf("CONVOLVER RECIVE: %s", buffer);

    // CONNECT WITH RECTIFIER
    int pipe_convolver_rectifier[2];
    pipe(pipe_convolver_rectifier);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_convolver_rectifier[READ], STDIN_FILENO);
        close(pipe_convolver_rectifier[WRITE]);
        execl("./rectifier", NULL);
    }
    else {
        dup2(pipe_convolver_rectifier[WRITE], STDOUT_FILENO);
        close(pipe_convolver_rectifier[READ]);
        write(STDOUT_FILENO, "It's a msg from convolver\n", 27);
        wait(&pid);
    }
}


int main()
{
    init_convolver();
    return 0;
}
