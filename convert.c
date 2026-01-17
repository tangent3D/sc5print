// tangent.space 2025

#include "sc5print.h"
#include "tcp.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

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
  output_flush();
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
