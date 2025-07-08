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
unsigned char connNum;

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
    192, 168 , 50, 79,      // Remote IP
    0x8C, 0x23,             // Remote port (9100)
    0xFF, 0xFF,             // Local port
    0x05, 0x0,              // Use default timeout value
    0x00,                   // Flags
    0x00, 0x00              // Skip host name validation
  };

  regs.Words.HL = (unsigned int)&params[0];

  UnapiCall(&codeBlock, TCPIP_TCP_OPEN, &regs, REGS_MAIN, REGS_MAIN);
  if (regs.Bytes.A != ERR_OK)
  {
    // Handle TCPIP_TCP_OPEN failure
    return 1;
  }
  connNum = regs.Bytes.B;

  printf("A = %u, B = %u, C = %u\n", regs.Bytes.A, regs.Bytes.B, regs.Bytes.C);

  do
  {
    UnapiCall(&codeBlock, TCPIP_WAIT, &regs, REGS_MAIN, REGS_MAIN);
    regs.Bytes.B = connNum;
    regs.Words.HL = 0;
    UnapiCall(&codeBlock, TCPIP_TCP_STATE, &regs, REGS_MAIN, REGS_MAIN);
    printf("A = %u, B = %u\n", regs.Bytes.A, regs.Bytes.B);

    if (regs.Bytes.A == ERR_NO_CONN)
    {
        printf("Connect failed: reason = %u\n", regs.Bytes.C);
    }

  } while((regs.Bytes.A) == ERR_OK && (regs.Bytes.B != TCP_STATE_ESTABLISHED));

  #endif

  return 0;
}

int send_tcp_data(const void *data, unsigned int length)
{
  #ifdef TARGET_MSX

  do
  {
    UnapiCall(&codeBlock, TCPIP_WAIT, &regs, REGS_MAIN, REGS_MAIN);
    regs.Bytes.B = connNum;   // Connection number
    regs.Words.DE = data;     // Address of the data to be sent
    regs.Words.HL = length;   // Length of the data to be sent
    regs.Bytes.C = 0;         // Flags
    UnapiCall(&codeBlock, TCPIP_TCP_SEND, &regs, REGS_MAIN, REGS_MAIN);
    printf("A = %u\n", regs.Bytes.A);

    if (regs.Bytes.A == ERR_BUFFER)
    {
            printf("Buffer is full.\n");
    }
  } while((regs.Bytes.A) == ERR_BUFFER);

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
