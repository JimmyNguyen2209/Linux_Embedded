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
        printf("My ID is %d\n", getpid());
        exit(0);
    }
    else 
    {
        printf("Shit, i'm a zombie\n");
        while(1);
    }

    pid_t pid2 = fork();

    sleep(2);

    if (pid2 == 0)
    {
        printf("Im number 2\n");
        sleep(1);
    }
    else 
    {
        printf("Where's my child ?\n");
        exit(0);
    }
}
