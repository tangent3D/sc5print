#include <stdio.h>

#define VRAM_SIZE 27136 // 212 rows of 128 bytes
#define BUFFER_SIZE (VRAM_SIZE / 4) // 6784 bytes, 53 rows of 128 bytes

#define EC 0x1B // PCL5 escape character

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: SC5PRINT FILE.SC5\n");
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

    if (ftell(read_ptr) != 27143) // Verify file is exactly 27,143 bytes
    {
        fclose(read_ptr);
        printf("Invalid file size. Valid SC5 files are 27,143 bytes.\n");
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE] = {}; // File input buffer

    fseek(read_ptr, 7, SEEK_SET); // Skip file header (7 bytes)

    FILE *write_ptr; // Write to file for testing
    remove("out.bin"); // Get rid of existing output if exists
    write_ptr = fopen("out.bin", "ab"); // Open and append to output file

    unsigned char pclInit[] =  {EC, '%', '-', '1', '2', '3', '4', '5', 'X',             // Exit language
                                EC, 'E',                                                // Printer reset
                                EC, '*', 'p', '3', '0', '0', 'x', '3', '0', '0', 'Y',   // Position cursor
                                EC, '*', 't', '7', '5', 'R',                            // Raster graphics resolution
                                EC, '*', 'r', '0', 'F',                                 // Raster presentation method
                                EC, '*', 'r', '1', 'A'};                                // Left raster graphics margin

    unsigned char pclRow[] =   {EC, '*', 'b', '3', '2', 'W'};

    unsigned char pclEnd[] =   {EC, '*', 'r', 'C',
                                EC, 'E',
                                EC, '%', '-', '1', '2', '3', '4', '5', 'X'};

    fwrite(pclInit, 1, sizeof(pclInit), write_ptr); // Write PCL initialization

    for (int i = 0; i < (VRAM_SIZE / BUFFER_SIZE); i++) // For each chunk of file (4 chunks of 6784 bytes)
    {
        fread(buffer, BUFFER_SIZE, 1, read_ptr);
        for (int j = 0; j < BUFFER_SIZE; j = j + 128) // For each row in buffer (53 rows of 128 bytes)
        {
            fwrite(pclRow, 1, sizeof(pclRow), write_ptr);
            for (int k = 0; k < 128; k = k + 4) // For each set of four VRAM bytes in row, compose one raster byte
            {
                unsigned char byte = 0;
                unsigned char mask = 0b10000000;
                for (int l = 0; l < 4; l++) // For each VRAM byte in set
                {
                    if ((buffer[j+k+l] & 0xF0) < 0xF0) // If upper nibble of byte < F, set bit
                    {
                        byte = byte | mask;
                    }

                    mask = mask >> 1; // Rotate bit mask

                    if ((buffer[j+k+l] & 0x0F) < 0x0F) // If lower nibble of byte < F, set bit
                    {
                        byte = byte | mask;
                    }

                    mask = mask >> 1; // Rotate bit mask

                }
                fwrite(&byte, 1, 1, write_ptr); // Write composed raster byte
            }
        }
    }

    fwrite(pclEnd, 1, sizeof(pclEnd), write_ptr);

    fclose(read_ptr);
    fclose(write_ptr);
}
