// UNAPI functionality implementation
// tangent.space 2025

#include "sc5print.h"
#include "tcp.h"
#include <stdio.h>
#include <string.h>

// Include ASMLIB for UNAPI interop
#ifdef TARGET_MSX

#include "arch/msx/asm.h"

unapi_code_block codeBlock;
Z80_registers regs;

#endif

int init_tcp_connection()
{
  #ifdef TARGET_MSX

  if (UnapiGetCount("TCP/IP") == 0) 
  {
    printf("No UNAPI TCP/IP implementation found.\n");
    return 1;
  }

  UnapiBuildCodeBlock(NULL, 1, &codeBlock);

  // Attempt to call UNAPI_GET_INFO and print the output
  regs.Bytes.A = 69;
  printf("A = %u\n", regs.Bytes.A);
  UnapiCall(&codeBlock, 0, &regs, REGS_MAIN, REGS_MAIN);
  printf("A = %u, HL = %04X, DE = %04X, BC = %04X\n", regs.Bytes.A, regs.Words.HL, regs.Words.DE, regs.Words.BC);
  // A = 0, HL = 6712, DE = 0101, BC = 0203

  unsigned char params[13] =
  {
    127, 0, 0, 1,        // Remote IP (127.0.0.1)
    0x23, 0x8C,          // Remote port (9100)
    0xFF, 0xFF,          // Local port
    0x00, 0x00,          // Timeout
    0x00,                // Flags
    0x00, 0x00           // Skip host name validation
  };

  regs.Words.HL = (unsigned int)&params[0];

  regs.Bytes.A = 69;
  printf("A = %u, HL = %04X\n", regs.Bytes.A, regs.Words.HL);

  UnapiCall(&codeBlock, TCPIP_TCP_OPEN, &regs, REGS_MAIN, REGS_MAIN);

  printf("A = %u, B = %u, C = %u\n", regs.Bytes.A, regs.Bytes.B, regs.Bytes.C);

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
  
  // Close all open transient TCP connections
  regs.Bytes.B = 0;
  UnapiCall(&codeBlock, TCPIP_TCP_CLOSE, &regs, REGS_MAIN, REGS_MAIN);

  #endif

  return 0;
}