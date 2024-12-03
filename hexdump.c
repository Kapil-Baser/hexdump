#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#define MAX_BUFF_SIZE 1024 * 50

char *read_and_process(char *file_path, size_t *size);
size_t get_file_size(char *file_path);
char *alloc_buffer(size_t size);
void read_file(char *file_path, char *buffer, int buffer_size);
void hexdump(void *buffer, size_t file_size);
void hexdump_little_endian(void *buffer, size_t file_size);
void rename_file(char *file);
void print_c_style(void *buffer, char *file_name, size_t file_size);
void print_usage();
void print_help();

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    char *file_path = argv[2];
    size_t file_size;
    char *buffer = NULL;
    int option;
    int cflag = 0, eflag = 0, iflag = 0;
    while ((option = getopt(argc, argv, "ceih")) != -1)
    {
        switch(option)
        {
            case 'c':
            {
                if (cflag)
                {
                    fprintf(stderr, "[Error]: Give only one option at a time\n");
                    return 1;
                }
                else
                {
                    cflag++;
                    eflag++;
                    iflag++;
                }
                buffer = read_and_process(file_path, &file_size);
                hexdump(buffer, file_size); 
                break;
            }
            case 'e':
            {
                if (eflag)
                {
                    fprintf(stderr, "[Error]: Give only one option at a time\n");
                    return 1;
                }
                else
                {
                    eflag++;
                    cflag++;
                    iflag++;
                }
                buffer = read_and_process(file_path, &file_size);
                hexdump_little_endian(buffer, file_size);
                break;
            }
            case 'i':
            {
                if (iflag)
                {
                    fprintf(stderr, "[Error]: Give only one option at a time\n");
                    return 1;
                }
                else
                {
                    cflag++;
                    eflag++;
                    iflag++;
                }
                buffer = read_and_process(file_path, &file_size);
                print_c_style(buffer, file_path, file_size);
                break;
            }
            case 'h':
            {
                print_help();
            }
            default:
            {
                print_usage(argv[0]);
            }
        }
    }      
    free(buffer);
    return 0;
}

void print_usage(char *program_name)
{
    fprintf(stderr, "Usage: %s [options] <file>\n", program_name);
}

char *read_and_process(char *file_path, size_t *size)
{
    size_t file_size = get_file_size(file_path);
    char *buffer = alloc_buffer(file_size);
    read_file(file_path, buffer, file_size);
    *size = file_size;
    return buffer;
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

void print_help()
{

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

void hexdump_little_endian(void *buffer, size_t file_size)
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

        // Add extra space after 8 bytes for readability
        if (i % 8 == 0 && i % 16 != 0)
        {
            printf(" ");
        }

        // Handle Little-Endian Order
        if (i % 4 == 3)  // Group by 4 bytes
        {
            // Reverse and print 4-byte group
            for (j = 0; j < 4; j++)
            {
                printf("%02X ", data[i - j]);
            }
        }

        // For the last few bytes of the file, ensure proper printing
        if (i == file_size - 1 && (i % 4 != 3))
        {
            // Print remaining bytes in reversed order
            size_t start = i - (i % 4);
            for (j = i; j >= start; j--)
            {
                printf("%02X ", data[j]);
            }

            // Add padding for missing bytes in the last group
            for (j = 0; j < (3 - i % 4); j++)
            {
                printf("   ");
            }
        }

        // Handle the end of a line
        if (i % 16 == 15 || i == file_size - 1)
        {
            // Add padding spaces if the line is incomplete
            if (i % 16 != 15)
            {
                for (j = i % 16 + 1; j < 16; j++)
                {
                    printf("   ");
                    if (j == 7) printf(" "); // Extra space after 8 bytes
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

    // Print the final address offset
    printf("%08zX\n", file_size);
}

void rename_file(char *file)
{
    while (*file)
    {
        if (*file == '.')
        {
            *file = '_';
        }
        file++;
    }
}

void print_c_style(void *buffer, char *file_name, size_t file_size)
{
    u_int8_t *data = buffer;
    size_t i;
    rename_file(file_name);
    printf("unsigned char %s[] = {\n", file_name);

    for (i = 0; i < file_size; i++)
    {
        // Spacing at every new line.
        if (i % 12 == 0)
        {
            printf("  ");
        }
        // Since we don't want a comma at the very last offset, we check if this index is the last one or not.
        if (i != file_size - 1)
        {
            printf("0x%02X, ", data[i]);
        }
        else
        {
            printf("0x%02X ", data[i]);
        }
        
        // New line after 12 bytes.
        if (i % 12 == 11 || i == file_size - 1)
        {
            printf("\n");
        }
    }
    printf("};\n");
    printf("unsigned int %s_len = %zu;\n", file_name, file_size);
}
