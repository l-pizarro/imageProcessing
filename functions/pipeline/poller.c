#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define READ 0
#define WRITE 1

void init_poller()
{
    char buffer[27];
    read(STDIN_FILENO, buffer, 27);
    printf("POLLER RECIVE: %s", buffer);

    // CONNECT WITH CLASIFIER
    int pipe_poller_clasifier[2];
    pipe(pipe_poller_clasifier);

    int pid = fork();

    if (pid == 0) {
        dup2(pipe_poller_clasifier[READ], STDIN_FILENO);
        close(pipe_poller_clasifier[WRITE]);
        execl("./clasifier", NULL);
    }
    else {
        dup2(pipe_poller_clasifier[WRITE], STDOUT_FILENO);
        close(pipe_poller_clasifier[READ]);
        write(STDOUT_FILENO, "It's a msg from poller\n", 23);
        wait(&pid);
    }
}


int main()
{
    init_poller();
    return 0;
}
