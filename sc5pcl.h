// Command-line utility to convert monochrome MSX2/2+/tR .SC5 VRAM dumps to printable PCL5 commands
// tangent.space 2023

#ifndef sc5pcl_H
#define sc5pcl_H

#define SC5_FILE_SIZE 27143 // Size of SC5 file
#define OUTPUT_BUFFER_SIZE 29749 // Size of output PCL

#define TOTAL_ROWS 212 
#define BYTES_PER_ROW 128

#define EC 0x1B // PCL5 escape character

extern const unsigned char pclInit[];
extern const unsigned char pclRow[];
extern const unsigned char pclEnd[];

extern unsigned char *rowBuffer;
extern unsigned char *outputBuffer;

// Function declarations

void convertRow();
void writeArray(const unsigned char* array, int size);

#endif
