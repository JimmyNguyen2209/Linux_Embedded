#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void main()
{
    int fd;
    int numRead, numWrite;
    char text[10] = "Hiii ";
    char name[10] = "Hien";

    fd = open("text.txt", O_RDWR | O_APPEND | O_CREAT, 0644);
    if(fd == -1)
    {
        printf("Failed open file!");
    }



    numWrite = write(fd, text, strlen(text));
    
    lseek(fd, 0, SEEK_SET);

    numWrite = write(fd, name, strlen(name));

    close(fd);

    printf("Done!");
}