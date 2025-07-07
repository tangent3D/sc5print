// UNAPI functionality implementation
// tangent.space 2025

#include "sc5print.h"
#include <stdio.h>

// Include ASMLIB for UNAPI interop
#ifdef TARGET_MSX
#include "third-party/asm.h"
#endif

int init_tcp_connection()
{
  #ifdef TARGET_MSX

  if (UnapiGetCount("TCP") == 0) 
  {
    printf("No UNAPI TCP implementation found.\n");
    return 1;
  }

  if (UnapiGetCount("TCP") != 0) 
  {
    printf("UNAPI TCP implementation found.\n");
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