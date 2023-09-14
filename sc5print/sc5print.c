#include <stdio.h>

#define BUFFER_SIZE 27136 // 212 rows of 128 bytes (size of VRAM dump portion of SC5 file)

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

    // // Verify file is exactly 27,143 bytes
    // // IMPORTANT: MSX-DOS will round detected file size up to nearest 128 bytes!
    // if (ftell(read_ptr) != 27143)
    // {
        // fclose(read_ptr);
        // printf("Invalid file size. Valid SC5 files are 27,143 bytes.\n");
        // return 1;
    // }

    unsigned char buffer[BUFFER_SIZE] = {}; // File input buffer

    FILE *write_ptr; // Write to file for testing
    remove("out.pcl"); // Get rid of existing output if exists
    write_ptr = fopen("out.pcl", "ab"); // Open and append to output file

    unsigned char pclInit[] =  {EC, '%', '-', '1', '2', '3', '4', '5', 'X',             // Exit language
                                EC, 'E',                                                // Printer reset
                                EC, '&', 'l', '2', '6', 'A',                            // Page size (A4)
                                EC, '&', 'l', '1', 'O',                                 // Orientation (landscape)
                                EC, '&', 'l', '1', 'E',                                 // Top margin
                                EC, '*', 'p', '6', '7', '8', 'x', '3', '4', '0', 'Y',   // Position cursor (center image on page)
                                EC, '*', 't', '7', '5', 'R',                            // Raster graphics resolution (75 DPI)
                                EC, '*', 'r', '0', 'F',                                 // Raster presentation method
                                EC, '*', 'r', '1', 'A'};                                // Start raster graphics

    unsigned char pclRow[] =   {EC, '*', 'b', '6', '4', 'W'};                           // 64 bytes per row

    unsigned char pclEnd[] =   {EC, '*', 'r', 'C',                                      // End raster graphics
                                EC, 'E',
                                EC, '%', '-', '1', '2', '3', '4', '5', 'X'};

    fseek(read_ptr, 7, SEEK_SET); // Skip file header (7 bytes)
    fread(buffer, BUFFER_SIZE, 1, read_ptr); // Read SC5 VRAM dump into buffer

    fwrite(pclInit, 1, sizeof(pclInit), write_ptr); // Write PCL initialization

    unsigned short int mask;
    unsigned short int word;
    unsigned char byte;

    for (int j = 0; j < sizeof(buffer); j = j + 128) // For each row in buffer (212 rows of 128 bytes)
    {
        for (int i = 0; i < 2; i++) // Write each raster row twice
        {
            fwrite(pclRow, 1, sizeof(pclRow), write_ptr); // Write PCL Transfer Raster Data by Row/Block
            for (int k = 0; k < 128; k = k + 4) // For each set of four VRAM bytes in row, compose one raster word
            {
                mask = 0b1100000000000000;
                word = 0b0000000000000000;

                for (int l = 0; l < 4; l++) // For each VRAM byte in set of four
                {
                    if ((buffer[j+k+l] & 0xF0) < 0xF0) // If upper nibble of byte < F, set bits
                    {
                        word = word | mask;
                    }

                    mask = mask >> 2; // Rotate bit mask

                    if ((buffer[j+k+l] & 0x0F) < 0x0F) // If lower nibble of byte < F, set bits
                    {
                        word = word | mask;
                    }

                    mask = mask >> 2; // Rotate bit mask
                }

                byte = (unsigned char) (word >> 8); // Write upper byte of word
                fwrite(&byte, 1, 1, write_ptr);

                byte = (unsigned char) word;
                fwrite(&byte, 1, 1, write_ptr);     // Write lower byte of word
            }
        }
    }

    fwrite(pclEnd, sizeof(pclEnd), 1, write_ptr); // Write PCL End Raster Graphics

    fclose(read_ptr);
    fclose(write_ptr);
}
