#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

int main()
{
    //Create child process
    pid_t pid = fork();

    if (pid == 0)
    {
        printf("Im here dad\n");
        exit(3);
    }
    else 
    {
        printf("Are you there child ?\n");
        int status;
        wait(&status);

        if (WIFEXITED(status))
        {
            printf("My dying number is %d dad\n", WEXITSTATUS(status));
        }
        else
        {
            printf("Im not dead dad\n");
        }

        printf("OK good \n"); // wait for SIGCHLD signal using pause()
    }
}
