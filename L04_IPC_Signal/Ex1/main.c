#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int count = 0;

void signal_handler(int signum) {
    count++;
    printf("SIGINT received %d/3\n", count);
    
    if (count == 3) {
        printf("Exiting...\n");
        exit(0);
    }
}

int main() 
{
    signal(SIGINT, signal_handler);

    while(1) {
        sleep(1);
    }
}
