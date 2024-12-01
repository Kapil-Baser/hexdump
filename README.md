# hexdump
A simplified clone of unix hexdump utility.

## Compiling
```bash
make all
```
## Usage
```bash
kapil@DESKTOP-Q6HEFQA:~/repos/hexdump$ ./hexdump test.txt
00000000  54 68 69 73 20 69 73 20  74 65 73 74 20 66 69 6C  |This is test fil|
00000010  65 2E 0A                                          |e..|
00000013
```
C style output
```bash
kapil@DESKTOP-Q6HEFQA:~/repos/hexdump$ ./hexdump test.txt
unsigned char test_txt[] = {
  0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x65, 0x73, 0x74,
  0x20, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x0A
};
unsigned int test_txt_len = 19;
```
TODO:
 - ~~Adding support for c style output~~
 - ~~Adding support for displaying bytes in little endian order~~
 - Need to fix command line argument options taking more than one option
