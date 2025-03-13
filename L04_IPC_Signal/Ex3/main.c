#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int count = 0;

void signal_handler(int signum) {
    count++;
    printf("Recieved signal form parent %d/5\n", count);    
    
    if (count == 5) {
        printf("Exiting...\n");
        exit(0);
    }
}

int main() 
{
    pid_t pid = fork();

    if (pid > 0)
    {
        for(int i = 0; i < 5; i++)
        {
            sleep(2);
            kill(pid, SIGUSR1); 
        }
        wait(NULL);
        exit(0);
    }
    else if (pid == 0)
    {
        signal(SIGUSR1, signal_handler);
        while(1) {
            pause();
        }
    }
    else
    {
        printf("Error forking\n");
    }
    return 0;
}
