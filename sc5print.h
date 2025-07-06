// Command-line utility to convert monochrome MSX2/2+/tR
// .SC5 VRAM dumps to printable PCL5 commands
// tangent.space 2025

#ifndef sc5print_H
#define sc5print_H

#define SC5_FILE_SIZE 27143     // Size of SC5 file
#define PCL_FILE_SIZE 29749     // Size of output PCL

#define TOTAL_SC5_ROWS 212      // 212 SC5 rows
#define BYTES_PER_SC5_ROW 128   // 128 bytes row of SC5 data (256 pixels, 1 pixel =  4 bits)
#define BYTES_PER_PCL_ROW 64    // 64 bytes (32 words) row of converted PCL raster data

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

extern unsigned char *inputRowBuffer;     // Buffer SC5 row from file (128 bytes)
extern unsigned char *convertedRowBuffer; // Buffer PCL row (64 bytes) 

// Function declarations

int preflight(int argc, char* argv[]);
int init_file_input(int argc, char* argv[]);
int init_buffers();
void convert();
unsigned char* convert_row();
void output(const void *data, unsigned int length);
int init_file_output();
void cleanup();

#endif
