#include <stdio.h>

#define BUFFER_SIZE 27136/

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("Usage: SC5PRINT FILE.SC5\n");
        return 1;
    }

    // Get SC5 file from argument

    char *outBuffer[BUFFER_SIZE];
    char *inBuffer[BUFFER_SIZE];

    FILE *read_ptr = fopen(argv[1], "rb");

    if (read_ptr == NULL)
    {
        printf("File not found.\n");
        return 1;
    }

    if (fseek(read_ptr, 0, SEEK_END) < 0)
    {
        printf("File contains no data.\n");
        fclose(read_ptr);
        return 1;
    }

    if (ftell(read_ptr) != 27143)
    {
        fclose(read_ptr);
        printf("Invalid file size.");
        return 1;
    }

    fseek(read_ptr, 7, SEEK_SET); // Skip file header (7 bytes)
    fread(inBuffer, BUFFER_SIZE, 1, read_ptr);

    // initialize printer for raster graphics

    // convert 128 sc5 bytes into 1 row of 32 raster bytes 212 times

    // close printer

    FILE *write_ptr;
    write_ptr = fopen("out.bin", "wb");
    fwrite(inBuffer, BUFFER_SIZE, 1, write_ptr);
    fclose(write_ptr);
}
