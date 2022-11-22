
#include "aliothos.h"

#ifdef OS_MUTEX_EN
OS_EVENT_PTR OSMutexCreate(OS_EVENT_ID id, UINT8 is_local, OS_ERROR *err)
{
  OS_EVENT_PTR event;
  OSEnterCritical();
  event = OSEventCreate(id, is_local, err);
  if(*err == OS_SUCCESS)
  {
    OSLocalEventArray[event].Type = is_local ? (OS_EVENT_MUTEX | OS_EVENT_BIT_OCCUR) : (OS_EVENT_BIT_REMOTE | OS_EVENT_MUTEX | OS_EVENT_BIT_OCCUR);
    OSLocalEventArray[event].Payload.Mutex.Lock = OS_MUTEX_FREE;
  }
  OSExitCritical();
  return event;
}
#endif // OS_MUTEX_EN

#ifdef OS_MUTEX_EN
OS_EVENT_PTR OSMutexGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err)
{
  return OSEventGet(id, address, err);
}
#endif // OS_MUTEX_EN

#ifdef OS_MUTEX_EN
void OSMutexLock(OS_EVENT_PTR mutex, OS_TASK_TIMEOUT timeout, UINT8 option, OS_ERROR *err)
{
  if(mutex < OS_EVENT_LOCAL_SIZE)
  {
    // Local mutex
    OSEnterCritical();
    if((OSLocalEventArray[mutex].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_MUTEX)
    {
      if(OSLocalEventArray[mutex].Payload.Mutex.Lock == OS_MUTEX_FREE)
      {
        // Get lock
        OSEventClear(mutex);
        OSLocalEventArray[mutex].Payload.Mutex.Lock = OS_MUTEX_LOCK;
        OSLocalEventArray[mutex].Payload.Mutex.Task = OSTaskArray[OSTaskRunning].ID;
        *err = OS_SUCCESS;
      }
      else
      {
        // Locked by others
        if(option == OS_MUTEX_OPT_NORMAL)
        {
          // Waiting lock with block
          OSEventWait(mutex, timeout, err);
          // Event occured or timeout
          if((OSLocalEventArray[mutex].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_MUTEX)
          {
            // Confirm the event not changed
            if(OSLocalEventArray[mutex].Payload.Mutex.Lock == OS_MUTEX_FREE)
            {
              OSEventClear(mutex);
              OSLocalEventArray[mutex].Payload.Mutex.Lock = OS_MUTEX_LOCK;
              OSLocalEventArray[mutex].Payload.Mutex.Task = OSTaskArray[OSTaskRunning].ID;
            }
          }
          else
          {
            *err = OS_FAIL_EVENT_TYPE_ERROR;
          }
        }
        else if(option == OS_MUTEX_OPT_NOBLOCK)
        {
          // Return with no block
          *err = OS_FAIL_EVENT_REJECT;
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
#endif // OS_MUTEX_EN

#ifdef OS_MUTEX_EN
void OSMutexFree(OS_EVENT_PTR mutex, OS_ERROR *err)
{
  if(mutex < OS_EVENT_LOCAL_SIZE)
  {
    // Local mutex
    OSEnterCritical();
    if((OSLocalEventArray[mutex].Type & OS_EVENT_TYPE_MASK) == OS_EVENT_MUTEX)
    {
      if(OSLocalEventArray[mutex].Payload.Mutex.Lock == OS_MUTEX_FREE)
      {
        // Mutex is free
        // Nothing to do
        *err = OS_SUCCESS;
      }
      else
      {
        // Mutex is locked
        if(OSLocalEventArray[mutex].Payload.Mutex.Task == OSTaskArray[OSTaskRunning].ID)
        {
          // Mutex unlock accept
          OSLocalEventArray[mutex].Payload.Mutex.Lock = OS_MUTEX_FREE;
          OSEventOccur(mutex, OS_EVENT_OPT_ONE, err);
          OSExitCritical();
          return;
        }
        else
        {
          // Not the locking task, reject
          *err = OS_FAIL_EVENT_REJECT;
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
#endif // OS_MUTEX_EN

#ifdef OS_MUTEX_EN
void OSMutexDelete(OS_EVENT_PTR mutex, UINT8 option, OS_ERROR *err)
{
  if(option == OS_MUTEX_OPT_NORMAL)
  {
    OSEventDelete(mutex, err);
  }
  else
  {
    OSEventDeleteForce(mutex, err);
  }
}
#endif // OS_MUTEX_EN