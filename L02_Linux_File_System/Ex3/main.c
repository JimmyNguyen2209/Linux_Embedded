#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>

int is_Number(char *str);
int create_text_file();

int main(int argc, char *argv[])
{
    create_text_file();

    if (argc != 5)
    {
        printf("Usage: %s filename num-bytes [r/w] \"text\"\n", argv[0]);
        return 1;
    }

    int fd, numRead, numWrite, num_Wanted;

    int length = strlen(argv[4]);
    char *str = (char *)malloc((length + 1) * sizeof(char));
    strncpy(str, argv[4], length);
    str[length] ='\0';

    if (strcmp(argv[1], "text.txt") != 0)
    {
        printf("%s is not available\n", argv[1]);
        return 1;
    }

    if (!is_Number(argv[2]))
    {
        printf("Invalid number of bytes: %s\n", argv[2]);
        return 1;
    }

    num_Wanted = atoi(argv[2]);
    if (num_Wanted <= 0)
    {
        printf("Number of bytes must be greater than zero.\n");
        return 1;
    }

    if (strcmp(argv[3], "r") == 0)
    {
        fd = open("text.txt", O_RDONLY);
        if (fd == -1)
        {
            printf("Failed to open file");
            return 1;
        }

        char *buffer = (char *)malloc((num_Wanted + 1) * sizeof(char));
        if (buffer == NULL)
        {
            printf("Memory allocation failed\n");
            close(fd);
            return 1;
        }

        numRead = read(fd, buffer, num_Wanted);
        lseek(fd, 0, SEEK_SET);
        if (numRead == -1)
        {
            perror("Failed to read file");
            free(buffer);
            close(fd);
            return 1;
        }

        buffer[numRead] = '\0';
        printf("%s\n", buffer);

        free(buffer);
        close(fd);
    }
    else if (strcmp(argv[3], "w") == 0)
    {
        fd = open("text.txt", O_WRONLY);
        if (fd == -1)
        {
            printf("Failed to open file");
            return 1;
        }

        lseek(fd, 0, SEEK_END);
        numWrite = write(fd, str, strlen(str));

        close (fd);
    }
    
    return 0;
}

int is_Number(char *str)
{
    if (str[0] == '\0') 
    {
        return 0;
    }

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int create_text_file()
{
    char buffer[100] = "Nguyễn Công ";
    int fd = open("text.txt", O_RDWR | O_CREAT, 0644);

    int write_bytes = write(fd, buffer, strlen(buffer));

    close(fd);
    return 0;
}