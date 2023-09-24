// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

#include <stdio.h>
#include "sc5print.h"

char inBuffer[INPUT_BUFFER_SIZE] = {};
char outBuffer[OUTPUT_BUFFER_SIZE] = {};
int outIndex;

extern const unsigned char pclInit[];
extern const unsigned char pclRow[];
extern const unsigned char pclEnd[];

void convert()
{
  outIndex = 0;

  writeArray(pclInit, sizeof pclInit); // Write PCL initialization to output buffer

  unsigned short int mask;
  unsigned short int word;
  unsigned char byte;

  // Skip first 7 bytes in buffer (SC5 header!)
  for (int j = 7; j < sizeof inBuffer; j = j + 128) // For each row in buffer (212 rows of 128 bytes)
  {
    for (int i = 0; i < 2; i++) // Write each raster row twice
    {
      writeArray(pclRow, sizeof pclRow); // Write PCL Transfer Raster Data by Row/Block
      for (int k = 0; k < 128; k = k + 4) // For each set of four VRAM bytes in row, compose one raster word
      {
        mask = 0b1100000000000000;
        word = 0b0000000000000000;

        for (int l = 0; l < 4; l++) // For each VRAM byte in set of four
        {
          if ((inBuffer[j+k+l] & 0xF0) < 0xF0) // If upper nibble of byte < F, set bits
          {
            word = word | mask;
          }

          mask = mask >> 2; // Rotate bit mask

          if ((inBuffer[j+k+l] & 0x0F) < 0x0F) // If lower nibble of byte < F, set bits
          {
              word = word | mask;
          }

          mask = mask >> 2; // Rotate bit mask
        }

        byte = (unsigned char) (word >> 8);
        writeByte(byte); // Write upper byte of word

        byte = (unsigned char) word;
        writeByte(byte); // Write lower byte of word
      }
    }
  }
  writeArray(pclEnd, sizeof pclEnd ); // Write PCL ending to output buffer
}

void writeArray(const unsigned char* array, int size) // Append array to output buffer
{
  for (int i = 0; i < size; i++, outIndex++)
  {
    outBuffer[outIndex] = array[i];
  }
}

void writeByte(unsigned char byte) // Append byte to output buffer
{
  outBuffer[outIndex] = byte;
  outIndex++;
}
