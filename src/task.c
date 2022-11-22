
#include "aliothos.h"

void OSTaskArrayInit(OS_TASK_PTR pos, OS_TASK_ID id, OS_TASK_PRI priority, UINT8 stack_size, OS_CPU_SP func, OS_CPU_SP arg_ptr, UINT8 is_local)
{
  OSTaskArray[pos].ID = id;
  OSTaskArray[pos].Status = is_local ? OS_TASK_LOCAL_RDY : OS_TASK_REMOTE_RDY;
  OSTaskArray[pos].Priority = priority;
  OSTaskArray[pos].TaskPend = OS_TASK_PEND_NULL;
  OSTaskArray[pos].EventPend = OS_EVENT_NULL;
  OSTaskArray[pos].Timeout = 0;
  // Stack is going down
  OSTaskArray[pos].StackPtr = OSStackTop - sizeof(OS_Stack);
  OSTaskArray[pos].StackBottom = OSStackTop;
  OSStackTop -= stack_size;
  OSTaskArray[pos].StackTop = OSStackTop + 1;
  OSTaskArray[pos].ErrorVector = 0;
  OSInitStack((OS_Stack *)(OSTaskArray[pos].StackPtr + 1), func, arg_ptr);
}

void OSTaskArrayClean(OS_TASK_PTR pos)
{
  OSMemset((char *)&OSTaskArray[pos], 0x00, sizeof(OS_Task));
}

OS_ERROR OSTaskInsert(OS_TASK_PTR pos, OS_TASK_ID id, OS_TASK_PRI priority, UINT8 stack_size, OS_CPU_PTR func, OS_CPU_PTR arg_ptr, UINT8 is_local)
{
  OS_TASK_PTR i;
  OSEnterCritical();
  
  // Move lower priority task
  for(i = OSTaskArraySize; i > pos; i--)
  {
    OSTaskArray[i] = OSTaskArray[i - 1];
    if(OSTaskArray[i].TaskPend >= pos)
    {
      // Redirect TaskPend to new position
      OSTaskArray[i].TaskPend++;
    }
  }
  for(i = 0; i < pos; i++)
  {
    if(OSTaskArray[i].TaskPend >= pos)
    {
      // Redirect TaskPend to new position
      OSTaskArray[i].TaskPend++;
    }
  }
  OSTaskArrayInit(pos, id, priority, stack_size, func, arg_ptr, is_local);
  OSTaskArraySize++;
  
  OSExitCritical();
  return OS_SUCCESS;
}

OS_ERROR OSTaskRemove(OS_TASK_PTR pos)
{
  OS_TASK_PTR i;
  OSEnterCritical();
  
  if(OSTaskArray[pos].Status & OS_TASK_BIT_REMOTE)
  {
    //OSTaskArray[pos].Status |= OS_TASK_REMOTE_UNREGISTER;
  }
  else
  {
    // Move lower priority task
    for(i = pos; i < OSTaskArraySize - 1; i++)
    {
      OSTaskArray[i] = OSTaskArray[i + 1];
    }
    OSTaskArrayClean(OSTaskArraySize - 1);
    OSTaskArraySize--;
  }
  
  OSExitCritical();
  return OS_SUCCESS;
}

OS_ERROR OSTaskCreate(OS_TASK_ID id, void (*func)(void *arg_ptr), void *arg_ptr, UINT8 stack_size, OS_TASK_PRI priority, UINT8 is_local)
{
  OS_TASK_PTR i;
  
  if(OSTaskArraySize < OS_TASK_ARRAY_SIZE)
  {
    if(stack_size > sizeof(OS_Stack))
    {
      // Check unique id and priority
      for(i = 0; i < OSTaskArraySize; i++)
      {
        if(OSTaskArray[i].ID == id)
        {
          return OS_FAIL_TASK_ID_EXIST;
        }
        if(OSTaskArray[i].Priority == priority)
        {
          return OS_FAIL_TASK_PRIORITY_EXIST;
        }
      }
      
      // Insert task into task array order by priority
      for(i = 0; i < OSTaskArraySize + 1; i++)
      {
        if(OSTaskArray[i].Status != OS_TASK_DELETE &&
           OSTaskArray[i].Priority > priority)
        {
          return OSTaskInsert(i, id, priority, stack_size, (OS_CPU_PTR)func, (OS_CPU_PTR)arg_ptr, is_local);
        }
        else if(OSTaskArray[i].Status == OS_TASK_DELETE)
        {
          return OSTaskInsert(i, id, priority, stack_size, (OS_CPU_PTR)func, (OS_CPU_PTR)arg_ptr, is_local);
        }
      }
    }
    return OS_FAIL_TASK_STACK_INSUFFICIENT;
  }
  
  return OS_FAIL_TASK_FULL;
}

OS_ERROR OSTaskDelete(OS_TASK_ID id)
{
  OS_TASK_PTR i;
  for(i = 0; i < OSTaskArraySize; i++)
  {
    if(OSTaskArray[i].Status != OS_TASK_DELETE &&
       OSTaskArray[i].ID == id)
    {
        return OSTaskRemove(i);
    }
  }
  
  return OS_FAIL_TASK_NULL;
}

OS_TASK_PTR OSTaskGetPtr(OS_TASK_ID id)
{
  OS_TASK_PTR i;
  for(i = 0; i < OSTaskArraySize; i++)
  {
    if(OSTaskArray[i].ID == id)
    {
        return i;
    }
  }
  return OS_TASK_PTR_NULL;
}

OS_ERROR OSTaskPendPtr(OS_TASK_PTR task)
{
  OSEnterCritical();
  OSTaskArray[task].Status |= OS_TASK_BIT_PEND;
  OSRun();
  OSExitCritical();
  return OS_SUCCESS;
}

OS_ERROR OSTaskPendID(OS_TASK_ID id)
{
  OS_TASK_PTR task;
  OSEnterCritical();
  task = OSTaskGetPtr(id);
  if(task != OS_TASK_PTR_NULL)
  {
    OSExitCritical();
    return OSTaskPendPtr(task);
  }
  else
  {
    OSExitCritical();
    return OS_FAIL_TASK_NULL;
  }
}

OS_ERROR OSTaskSuspendPtr(OS_TASK_PTR task)
{
  OSEnterCritical();
  OSTaskArray[task].Status |= OS_TASK_BIT_SUSPENDED;
  OSRun();
  OSEnterCritical();
  return OS_SUCCESS;
}

OS_ERROR OSTaskSuspendID(OS_TASK_ID id)
{
  OS_TASK_PTR task;
  OSEnterCritical();
  task = OSTaskGetPtr(id);
  if(task != OS_TASK_PTR_NULL)
  {
    OSExitCritical();
    return OSTaskSuspendPtr(task);
  }
  else
  {
    OSExitCritical();
    return OS_FAIL_TASK_NULL;
  }
}

OS_ERROR OSTaskResumePtr(OS_TASK_PTR task)
{
  OSEnterCritical();
  OSTaskArray[task].Status &= ~(OS_TASK_BIT_TIMEOUT | OS_TASK_BIT_SUSPENDED | OS_TASK_BIT_PEND);
  OSRun();
  OSEnterCritical();
  return OS_SUCCESS;
}

OS_ERROR OSTaskResumeID(OS_TASK_ID id)
{
  OS_TASK_PTR task;
  OSEnterCritical();
  task = OSTaskGetPtr(id);
  if(task != OS_TASK_PTR_NULL)
  {
    OSExitCritical();
    return OSTaskResumePtr(task);
  }
  else
  {
    OSExitCritical();
    return OS_FAIL_TASK_NULL;
  }
}

/*OS_TASK_PTR OSTaskGetPend(OS_EVENT_PTR event)
{
  OS_TASK_PTR i;
  for(i = 0; i < OSTaskRunning; i++)
  {
    if(OSTaskArray[i].EventPend == event)
    {
      return i;
    }
  }
  return OSTaskRunning;
}*/
