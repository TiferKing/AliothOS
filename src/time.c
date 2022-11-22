
#include "aliothos.h"

void OSDelay(UINT16 ticks)
{
  if (ticks > 0)
  {
    OSEnterCritical();
    
    // Set timeout to task status
    OSTaskArray[OSTaskRunning].Status |= OS_TASK_BIT_TIMEOUT;
    OSTaskArray[OSTaskRunning].Timeout = ticks;
    
    OSSetTimerLeft(ticks - 1);
    
    OSExitCritical();
    
    OSRun();
  }
}
