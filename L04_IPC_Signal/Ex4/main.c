#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

void sigtstp_handler(int signum) {
    printf("SIGTSTP ignored\n");
}

int main() 
{
    signal(SIGTSTP, sigtstp_handler);

    while (1) {
        printf("Running... \n");
        sleep(1);
    }

    return 0;
}
