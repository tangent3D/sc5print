// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

#ifndef sc5print_H
#define sc5print_H

#define INPUT_BUFFER_SIZE 27143 // Size of SC5 file
#define OUTPUT_BUFFER_SIZE 29749 // Size of output PCL

// Pin definitions
#define PSTB_n  34
#define D0      35
#define D1      32
#define D2      33
#define D3      25
#define D4      26
#define D5      27
#define D6      14
#define D7      12

#define EC 0x1B // PCL5 escape character

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


extern char inBuffer[INPUT_BUFFER_SIZE];
extern char outBuffer[OUTPUT_BUFFER_SIZE];

// Function declarations
void ISR();
void convert();
void writeArray(const unsigned char* array, int size);
void writeByte(unsigned char byte);

#endif
