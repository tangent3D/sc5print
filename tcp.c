// UNAPI functionality implementation
// tangent.space 2025

#include "sc5print.h"
#include <stdio.h>

// Include ASMLIB for UNAPI interop
#ifdef TARGET_MSX

#include "third-party/asm.h"

unapi_code_block tcpCode;

#endif

int init_tcp_connection()
{
  #ifdef TARGET_MSX

  if (UnapiGetCount("TCP/IP") == 0) 
  {
    printf("No UNAPI TCP/IP implementation found.\n");
    return 1;
  }

  UnapiBuildCodeBlock("TCP/IP", 0, &tcpCode);

  // Print the unapi_code_block for debugging purposes
  unsigned char slot;
  unsigned char segment;
  unsigned int address;  
  UnapiParseCodeBlock(&tcpCode, &slot, &segment, &address);
  printf("UNAPI code block: slot %u, segment %u, address %04X\n", slot, segment, address);

  const char ip[] = "127.0.0.1";
  unsigned char portBytes[2] = { 9100 >> 8, 9100 & 0xFF };

  Z80_registers regs;

  regs.Words.DE = (unsigned int)&ip[0];         // IP address "x.x.x.x"
  regs.Words.HL = (unsigned int)&portBytes[0];  // Port number (high, low)
  regs.Bytes.C = 0;                             // 'Active' connection mode
  regs.Bytes.A = 10;                            // Timeout (seconds)

  UnapiCall(&tcpCode, 0, &regs, REGS_MAIN, REGS_MAIN);

  if (regs.Bytes.A == 0)
  {
      printf("Connection opened successfully!\n");
      printf("Connection handle: %04X\n", regs.Words.HL);
  } 
  else
  {
      printf("TCP_OPEN failed.\n");
      printf("Error code: %u\n", regs.Bytes.A);
  }

  #endif

  return 0;
}

int send_tcp_data(const void *data, unsigned int length)
{
  #ifdef TARGET_MSX

  #endif

  return 0;
}

int close_tcp_connection()
{
  #ifdef TARGET_MSX
  
  #endif

  return 0;
}