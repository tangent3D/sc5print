// tangent.space 2025

#include "sc5print.h"
#include "tcp.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

// FIXME: Set output_mode according to arguments
OutputMode output_mode = OUTPUT_TCP;

FILE *read_ptr = NULL;
FILE *write_ptr= NULL;
unsigned char *inputRowBuffer = NULL;
unsigned char *convertedRowBuffer = NULL;
unsigned char *outputBuffer = NULL;
unsigned int outputBufferIndex = 0;

// PCL5 command constants
const unsigned char pclInit[] =
{
  EC, '%', '-', '1', '2', '3', '4', '5', 'X',           // Exit language
  EC, 'E',                                              // Printer reset
  EC, '&', 'l', '2', '6', 'A',                          // Page size (A4)
  EC, '&', 'l', '1', 'O',                               // Landscape
  EC, '&', 'l', '1', 'E',                               // Top margin
  EC, '*', 'p', '6', '7', '8', 'x', '3', '4', '0', 'Y', // Position cursor
  EC, '*', 't', '7', '5', 'R',                          // Resolution (75 DPI)
  EC, '*', 'r', '0', 'F',                               // Presentation method
  EC, '*', 'r', '1', 'A'                                // Start raster graphics
};                                                      

const unsigned char pclRow[] =
{
  EC, '*', 'b', '6', '4', 'W'                           // 64 bytes per row
};

const unsigned char pclEnd[] =
{
  EC, '*', 'r', 'C',                                    // End raster graphics
  EC, 'E',
  EC, '%', '-', '1', '2', '3', '4', '5', 'X'
};

static const unsigned int nibbleMask[8] =
{
  0b1100000000000000,
  0b0011000000000000,
  0b0000110000000000,
  0b0000001100000000,
  0b0000000011000000,
  0b0000000000110000,
  0b0000000000001100,
  0b0000000000000011
};

int main(int argc, char* argv[])
{
  // Perform preflight checks on arguments
  if (preflight(argc, argv) != 0) goto fail;

  // Verify and open SC5 data for input, seek past SC5 header
  if (init_file_input(argc, argv) != 0) goto fail;

  // Initialize memory allocation
  if (init_buffers() != 0) goto fail;

  // Open and prepare to append to debug output file 'OUT.PCL'
  if (output_mode == OUTPUT_FILE && init_file_output() != 0) goto fail;

  // Initialize UNAPI TCP connection
  if (output_mode == OUTPUT_TCP && init_tcp_connection() != 0) goto fail;

  // Convert SC5 data to PCL and output according to output mode
  convert();

  // Program finished successfully  
  cleanup();
  return 0;

  fail:
    cleanup();
    return 1;
}

int preflight(int argc, char* argv[])
{
  if (argc == 1)
  {
      printf("Usage: SC5PCL FILE.SC5\n");
      return 1;
  }

  return 0;
}

int init_file_input(int argc, char* argv[])
{
  // Get SC5 file from argument
  read_ptr = fopen(argv[1], "rb");

  // Verify file exists
  if (read_ptr == NULL)
  {
    printf("File not found.\n");
    return 1;
  }

  // Verify file size
  fseek(read_ptr, 0, SEEK_END);

  // Verify file is not empty
  if (ftell(read_ptr) == 0)
  {
      printf("File contains no data.\n");
      return 1;
  }

  // Verify file is at least 27,143 bytes
  if (ftell(read_ptr) < SC5_FILE_SIZE)
  {
    printf("Invalid file size.\nValid SC5 files are >=27,143 bytes.\n");
    return 1;
  }

  // Skip SC5 header (7 bytes))
  fseek(read_ptr, 7, SEEK_SET);

  return 0;
}

int init_buffers()
{
  // 128 byte buffer (one row of SC5 data)
  inputRowBuffer = malloc(sizeof *inputRowBuffer * BYTES_PER_SC5_ROW);
  if (inputRowBuffer == NULL) return 1;

  // 64 byte buffer (one row of converted PCL raster data)
  convertedRowBuffer = malloc(sizeof *convertedRowBuffer * BYTES_PER_PCL_ROW);
  if (convertedRowBuffer == NULL) return 1;

  // Global output buffer
  outputBuffer = malloc(sizeof *outputBuffer * OUTPUT_BUFFER_SIZE);
  if (outputBuffer == NULL) return 1;

  // Buffer initialization successful
  return 0;
}

int init_file_output()
{
  char outputFileName[13];
  snprintf(outputFileName, sizeof(outputFileName), "OUT.PCL");
  write_ptr = fopen(outputFileName, "wb");
  if (!write_ptr)
  {
    printf("Failed to write OUT.PCL.");
    return 1;
  }

  return 0;
}

// Convert SC5 data to PCL to output buffer
void convert()
{
  // Output PCL initialization
  output(pclInit, sizeof pclInit);

  // Convert each row in SC5 to PCL5
  // Output each row twice to double image size vertically
  for (int i = 0; i < TOTAL_SC5_ROWS; i++)
  {
    fread(inputRowBuffer, BYTES_PER_SC5_ROW, 1, read_ptr);
    convert_row();
    output(pclRow, sizeof pclRow);
    output(convertedRowBuffer, BYTES_PER_PCL_ROW);
    output(pclRow, sizeof pclRow);
    output(convertedRowBuffer, BYTES_PER_PCL_ROW);
  }

  // Output PCL ending
  output(pclEnd, sizeof pclEnd);

  // Flush any remaining data in output buffer
  flush_output();
}

// Convert each pixel row of SC5 data to PCL to buffer
void convert_row()
{
  unsigned int stretchedByte;
  unsigned char* out = convertedRowBuffer;

  // For each set of 4 input bytes = 8 nibbles = 8 VRAM pixels
  // Stretch horizontally to 16 pixels (2 PCL raster bytes)
  for (unsigned int inputSet = 0; inputSet < 128; inputSet += 4)
  {
    stretchedByte = 0b0000000000000000;

    // For each VRAM byte in set of four
    for (unsigned int inputByte = 0; inputByte < 4; inputByte++)
    {
      unsigned char byte = inputRowBuffer[inputSet + inputByte];

      // If upper nibble of byte < F, set bits
      unsigned char upper = byte & 0xF0;
      if (upper < 0xF0)
      {
        stretchedByte |= nibbleMask[inputByte * 2];
      }

      // If lower nibble of byte < F, set bits
      unsigned char lower = byte & 0x0F;
      if (lower < 0x0F)
      {
        stretchedByte |= nibbleMask[inputByte * 2 + 1];
      }
    }

  *out++ = (unsigned char)(stretchedByte >> 8);
  *out++ = (unsigned char)(stretchedByte);
  }
}

// Write specified data to output buffer
void output(const void *data, unsigned int length)
{
  const unsigned char *bytes = (const unsigned char *)data;

  for (unsigned int i = 0; i < length; i++)
  {
    if (outputBufferIndex >= OUTPUT_BUFFER_SIZE)
    {
      flush_output();
    }

    outputBuffer[outputBufferIndex++] = bytes[i];
  }
}

// Flush data in output buffer according to output mode
void flush_output()
{
  if (outputBufferIndex == 0) return;

  if (output_mode == OUTPUT_FILE)
  {
    fwrite(outputBuffer, 1, outputBufferIndex, write_ptr);
  }

  if (output_mode == OUTPUT_TCP)
  {
    send_tcp_data(outputBuffer, outputBufferIndex);
  }

  printf("Wrote %u bytes\n", outputBufferIndex);

  outputBufferIndex = 0;
}

// Clean up before exit
void cleanup()
{
  if (read_ptr) fclose(read_ptr);
  if (write_ptr) fclose(write_ptr);
  if (inputRowBuffer) free(inputRowBuffer);
  if (convertedRowBuffer) free(convertedRowBuffer);
  if (outputBuffer) free(outputBuffer);
  if (output_mode == OUTPUT_TCP) close_tcp_connection();
}