CC = clang
CFLAGS = -Wall -ggdb -std=c11 -Wextra -pedantic

all: hexdump.c
	$(CC) $(CFLAGS) hexdump.c -o hexdump

clean:
	rm -f hexdump
