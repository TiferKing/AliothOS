
#include "aliothos.h"

__weak void OSTaskLoad(void)
{
  /* Load running task */
}

void OSMain(void *ptr)
{
  OSTaskLoad();
  while(1)
  {
    OSTaskPendPtr(OSTaskRunning);
  }
}