// Simple MSX-DOS file output to printer port
// tangent.space 2023

#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: PRINT FILE.EXT\n");
        return 1;
    }

    FILE *read_ptr = fopen(argv[1], "rb"); // Attempt to load file specified by argument

    if (read_ptr == NULL) // Verify file exists
    {
        printf("File not found.\n");
        return 1;
    }

    fseek(read_ptr, 0, SEEK_END); // Verify file size

    if (ftell(read_ptr) == 0) // Verify file is not empty
    {
        printf("File contains no data.\n");
        fclose(read_ptr);
        return 1;
    }

    fseek(read_ptr, 0, SEEK_SET);

    // FIXME: WRITE EACH BYTE OF INPUT FILE TO PRINTER PORT AND TOGGLE PSTB_n FOLLOWING EACH BYTE!

    fclose(read_ptr);
}
