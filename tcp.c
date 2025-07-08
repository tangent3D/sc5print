// UNAPI functionality implementation
// tangent.space 2025

#include "sc5print.h"
#include "tcp.h"
#include <stdio.h>
#include <string.h>

// Include ASMLIB for UNAPI interop
#ifdef TARGET_MSX

#include "third-party/asm.h"

unapi_code_block codeBlock;

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

  unsigned char params[13] =
  {
    127, 0, 0, 1,        // Remote IP (127.0.0.1)
    0x23, 0x8C,          // Remote port (9100, big-endian)
    0xFF, 0xFF,          // Local port (0 = auto)
    0x00, 10,            // Timeout = 10 seconds
    0x00,                // Flags
    0x00, 0x00           // Skip host name validation
  };

  Z80_registers regs;
  memset(&regs, 0, sizeof regs);

  regs.Words.HL = (unsigned int)&params[0];

  printf("A = %u, HL = %04X\n", regs.Bytes.A, regs.Words.HL);

  UnapiCall(codeBlock, TCPIP_TCP_OPEN, &regs, REGS_MAIN, REGS_MAIN);

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
  
  #endif

  return 0;
}