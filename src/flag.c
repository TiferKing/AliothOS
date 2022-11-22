
#include "aliothos.h"

#ifdef OS_FLAG_EN
OS_EVENT_PTR OSFlagCreate(OS_EVENT_ID id, UINT8 type, UINT8 is_local, OS_ERROR *err)
{
  OS_EVENT_PTR event;
  OSEnterCritical();
  event = OSEventCreate(id, is_local, err);
  if(*err == OS_SUCCESS)
  {
    OSLocalEventArray[event].Type = is_local ? OS_EVENT_FLAG : (OS_EVENT_BIT_REMOTE | OS_EVENT_FLAG);
    OSLocalEventArray[event].Payload.Flag.Type = type;
    OSMemset((char *)OSLocalEventArray[event].Payload.Flag.Mask, 0x00, OS_EVENT_MASK_SIZE);
  }
  OSExitCritical();
  return event;
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
OS_EVENT_PTR OSFlagGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err)
{
  return OSEventGet(id, address, err);
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
OS_ERROR OSFlagStatus(OS_EVENT_PTR flag)
{
  UINT8 i;
  OSEnterCritical();
  if(OSLocalEventArray[flag].Payload.Flag.Type == OS_FLAG_TYPE_AND)
  {
    for(i = 0; i < OS_EVENT_MASK_SIZE; i++)
    {
      if((OSLocalEventArray[flag].Payload.Flag.Mask[i] & ((UINT8 *)OSEventOccurList)[i]) != OSLocalEventArray[flag].Payload.Flag.Mask[i])
      {
        OSEventClear(flag);
        OSExitCritical();
        return OS_FAIL_FLAG_WAITING;
      }
    }
    OSExitCritical();
    return OS_SUCCESS;
  }
  else
  {
    for(i = 0; i < OS_EVENT_MASK_SIZE; i++)
    {
      if((OSLocalEventArray[flag].Payload.Flag.Mask[i] & ((UINT8 *)OSEventOccurList)[i]) != 0)
      {
        OSExitCritical();
        return OS_SUCCESS;
      }
    }
    OSEventClear(flag);
    OSExitCritical();
    return OS_FAIL_FLAG_WAITING;
  }
  
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagWait(OS_EVENT_PTR flag, OS_TASK_TIMEOUT timeout, UINT8 option, OS_ERROR *err)
{
  if(flag < OS_EVENT_LOCAL_SIZE)
  {
    // Local flag
    OSEnterCritical();
    if((OSLocalEventArray[flag].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_FLAG)
    {
      if(OSFlagStatus(flag) == OS_SUCCESS)
      {
        // Flag occured
        *err = OS_SUCCESS;
      }
      else
      {
        // Flag listening other events
        if(option == OS_FLAG_OPT_NORMAL)
        {
          // Waiting lock with block
          OSEventWait(flag, timeout, err);
          // Event occured or timeout
        }
        else if(option == OS_MUTEX_OPT_NOBLOCK)
        {
          // Return with no block
          *err = OS_FAIL_FLAG_WAITING;
        }
      }
    }
    else
    {
      // the event is not a mutex
      *err = OS_FAIL_EVENT_TYPE_ERROR;
    }
    OSExitCritical();
    return;
  }
  else
  {
    // Remote mutex
  }
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagSet(OS_EVENT_PTR event)
{
  ((UINT8 *)OSEventOccurList)[event / 8] |= 1 << event;
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagClear(OS_EVENT_PTR event)
{
  ((UINT8 *)OSEventOccurList)[event / 8] &= ~(1 << event);
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagOccur(OS_EVENT_PTR event, OS_ERROR *err)
{
  OS_EVENT_PTR i;
  OSEnterCritical();
  OSFlagSet(event);
  if(OSLocalEventArray[event].Type & OS_EVENT_BIT_FLAG == OS_EVENT_BIT_FLAG)
  {
    for(i = 0; i < OS_EVENT_LOCAL_SIZE; i++)
    {
      if((OSLocalEventArray[i].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_FLAG)
      {
        if(OSFlagStatus(i) == OS_SUCCESS)
        {
          OSEventOccur(i, OS_EVENT_OPT_ALL, err);
        }
      }
    }
  }
  OSExitCritical();
  return;
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagListen(OS_EVENT_ID flag, OS_EVENT_PTR listen, OS_ERROR *err)
{
  OSEnterCritical();
  if((OSLocalEventArray[flag].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_FLAG)
  {
    OSLocalEventArray[listen].Type |= OS_EVENT_BIT_FLAG;
    OSLocalEventArray[flag].Payload.Flag.Mask[listen / 8] |= 1 << listen;
    *err = OS_SUCCESS;
  }
  else
  {
    *err = OS_FAIL_EVENT_TYPE_ERROR;
  }
  OSExitCritical();
  return;
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagCancel(OS_EVENT_ID flag, OS_EVENT_PTR listen, OS_ERROR *err)
{
  OSEnterCritical();
  if((OSLocalEventArray[flag].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_FLAG)
  {
    OSLocalEventArray[listen].Type &= ~OS_EVENT_BIT_FLAG;
    OSLocalEventArray[flag].Payload.Flag.Mask[listen / 8] &= ~(1 << listen);
    *err = OS_SUCCESS;
  }
  else
  {
    *err = OS_FAIL_EVENT_TYPE_ERROR;
  }
  OSExitCritical();
  return;
}
#endif // OS_FLAG_EN

#ifdef OS_FLAG_EN
void OSFlagDelete(OS_EVENT_PTR flag, UINT8 option, OS_ERROR *err)
{
  if(option == OS_FLAG_OPT_NORMAL)
  {
    OSEventDelete(flag, err);
  }
  else
  {
    OSEventDeleteForce(flag, err);
  }
}
#endif // OS_FLAG_EN
