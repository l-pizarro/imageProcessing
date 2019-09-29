#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_rectifier()
{
    char buffer[27];
    read(STDIN_FILENO, buffer, 27);
    printf("RECTIFIER RECIVE: %s", buffer);

    // CONNECT WITH POLLER
    int pipe_rectifier_poller[2];
    pipe(pipe_rectifier_poller);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_rectifier_poller[READ], STDIN_FILENO);
        close(pipe_rectifier_poller[WRITE]);
        execl("./poller", NULL);
    }
    else {
        dup2(pipe_rectifier_poller[WRITE], STDOUT_FILENO);
        close(pipe_rectifier_poller[READ]);
        write(STDOUT_FILENO, "It's a msg from rectifier\n", 27);
        wait(&pid);
    }
}


int main()
{
    init_rectifier();
    return 0;
}
