#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
    char buffer[100] = "Nguyễn Công ";
    int fd = open("text.txt", O_RDWR | O_CREAT, 0644);

    int write_bytes = write(fd, buffer, strlen(buffer));

    close(fd);
    return 0;
}