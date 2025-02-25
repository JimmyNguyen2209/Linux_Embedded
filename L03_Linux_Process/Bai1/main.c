#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        printf("Lỗi khi tạo tiến trình con\n");
        return 1;
    } 
    else if (pid == 0) { 
        printf("Tiến trình con: PID = %d, PPID = %d\n", getpid(), getppid());
    } 
    else { 
        printf("Tiến trình cha: PID = %d, Con PID = %d\n", getpid(), pid);
    }

    return 0;
}