#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define MAX_BUFF_SIZE 1024 * 50

size_t get_file_size(char *file_path);
char *alloc_buffer(size_t size);
void read_file(char *file_path, char *buffer, int buffer_size);
void hexdump(void *buffer, size_t file_size);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    char *file_path = argv[1];

    size_t file_size = get_file_size(file_path);
    char *buffer = alloc_buffer(file_size);
    read_file(file_path, buffer, file_size);
    hexdump(buffer, file_size);
    free(buffer);
    return 0;
}

size_t get_file_size(char *file_path)
{
    off_t file_size;
    struct stat stbuff;
    int fd;

    // Checking if we can open the given file or not.
    fd = open(file_path, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Could not open '%s'\n", file_path);
        exit(EXIT_FAILURE);
    }
    
    // Checking if the given file is a regular file or not and if we can read the file attributes.
    if (fstat(fd, &stbuff) != 0 || (!S_ISREG(stbuff.st_mode)))
    {
        fprintf(stderr, "Given file:'%s' is not a regular file\n", file_path);
        exit(EXIT_FAILURE);
    }

    file_size = stbuff.st_size;

    if (file_size <= 0)
    {
        fprintf(stderr, "[Error]: File has '%zu' <= 0 bytes\n", file_size);
        exit(EXIT_FAILURE);
    }
    else if (file_size > MAX_BUFF_SIZE)
    {
        fprintf(stderr, "[Error]: File size (%zu) is too big, max size allowed is %d bytes\n", file_size, MAX_BUFF_SIZE);
        exit(EXIT_FAILURE);
    }
    // Closing the file descriptor before returning.
    close(fd);
    return file_size;
}

char *alloc_buffer(size_t size)
{
    char *buffer = calloc(size, sizeof(char));
    if (buffer == NULL)
    {
        fprintf(stderr, "[Error]: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return buffer;
}

void read_file(char *file_path, char *buffer, int file_size)
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "[Error]: Unable to open and read the file '%s\n", file_path);
        exit(EXIT_FAILURE);
    }

    fread(buffer, sizeof(char), file_size, fp);

    // Closing file pointer and file descriptor
    fclose(fp);
}

void hexdump(void *buffer, size_t file_size)
{
    u_int8_t *data = buffer;
    size_t i, j;

    for (i = 0; i < file_size; i++)
    {
        // Print the address offset at the start of each line
        if (i % 16 == 0)
        {
            printf("%08zX  ", i);
        }

        // Add extra space after 8 bytes
        if (i % 8 == 0 && i % 16 != 0)
        {
            printf(" ");
        }

        // Print the byte in hexadecimal format
        printf("%02X ", data[i]);

        // Checking if we are at the end of a line or the end of the file
        if (i % 16 == 15 || i == file_size - 1)
        {
            // Add padding spaces if the line is incomplete
            if (i % 16 != 15)
            {
                for (j = i % 16 + 1; j < 16; j++)
                {
                    printf("   ");
                    if (j == 7) printf(" "); // Add extra space after 8 bytes
                }
            }

            // Print the ASCII representation
            printf(" |");
            for (j = i - (i % 16); j <= i; j++)
            {
                if (isprint(data[j]))
                {
                    printf("%c", data[j]);
                }
                else
                {
                    printf(".");
                }
            }
            printf("|\n");
        }
    }

    // Printing the final address offset
    printf("%08zX\n", file_size);
}
