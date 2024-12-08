#ifndef HELPER_H
#define HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <argp.h>
#include <argz.h>

#define MAX_BUFF_SIZE 1024 * 500    // Max supported file size

// Argp related

const char *argp_program_version = "version 1.0";                   

// Options for argp
static struct argp_option options[] = 
{
  { 0, 'i', "FILE",      0,  "output in C include file style.", 0 },
  { 0, 'e', "FILE",      0,  "output in little endian", 0 },
  { 0 }
};

// Struct to hold non optional arguments
struct arguments
{
    char *argz;
    size_t argz_len;
    char *input_file;
    int opt;
};



// Function prototypes
static int parse_opt (int key, char *arg, struct argp_state *state);
char *read_and_process(char *file_path, size_t *size);
size_t get_file_size(char *file_path);
char *alloc_buffer(size_t size);
void read_file(char *file_path, char *buffer, int buffer_size);
void hexdump(void *buffer, size_t file_size);
void hexdump_little_endian(void *buffer, size_t file_size);
void rename_file(char *file);
void print_c_style(void *buffer, char *file_name, size_t file_size);

#endif
