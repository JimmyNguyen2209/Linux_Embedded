#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void main()
{
    int fd;
    int numRead, numWrite;
    char text[10] = "Chàoo";

    fd = open("text.txt", O_RDWR | O_APPEND);
    if(fd == -1)
    {
        printf("Failed open file!");
    }
    
    lseek(fd, 1, SEEK_SET);

    numWrite = write(fd, text, strlen(text));

    close(fd);

    printf("Done!");
}