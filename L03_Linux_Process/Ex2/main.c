#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s \'1 or 2\'",argv[0]);
        return 0;
    }
    int num = atoi(argv[1]);

    pid_t pid = fork();

    char *args[] = {"date", NULL};

    if (pid == 0)
    {
        if (num == 1)
        {
            printf("Run command <ls -lah>\n");
            execlp("/bin/ls", "ls", "-l", "-a", "-h", NULL);
        }
        if (num == 2)
        {
            printf("Run command <date>\n");
            execvp("/bin/date", args);
        }
    }
    else 
    {
        printf("Im your dad punk!\n");
        wait(NULL);
        printf("Done\n");
    }
}