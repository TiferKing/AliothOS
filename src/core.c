
#include "aliothos.h"

// Run os kernel, called by system function
void OSRun(void)
{
  OS_SWITCH_CONTEXT;
  return;
}

// Using in OSRunInt for loop counter
// Run os kernel, called by interrput function
void OSRunInt(void)
{
  OSEnterCritical();
  
  // Get stack pointer
  OS_GET_STACK;
  // Check stack pointer in the stack space
  if((OSStackPtr < OSTaskArray[OSTaskRunning].StackBottom) && (OSStackPtr > OSTaskArray[OSTaskRunning].StackTop))
  {
    OSTaskArray[OSTaskRunning].StackPtr = OSStackPtr;
  }
  else
  {
    // Stack overflow
    OSErrorStack();
  }
  
  // Find next ready task to run
  for(OSTaskCnt = 0; OSTaskCnt < OSTaskArraySize; OSTaskCnt++)
  {
    if((OSTaskArray[OSTaskCnt].Status & OS_TASK_MASK_RDY) == OS_TASK_BIT_RDY)
    {
      OSTaskRunning = OSTaskCnt;
      OSStackPtr = OSTaskArray[OSTaskRunning].StackPtr;
      OSExitCritical();
      OS_SWITCH_CONTEXT_IT;
    }
    else if(((OSTaskArray[OSTaskCnt].Status & OS_TASK_BIT_PEND) == OS_TASK_BIT_PEND) && 
            (OSTaskArray[OSTaskCnt].TaskPend != OS_TASK_PEND_NULL) && 
            ((OSTaskArray[OSTaskArray[OSTaskCnt].TaskPend].Status & OS_TASK_MASK_RDY) == OS_TASK_BIT_RDY))
    {
      // Solve priority reverse
      OSTaskCnt = OSTaskArray[OSTaskCnt].TaskPend - 1;
      continue;
    }
  }
  OSExitCritical();
}

UINT8 OSTimeoutUpdate(UINT8 tick)
{
  OS_TASK_PTR i;
  UINT8 left;
  OSEnterCritical();
  
  // Tick is the total time for all missed ticks
  tick++;
  left = OS_SWITCH_PERIOD;
  for(i = 0; i < OSTaskArraySize; i++)
  {
    if(OSTaskArray[i].Status & OS_TASK_BIT_TIMEOUT)
    {
      if( OSTaskArray[i].Timeout > tick )
      {
        OSTaskArray[i].Timeout = OSTaskArray[i].Timeout - tick;
        left = left > OSTaskArray[i].Timeout ? OSTaskArray[i].Timeout : left;
      }
      else
      {
        OSTaskArray[i].Timeout = 0;
        OSTaskArray[i].Status &= ~OS_TASK_BIT_TIMEOUT & ~OS_TASK_BIT_PEND;
        left = 1;
      }
    }
  }
  
  OSExitCritical();
  return left - 1;
}

void OSStart(void)
{
  OSStackPtr = OSTaskArray[0].StackPtr;
  //OSTaskArray[0].Status &= ~OS_TASK_BIT_START;
  //OSTaskArray[0].Status |= OS_TASK_BIT_RDY;
  OSRunning = 1;
  //OS_ENABLE_INT;
  OS_SWITCH_CONTEXT_IT;
}

UINT16 OSVersion(void)
{
    return (OS_VERSION);
}

void OSMemcpy(char *dest, char *src, UINT16 size)
{
  while(size > 0)
  {
    *dest++ = *src++;
    size--;
  }
}

void OSMemcpyDec(char *dest, char *src, UINT16 size)
{
  while(size > 0)
  {
    *dest-- = *src--;
    size--;
  }
}

void OSMemset(char *dest, char ch, UINT16 size)
{
  while(size > 0)
  {
    *dest++ = ch;
    size--;
  }
}