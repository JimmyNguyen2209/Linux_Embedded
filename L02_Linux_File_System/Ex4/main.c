#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

int main()
{
    char file_name[] = "test.txt";
    char content[] = "Test file thôi nên đừng quan tâm \n";

    int fd = open(file_name, O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        printf("Failed to open file");
        close(fd);
        return 0;
    }

    int write_bytes = write(fd, content, strlen(content));

    struct stat fileStat;
    if (stat(file_name, &fileStat) == -1)
    {
        printf("Failed to read file");
        return 1;
    }

    printf("File infomation: %s\n", file_name);
    printf("Memory: %ld bytes\n", fileStat.st_size);
    printf("Last edited time: %s", ctime(&fileStat.st_mtime));
    if (S_ISREG(fileStat.st_mode)) {
        printf("%s is Normal file.\n", file_name);
    } else if (S_ISDIR(fileStat.st_mode)) {
        printf("%s is dictionary.\n", file_name);
    } else if (S_ISLNK(fileStat.st_mode)) {
        printf("%s is symbolic link file.\n", file_name);
    } else {
        printf("%s is another file type.\n", file_name);
    }

    return 0;
}