// Simple MSX-DOS file output to printer port
// tangent.space 2023

#define BUFFER_SIZE 4096

#include <stdio.h>

__sfr __at  0x90 PSTB_n; // MSX printer port /STROBE signal (write) ex. PSTB_n = 0; // Set /STROBE signal active
__sfr __at  0x91 DATA; // MSX printer port data bits 7-0 (write only) ex. DATA = foo // Write value of foo to parallel data bus

int main(int argc, char* argv[])
{
    DATA = 0;
    PSTB_n = 1;

    if (argc == 1)
    {
        printf("Write contents of file to printer port.\nUsage: PRINT FILE.EXT\n");
        return 1;
    }

    FILE *read_ptr = fopen(argv[1], "rb"); // Attempt to read file from argument

    if (read_ptr == NULL) // Verify file exists
    {
        printf("File not found.\n");
        return 1;
    }

    // Write out contents of file to printer port in chunks
    // NOTE: Total bytes written will round up to nearest 128 bytes!
    unsigned char buffer[BUFFER_SIZE] = {};
    int nread;
    while ((nread = fread(buffer, 1, sizeof buffer, read_ptr)) > 0)
    {
        //printf("%d\n",nread); // Print amount of bytes read
        for (int i = 0; i < nread; i++)
        {
          DATA = buffer[i];
          PSTB_n = 0;
          PSTB_n = 1;
        }
    }
    fclose(read_ptr);
}
