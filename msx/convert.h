// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

#ifndef convert_H
#define convertRow_H

#define INPUT_BUFFER_SIZE 27143 // Size of SC5 file
#define OUTPUT_BUFFER_SIZE 29749 // Size of output PCL

#define EC 0x1B // PCL5 escape character

extern const unsigned char pclInit[];
extern const unsigned char pclRow[];
extern const unsigned char pclEnd[];

extern unsigned char *inBuffer;
extern unsigned char *outBuffer;

// Function declarations

void convert();
void convertRow();
void writeArray(const unsigned char* array, int size);
void writeWord();

#endif
