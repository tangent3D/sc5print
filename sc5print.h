// tangent.space 2025

#ifndef SC5PRINT_H
#define SC5PRINT_H

#define SC5_FILE_SIZE 27143     // Size of SC5 file
#define PCL_FILE_SIZE 29749     // Size of output PCL

#define TOTAL_SC5_ROWS 212      // 212 SC5 rows
#define BYTES_PER_SC5_ROW 128   // 128 bytes row of SC5 data
#define BYTES_PER_PCL_ROW 64    // 64 bytes row of converted PCL raster data

#define OUTPUT_BUFFER_SIZE 512

#define EC 0x1B // PCL5 escape character

extern const unsigned char pclInit[];
extern const unsigned char pclRow[];
extern const unsigned char pclEnd[];

typedef enum
{
  OUTPUT_FILE,
  OUTPUT_TCP
} OutputMode;

extern OutputMode output_mode;

extern unsigned char *inputRowBuffer;     // SC5 row buffer
extern unsigned char *convertedRowBuffer; // PCL row buffer

extern unsigned char *outputBuffer;
extern unsigned int outputBufferIndex;

// Function declarations

int preflight(int argc, char* argv[]);
int init_file_input(int argc, char* argv[]);
int init_buffers();
int init_file_output();
void convert();
void convert_row();
void output(const void *data, unsigned int length);
void flush_output();
void cleanup();

#endif
