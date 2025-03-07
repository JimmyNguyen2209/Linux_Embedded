#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    // Make children process
    pid_t pid = fork(); 

    if (pid == 0)
    {
        printf("Im children process, my PID is %d, my parent PID is %d \n", getpid(), getppid());
    }
    else
    {
        printf("Im parents process, my PID is %d, my parent PID is %d \n", getpid(), getppid());
        //Keep children process not orphan
        while(1);
    }
    //Children parent PID is parent PID
    return 0;
}