// tangent.space 2023

// FIXME: put this on command line
#pragma output USING_amalloc

#include "convert.h"
#include <stdio.h>
#include <malloc.h>

// Generate an executable and write output buffer as file for testing

unsigned char *inBuffer;
unsigned char *outBuffer;

unsigned int outIndex = 0;

int main(int argc, char* argv[])
{
  inBuffer = malloc(sizeof *inBuffer * INPUT_BUFFER_SIZE);
  outBuffer = malloc(sizeof *outBuffer * OUTPUT_BUFFER_SIZE);

  if (argc == 1)
  {
      printf("Usage: SC5PRINT FILE.SC5\n");
      return 1;
  }

  FILE *read_ptr = fopen(argv[1], "rb"); // Get SC5 file from argument

  // Error checking
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

  fseek(read_ptr, 0, SEEK_SET); // Read SC5 file into input buffer
  fread(inBuffer, INPUT_BUFFER_SIZE, 1, read_ptr);

  // FIXME: get output file name from input file name duh
  FILE *write_ptr; // Write to file for testing
  remove("test_out.PCL"); // Delete existing output if exists
  write_ptr = fopen("test_out.PCL", "ab"); // Open and append to output file

  convert(); // Convert contents of input buffer into output buffer

  fwrite(outBuffer, 1, OUTPUT_BUFFER_SIZE, write_ptr); // Write output buffer to file

  fclose(read_ptr);
  fclose(write_ptr);

  free(outBuffer);
  free(inBuffer);
}

// PCL5 command constants
const unsigned char pclInit[] = {EC, '%', '-', '1', '2', '3', '4', '5', 'X',             // Exit language
                                 EC, 'E',                                                // Printer reset
                                 EC, '&', 'l', '2', '6', 'A',                            // Page size (A4)
                                 EC, '&', 'l', '1', 'O',                                 // Orientation (landscape)
                                 EC, '&', 'l', '1', 'E',                                 // Top margin
                                 EC, '*', 'p', '6', '7', '8', 'x', '3', '4', '0', 'Y',   // Position cursor (center image on page)
                                 EC, '*', 't', '7', '5', 'R',                            // Raster graphics resolution (75 DPI)
                                 EC, '*', 'r', '0', 'F',                                 // Raster presentation method
                                 EC, '*', 'r', '1', 'A'};                                // Start raster graphics

const unsigned char pclRow[] =  {EC, '*', 'b', '6', '4', 'W'};                           // 64 bytes per row

const unsigned char pclEnd[] =  {EC, '*', 'r', 'C',                                      // End raster graphics
                                 EC, 'E',
                                 EC, '%', '-', '1', '2', '3', '4', '5', 'X'};


unsigned int mask;
unsigned int word;
unsigned int inIndex;

void convert() // Convert contents of inBuffer (SC5) into outBuffer (PCL)
{
  writeArray(pclInit, sizeof pclInit); // Write PCL initialization to output buffer

  // Skip first 7 bytes in buffer (SC5 header!)
  for (inIndex = 7; inIndex < INPUT_BUFFER_SIZE; inIndex = inIndex + 128) // For each row of 128 bytes in input buffer (212 rows)
  {
    // Write each raster row twice
    convertRow();
    convertRow();
  }

  writeArray(pclEnd, sizeof pclEnd); // Write PCL ending to output buffer
}

void convertRow()
{
  writeArray(pclRow, sizeof pclRow); // Write PCL Transfer Raster Data by Row/Block
  for (unsigned int k = 0; k < 128; k = k + 4) // For each set of four VRAM bytes in row, compose one raster word
  {
    mask = 0b1100000000000000;
    word = 0b0000000000000000;

    for (unsigned int l = 0; l < 4; l++) // For each VRAM byte in set of four
    {
      if ((inBuffer[inIndex+k+l] & 0xF0) < 0xF0) // If upper nibble of byte < F, set bits
      {
        word = word | mask;
      }

      mask = mask >> 2; // Rotate bit mask

      if ((inBuffer[inIndex+k+l] & 0x0F) < 0x0F) // If lower nibble of byte < F, set bits
      {
        word = word | mask;
      }

      mask = mask >> 2; // Rotate bit mask
    }

    writeWord();
  }
}

void writeArray(const unsigned char* array, int size) // Append array to output buffer
{
  for (int i = 0; i < size; i++)
  {
    outBuffer[outIndex] = array[i];
    outIndex++;
  }
}

void writeWord() // Compose bytes and write word to output buffer
{
  unsigned char upperByte = (unsigned char) (word >> 8);
  outBuffer[outIndex] = upperByte;
  outIndex++;

  unsigned char lowerByte = (unsigned char) word;
  outBuffer[outIndex] = lowerByte;
  outIndex++;
}
