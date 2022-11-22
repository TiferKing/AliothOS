
#include "aliothos.h"

OS_EVENT_PTR OSEventCreate(OS_EVENT_ID id, UINT8 is_local, OS_ERROR *err)
{
  OS_EVENT_PTR i;
  OS_EVENT_PTR event = OS_EVENT_NULL;
  
  OSEnterCritical();
  // Find a local blank
  for(i = 0; i < OS_EVENT_LOCAL_SIZE; i++)
  {
    if(event == OS_EVENT_NULL && OSLocalEventArray[i].Type == OS_EVENT_DELETED)
    {
      event = i;
    }
    if(OSLocalEventArray[i].ID == id)
    {
      *err = OS_FAIL_EVENT_ID_EXIST;
      OSExitCritical();
      return OS_EVENT_NULL;
    }
  }
  
  if(event != OS_EVENT_NULL)
  {
    OSLocalEventArray[event].ID = id;
    OSLocalEventArray[event].Type = is_local ? OS_EVENT_LOCAL_RAW : OS_EVENT_REMOTE_RAW;
    *err = OS_SUCCESS;
    OSExitCritical();
    return event;
  }
  else
  {
    *err = OS_FAIL_EVENT_FULL;
    OSExitCritical();
    return OS_EVENT_NULL;
  }
}

OS_EVENT_PTR OSEventGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err)
{
  OS_EVENT_PTR i;
  if(address == OS_ADDRESS_LOCAL)
  {
    for(i = 0; i < OS_EVENT_LOCAL_SIZE; i++)
    {
      if(OSLocalEventArray[i].ID == id)
      {
        *err = OS_SUCCESS;
        return i;
      }
    }
    *err = OS_FAIL_EVENT_NULL;
    return OS_EVENT_NULL;
  }
  else
  {
    // Get Remote task
    return OS_EVENT_NULL;
  }
}

void OSEventWait(OS_EVENT_PTR event, OS_TASK_TIMEOUT timeout, OS_ERROR *err)
{
  OSEnterCritical();
  if(OSLocalEventArray[event].Type & OS_EVENT_BIT_OCCUR)
  {
    //Event has already occured
    *err = OS_SUCCESS;
  }
  else
  {
    //Waiting for event
    OSTaskArray[OSTaskRunning].TaskPend = OSTaskGetPtr(OSLocalEventArray[event].Payload.Mutex.Task);
    OSTaskArray[OSTaskRunning].EventPend = event;
    if(timeout > 0)
    {
      OSTaskArray[OSTaskRunning].Status |= OS_TASK_BIT_TIMEOUT | OS_TASK_BIT_PEND;
      OSTaskArray[OSTaskRunning].Timeout = timeout;
    }
    else
    {
      OSTaskArray[OSTaskRunning].Status |= OS_TASK_BIT_PEND;
    }
    OSRun();
    if(OSLocalEventArray[event].Type & OS_EVENT_BIT_OCCUR)
    {
      *err = OS_SUCCESS;
    }
    else
    {
      *err = OS_FAIL_EVENT_TIMEOUT;
    }
  }
  OSExitCritical();
  return;
}

void OSEventClear(OS_EVENT_PTR event)
{
  OSLocalEventArray[event].Type &= ~OS_EVENT_BIT_OCCUR;
#ifdef OS_FLAG_EN
  OSFlagClear(event);
#endif // OS_FLAG_EN
  return;
}

void OSEventOccur(OS_EVENT_PTR event, UINT8 is_one, OS_ERROR *err)
{
  OS_TASK_PTR i;
  OSEnterCritical();
  OSLocalEventArray[event].Type |= OS_EVENT_BIT_OCCUR;
  for(i = 0; i < OSTaskRunning; i++)
  {
    if((OSTaskArray[i].Status & OS_TASK_BIT_PEND) && (OSTaskArray[i].EventPend == event))
    {
      // Switch to higher priority task
      OSTaskArray[i].Status &= ~(OS_TASK_BIT_TIMEOUT | OS_TASK_BIT_PEND);
      OSRun();
      if(is_one)
      {
#ifdef OS_FLAG_EN
        OSFlagOccur(event, err);
#endif // OS_FLAG_EN
        *err = OS_SUCCESS;
        OSExitCritical();
        return;
      }
    }
  }
#ifdef OS_FLAG_EN
  OSFlagOccur(event, err);
#endif // OS_FLAG_EN
  for(i = OSTaskRunning; i < OSTaskArraySize; i++)
  {
    if((OSTaskArray[i].Status & OS_TASK_BIT_PEND) && (OSTaskArray[i].EventPend == event))
    {
      // Set lower priority task to ready
      OSTaskArray[i].Status &= ~(OS_TASK_BIT_TIMEOUT | OS_TASK_BIT_PEND);
      if(is_one)
      {
        break;
      }
    }
  }
  *err = OS_SUCCESS;
  OSExitCritical();
  return;
}

void OSEventDelete(OS_EVENT_PTR event, OS_ERROR *err)
{
  OS_EVENT_PTR i;
  if(event < OS_EVENT_LOCAL_SIZE)
  {
    OSEnterCritical();
    for(i = 0; i < OSTaskArraySize; i++)
    {
      if((OSTaskArray[i].Status & OS_TASK_BIT_PEND) &&
         OSTaskArray[i].EventPend == event)
      {
        *err = OS_FAIL_EVENT_PEND;
        OSExitCritical();
        return;
      }
    }
    for(i = 0; i < OSTaskRemoteArraySize; i++)
    {
      if((OSTaskRemoteArray[i].Status & OS_TASK_BIT_PEND) &&
         OSTaskRemoteArray[i].EventPend == event)
      {
        *err = OS_FAIL_EVENT_PEND;
        OSExitCritical();
        return;
      }
    }
    OSMemset((char *)&OSLocalEventArray[event], 0x00, sizeof(OSEvent));
    *err = OS_SUCCESS;
    OSExitCritical();
    return;
  }
  else
  {
    // Remote Event
  }
}

void OSEventDeleteForce(OS_EVENT_PTR event, OS_ERROR *err)
{
  if(event < OS_EVENT_LOCAL_SIZE)
  {
    OSEnterCritical();
    OSMemset((char *)&OSLocalEventArray[event], 0x00, sizeof(OSEvent));
    *err = OS_SUCCESS;
    OSExitCritical();
    return;
  }
  else
  {
    // Remote Event
  }
}