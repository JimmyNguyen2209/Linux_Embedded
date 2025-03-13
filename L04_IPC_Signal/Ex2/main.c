#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int count = 1;

void signal_handler(int signum) {
    printf("Timer <%d> seconds\n", count);
    count++;

    if (count == 11) {
        printf("Exiting...\n");
        exit(0);
    }

    alarm(1);
}

int main() 
{
    signal(SIGALRM, signal_handler);

    alarm(count);

    while(1) {
        pause();
    }

    return 0;
}
