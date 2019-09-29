#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_clasifier()
{
    char buffer[23];
    read(STDIN_FILENO, buffer, 23);
    printf("CLASIFIER RECIVE: %s", buffer);

    // CONNECT WITH WRITER
    int pipe_clasifier_write[2];
    pipe(pipe_clasifier_write);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_clasifier_write[READ], STDIN_FILENO);
        close(pipe_clasifier_write[WRITE]);
        execl("./writer", NULL);
    }
    else {
        dup2(pipe_clasifier_write[WRITE], STDOUT_FILENO);
        close(pipe_clasifier_write[READ]);
        write(STDOUT_FILENO, "It's a msg from clasifier\n", 26);
        wait(&pid);
    }
}


int main()
{
    init_clasifier();
    return 0;
}
