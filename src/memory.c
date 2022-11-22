
#include "aliothos.h"

OS_MEM_PTR OSMalloc(OS_MEM_LEN size)
{
  UINT8 block;
  UINT8 block_need;
  UINT8 i;
  if(size > 0)
  {
    OSEnterCritical();
    block_need = (size - 1) / OS_MEMORY_BLOCK_SIZE + 1;
    block = block_need;
    for(i = 0; i < OS_MEMORY_BLOCK_NUM; i++)
    {
      if(((UINT8 *)OSMemoryManager)[i] == OS_MEMORY_ALLOCATE_FREE)
      {
        block--;
      }
      else
      {
        block = block_need;
      }
      if(block == 0)
      {
        // var block is now reused to represent index of block
        i++;
        block = i;
        for(i = i - block_need; i < block; i++)
        {
          ((UINT8 *)OSMemoryManager)[i] = block - block_need;
        }
        block -= block_need;
        OSExitCritical();
        return OSMemoryBase + (OS_MEMORY_BLOCK_SIZE * block);
      }
    }
    OSExitCritical();
    return OS_MEMORY_NULL;
  }
  else
  {
    return OS_MEMORY_NULL;
  }
}

void OSFree(OS_MEM_PTR pmem)
{
  UINT8 block;
  UINT8 i;
  if(pmem >= OSMemoryBase && pmem < (OSMemoryBase + OS_MEMORY_BLOCK_SIZE * OS_MEMORY_BLOCK_NUM))
  {
    OSEnterCritical();
    block = (pmem - OSMemoryBase) / OS_MEMORY_BLOCK_SIZE;
    for(i = block; i < OS_MEMORY_BLOCK_NUM; i++)
    {
      if(((UINT8 *)OSMemoryManager)[i] == block)
      {
        ((UINT8 *)OSMemoryManager)[i] = OS_MEMORY_ALLOCATE_FREE;
      }
    }
    OSExitCritical();
  }
}
