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

unsigned char connNum = 0;

unsigned char params[13] =
{
  0, 0, 0, 0,             // Remote IP placeholder (set by argument)
  0x8C, 0x23,             // Remote port (default = 9100)
  0xFF, 0xFF,             // Random local port
  0x05, 0x00,             // Timeout
  0x00,                   // Flags
  0x00, 0x00              // Skip host name validation
};

int init_tcp_connection()
{
  #ifdef TARGET_MSX

  // Test for active UNAPI implementation
  if (UnapiGetCount("TCP/IP") == 0)
  {
    printf("No UNAPI TCP/IP implementation found.\n");
    return 1;
  }

  UnapiBuildCodeBlock(NULL, 1, &codeBlock);

  // Test for valid local IP address
  regs.Words.HL = (unsigned int)&params[0];
  regs.Bytes.B = 1;
  UnapiCall(&codeBlock, TCPIP_GET_IPINFO, &regs, REGS_MAIN, REGS_MAIN);
  if (regs.Words.HL == 0 && regs.Words.DE == 0)
  {
    printf("Local IP not configured.\n");
    return 1;
  }

  // Open a TCP connection and set connection number (1-254)
  regs.Words.HL = (unsigned int)&params[0];
  UnapiCall(&codeBlock, TCPIP_TCP_OPEN, &regs, REGS_MAIN, REGS_MAIN);
  if (regs.Bytes.A == ERR_OK)
  {
    connNum = regs.Bytes.B;
  }
  else
  {
    printf("Error opening TCP connection.\n");
    return 1;
  }

  // Loop and wait while connection establishes
  do
  {
    UnapiCall(&codeBlock, TCPIP_WAIT, &regs, REGS_MAIN, REGS_MAIN);
    regs.Bytes.B = connNum;
    regs.Words.HL = 0;
    UnapiCall(&codeBlock, TCPIP_TCP_STATE, &regs, REGS_MAIN, REGS_MAIN);

    if (regs.Bytes.A == ERR_NO_CONN)
    {
      if (regs.Bytes.C == CLOSE_REASON_USER_TIMEOUT || regs.Bytes.C == CLOSE_REASON_TIMEOUT) printf("Connection timed out.\n");
      else printf("Connection closed: %u\n", regs.Bytes.C);
      return 1;
    }
  } while(regs.Bytes.A == ERR_OK && (regs.Bytes.B == TCP_STATE_SYN_SENT || regs.Bytes.B == TCP_STATE_SYN_RECEIVED));

  // Determine connection success or failure
  if (regs.Bytes.B == TCP_STATE_ESTABLISHED || regs.Bytes.B == TCP_STATE_CLOSE_WAIT) return 0;
  else return 1;

  #endif

  return 0;
}

int send_tcp_data(const void *data, unsigned int length)
{
  #ifdef TARGET_MSX

  if (connNum == 0 || connNum == 255) return 1;

  do
  {
    UnapiCall(&codeBlock, TCPIP_WAIT, &regs, REGS_MAIN, REGS_MAIN);
    regs.Bytes.B = connNum;                 // Connection number
    regs.Words.DE = (unsigned int)data;     // Address of the data to be sent
    regs.Words.HL = length;                 // Length of the data to be sent
    regs.Bytes.C = 0;                       // Flags
    UnapiCall(&codeBlock, TCPIP_TCP_SEND, &regs, REGS_MAIN, REGS_MAIN);

    if (regs.Bytes.A == ERR_NO_CONN)
    {
      printf("Failed sending data.\n");
      return 1;
    }

  } while((regs.Bytes.A) == ERR_BUFFER);

  if (regs.Bytes.A != ERR_OK)
  {
    return 1;
  }

  #endif

  return 0;
}

int close_tcp_connection()
{
  #ifdef TARGET_MSX

  if (connNum == 0 || connNum == 255) return 0;

  regs.Bytes.B = connNum;
  UnapiCall(&codeBlock, TCPIP_TCP_CLOSE, &regs, REGS_MAIN, REGS_MAIN);

  if (regs.Bytes.A == ERR_OK || regs.Bytes.A == ERR_NO_CONN)
  {
    connNum = 0;
    return 0;
  }
  else
  {
    printf("Error closing connection.\n");
  }

  #endif

  return 0;
}
