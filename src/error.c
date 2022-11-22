
#include "aliothos.h"

__weak void OSErrorStack(void)
{
  /* Call in error of stack */
  OS_DISABLE_INT;
  while(1)
  {
  }
}