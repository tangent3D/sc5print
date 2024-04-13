// Command-line utility to convert monochrome MSX2/2+/tR .SC5 VRAM dumps to printable PCL5 commands
// SC5 colors 0 = black, 15 = white
// tangent.space 2023

#include "sc5pcl.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

unsigned char *rowBuffer;
unsigned char *outputBuffer;
unsigned int outputBufferIndex = 0;

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


int main(int argc, char* argv[])
{
  // 128 byte buffer (one row of SC5 pixel data)
  rowBuffer = malloc(sizeof *rowBuffer * BYTES_PER_ROW);

  outputBuffer = malloc(sizeof *outputBuffer * OUTPUT_BUFFER_SIZE);

  if (argc == 1)
  {
      printf("Usage: SC5PCL FILE.SC5\n");
      return 1;
  }

  // Error checking

  // Check argument extension
  char *ext = strrchr(argv[1], '.');
  if (!ext)
  {
    // FIXME: final text
    printf("no extension");
    return 1;
  }
  else
  {
    // FIXME: exit if extension is not SC5
    printf("extension is %s\n", ext + 1);
  }

  FILE *read_ptr = fopen(argv[1], "rb"); // Get SC5 file from argument

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

  if (ftell(read_ptr) < SC5_FILE_SIZE) // Verify file is at least 27,143 bytes
  {
    fclose(read_ptr);
    printf("Invalid file size.\nValid SC5 files are >=27,143 bytes.\n");
    return 1;
  }

  // Read SC5 file into input buffer (skips SC5 header (7 bytes))
  fseek(read_ptr, 7, SEEK_SET);

  // Put PCL initialization to output buffer
  writeArray(pclInit, sizeof pclInit);

  // Convert each pixel row in SC5 to PCL5 and put each row twice in output buffer to double image size vertically
  for (int i = 0; i < TOTAL_ROWS; i++)
  {
    fread(rowBuffer, BYTES_PER_ROW, 1, read_ptr);
    writeArray(pclRow, sizeof pclRow); // Write PCL Transfer Raster Data by Row/Block
    convertRow();
    writeArray(pclRow, sizeof pclRow); // Write PCL Transfer Raster Data by Row/Block
    convertRow();
  }

  // Put PCL ending to output buffer
  writeArray(pclEnd, sizeof pclEnd);

  // Compose output file name (basename + '.PCL')
  char *baseName = strtok(argv[1], ".");
  char *outputFileName = strcat(baseName, ".PCL");

  // Open and append to output file
  FILE *write_ptr;
  write_ptr = fopen(outputFileName, "ab");

  // Write output buffer to file
  fwrite(outputBuffer, 1, OUTPUT_BUFFER_SIZE, write_ptr);

  // Clean up before exit
  fclose(read_ptr);
  fclose(write_ptr);
  free(rowBuffer);
  free(outputBuffer);
}

void convertRow()
{
  unsigned int mask;
  unsigned int word;
  unsigned int inIndex;

  // For each set of four VRAM bytes in row, compose one raster word
  for (unsigned int k = 0; k < 128; k = k + 4)
  {
    // Rotate a mask to double image size horizontally
    mask = 0b1100000000000000;
    word = 0b0000000000000000;

    // For each VRAM byte in set of four
    for (unsigned int l = 0; l < 4; l++)
    {
      if ((rowBuffer[inIndex+k+l] & 0xF0) < 0xF0) // If upper nibble of byte < F, set bits
      {
        word = word | mask;
      }

      mask = mask >> 2; // Rotate bit mask

      if ((rowBuffer[inIndex+k+l] & 0x0F) < 0x0F) // If lower nibble of byte < F, set bits
      {
        word = word | mask;
      }

      mask = mask >> 2; // Rotate bit mask
    }

  // Compose bytes and write word to output buffer
  unsigned char upperByte = (unsigned char) (word >> 8);
  outputBuffer[outputBufferIndex] = upperByte;
  outputBufferIndex++;
  unsigned char lowerByte = (unsigned char) word;
  outputBuffer[outputBufferIndex] = lowerByte;
  outputBufferIndex++;
  }
}

// Append array to output buffer
void writeArray(const unsigned char* array, int size)
{
  for (int i = 0; i < size; i++)
  {
    outputBuffer[outputBufferIndex] = array[i];
    outputBufferIndex++;
  }
}
