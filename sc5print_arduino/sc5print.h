// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

#ifndef sc5print_H
#define sc5print_H

#define INPUT_BUFFER_SIZE 27143 // Size of SC5 file 
#define OUTPUT_BUFFER_SIZE 29749 // Size of output PCL

// Pin definitions
#define D0      12  // MSX printer port parallel data
#define D1      13
#define D2      14
#define D3      15
#define D4      16
#define D5      17
#define D6      18
#define D7      19
#define PSTB_n  21  // MSX printer port Strobe signal
#define OE_n    22  // SN74LVC245 /OE pin (tie to VCC with 4.7K resistor)

#define EC 0x1B // PCL5 escape character
extern const unsigned char pclInit[];
extern const unsigned char pclRow[];
extern const unsigned char pclEnd[];

extern char inBuffer[INPUT_BUFFER_SIZE];
extern char outBuffer[OUTPUT_BUFFER_SIZE];

// Function declarations
void convert();
void writeArray(const unsigned char* array, int size);
void writeByte(unsigned char byte);

#endif
