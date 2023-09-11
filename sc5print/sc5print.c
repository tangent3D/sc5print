#include <stdio.h>

#define VRAM_SIZE 27136
#define BUFFER_SIZE (VRAM_SIZE / 4) // 6784 bytes, 53 rows of 128 bytes

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: SC5PRINT FILE.SC5\n");
        return 1;
    }

    FILE *read_ptr = fopen(argv[1], "rb"); // Attempt to load file specified by argument

    if (read_ptr == NULL) // Check if file exists
    {
        printf("File not found.\n");
        return 1;
    }

    fseek(read_ptr, 0, SEEK_END); // Check file size

    if (ftell(read_ptr) == 0) // Check if file is empty
    {
        printf("File contains no data.\n");
        fclose(read_ptr);
        return 1;
    }

    if (ftell(read_ptr) != 27143) // Check if file is exactly 27,143 bytes
    {
        fclose(read_ptr);
        printf("Invalid file size. Valid SC5 files are 27,143 bytes.\n");
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE] = {}; // File input buffer

    fseek(read_ptr, 7, SEEK_SET); // Skip file header (7 bytes)

    FILE *write_ptr; // Write to file for testing
    remove("out.bin"); // Get rid of existing output if exists
    write_ptr = fopen("out.bin", "ab"); // Append to file

    // !!! Initialize printer for raster graphics

    unsigned char byte;

    for (int i = 0; i < (VRAM_SIZE/BUFFER_SIZE); i++) // For each chunk of file (4 chunks of 6784 bytes)
    {
        fread(buffer, BUFFER_SIZE, 1, read_ptr);
        for (int j = 0; j < BUFFER_SIZE; j = j + 128) // For each row in buffer (53 rows of 128 bytes)
        {
            //fwrite(&byte, 1, 1, write_ptr);
            // Write EC*b32W to begin each row here
            printf("chunk %d row %d\n", i, j / 128);
            for (int k = 0; k < 128; k = k + 4) // For each four bytes in row
            {
                for (int l = 0; l < 4; l++) // For each byte in group of four bytes
                {
                    if ((buffer[j+k+l] | 0b11110000) == 0xF0)
                    {
                        printf("%d!\n", buffer[j+k+l]);
                    }
                }
            }
        }
    }

    // !!! close printer

    // convert 128 sc5 bytes into 1 row of 32 raster bytes 212 times

    fclose(read_ptr);
    fclose(write_ptr);
}
