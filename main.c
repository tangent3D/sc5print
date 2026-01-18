// tangent.space 2025

#include "sc5print.h"
#include "tcp.h"
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char* argv[])
{
  // Perform preflight checks on arguments
  if (init_preflight(argc, argv) != 0) goto fail;

  // Verify and open SC5 data for input, seek past SC5 header
  if (init_file_input(argc, argv) != 0) goto fail;

  if (init_tcp_params(argc, argv) != 0) goto fail;

  // Initialize memory allocation
  if (init_buffers() != 0) goto fail;

  // Open and prepare to append to debug output file 'OUT.PCL'
  if (output_mode == OUTPUT_FILE && init_file_output() != 0) goto fail;

  // Initialize UNAPI TCP connection
  if (output_mode == OUTPUT_TCP && init_tcp_connection() != 0) goto fail;

  // Convert SC5 data to PCL and output according to output mode
  convert();

  cleanup();

  return 0;

  fail:
    cleanup();
    return 1;
}

int init_preflight(int argc, char* argv[])
{
  if (argc < 3)
  {
      printf("Usage: SC5PCL <FILENAME> <IP_ADDRESS> <PORT>\n");
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

int init_tcp_params(int argc, char* argv[])
{
  // Get destination IPv4 address from user and load to UNAPI params
  unsigned int a, b, c, d;

  if (sscanf(argv[2], "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
  {
      printf("Invalid IPv4 address.\n");
      return 1;
  }

  if (a > 255 || b > 255 || c > 255 || d > 255)
  {
      printf("Invalid IPv4 address.\n");
      return 1;
  }

  params[0] = (unsigned char)a;
  params[1] = (unsigned char)b;
  params[2] = (unsigned char)c;
  params[3] = (unsigned char)d;

  // Optionally get destination port from user and load to UNAPI params
  if (argc > 3 && argv[3] != NULL)
  {
    if (argv[3][0] < '0' || argv[3][0] > '9')
    {
      printf("Invalid port.\n");
      return 1;
    }

    unsigned long port = strtoul(argv[3], NULL, 10);

    if (port > 65535)
    {
      printf("Invalid port.\n");
      return 1;
    }

    params[4] = (unsigned char)(port & 0x00FFul);
    params[5] = (unsigned char)((port >> 8) & 0x00FFul);
  }

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

// Write specified data to output buffer
void output(const void *data, unsigned int length)
{
  const unsigned char *bytes = (const unsigned char *)data;

  for (unsigned int i = 0; i < length; i++)
  {
    if (outputBufferIndex >= OUTPUT_BUFFER_SIZE)
    {
      output_flush();
    }

    outputBuffer[outputBufferIndex++] = bytes[i];
  }
}

// Flush data in output buffer according to output mode
void output_flush()
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
