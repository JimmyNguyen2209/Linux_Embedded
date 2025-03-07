#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

//Run when child process is terminate
void waitHandler(int signum)
{
    printf("waitHandler() in progress\n");
    //Clean up child process
    wait(NULL);
    exit(10);
}

int main()
{
    //Create child process
    pid_t pid = fork();

    if (pid == 0)
    {
        printf("Im here dad\n");
        sleep(1);
        exit(3);
    }
    else 
    {
        //Receive every child process to avoid zombie process and run waitHandler();
        signal(SIGCHLD, waitHandler);
        printf("Are you there child ?\n");
        while(1) pause();  // wait for SIGCHLD signal using pause()
    }
}
