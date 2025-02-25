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

    printf("Thông tin file: %s\n", file_name);
    printf("Kích thước: %ld bytes\n", fileStat.st_size);
    printf("Thời gian sửa đổi cuối: %s", ctime(&fileStat.st_mtime));
    if (S_ISREG(fileStat.st_mode)) {
        printf("%s là file thông thường.\n", file_name);
    } else if (S_ISDIR(fileStat.st_mode)) {
        printf("%s là thư mục.\n", file_name);
    } else if (S_ISLNK(fileStat.st_mode)) {
        printf("%s là file symbolic link.\n", file_name);
    } else {
        printf("%s là loại file khác.\n", file_name);
    }

    return 0;
}