#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd[2];
    pid_t pid;
    char message[] = "Hello from parent!";
    char buffer[100];

    if (pipe(fd) == -1) {
        perror("pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) { // Tiến trình con
        close(fd[1]); // Đóng đầu ghi
        read(fd[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        close(fd[0]);
    } else { // Tiến trình cha
        close(fd[0]); // Đóng đầu đọc
        write(fd[1], message, strlen(message) + 1);
        close(fd[1]);
    }

    return 0;
}
