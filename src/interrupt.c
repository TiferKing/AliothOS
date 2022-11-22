
#include "aliothos.h"
//volatile UINT8 OSInterrupt = 0;

void OSTimerInt(void)
{
  if(OSTimerLeft > 0)
  {
    OSTimerMiss++;
    OSTimerLeft--;
  }
  else if(OSCriticalCounter == 0)
  {
    // System tick skip time up
    OSTimerLeft = OSTimeoutUpdate(OSTimerMiss);
    OSTimerMiss = 0;
    OSRunInt();
  }
  else
  {
    // System is in critical region
    OSTimerMiss++;
  }
}

void OSSoftInt(void)
{
  OSRunInt();
}

void OSSetTimerLeft(UINT8 tick)
{
  OSTimerLeft = OSTimerLeft > tick ? tick : OSTimerLeft;
}