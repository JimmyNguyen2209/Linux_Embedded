#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void signal_handler(int signum) {
    printf("Received signal %d\n", signum);
}

int main() 
{
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        printf("Cannot handle SIGINT\n");
        exit(1);
    }
    while(1);
}
