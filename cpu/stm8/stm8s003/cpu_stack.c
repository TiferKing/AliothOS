
#include "cpu.h"

void OSInitStack(OS_Stack *stk, unsigned long func, unsigned long arg)
{
  stk->PCL = func & 0xFF;
  stk->PCH = (func >> 8) & 0xFF;
  stk->PCE = (func >> 16) & 0xFF;
  stk->CC = OS_CPU_CC_INT;
  stk->XL = arg & 0xFF;
  stk->XH = (arg >> 8) & 0xFF;
  stk->CriticalCounter = 0x00;
}

UINT8 OSCriticalCounter                 @ 0x010;
UINT8 OSRunning                         @ 0x011;
UINT8 OSTimerLeft                       @ 0x012;
UINT16 OSTimerMiss                      @ 0x013;
OS_CPU_SP OSStackPtr                    @ 0x015;
OSEvent_t* OSLocalEventArray            @ 0x017;
OSEvent_t* OSRemoteEventArray           @ 0x019;

OS_TASK_PTR OSTaskArraySize             @ 0x01B;
OS_TASK_PTR OSTaskRemoteArraySize       @ 0x01C;
OS_TASK_PTR OSTaskRunning               @ 0x01D;
OS_CPU_SP OSStackTop                    @ 0x01E;

OS_Task_t* OSTaskArray                  @ 0x020;
// An array store the information of local running task or remote controlled task 
// but actually running in local cpu.
OS_Task_Remote_t* OSTaskRemoteArray     @ 0x022;
// An array store the task controlled by this cpu but actually running in remote
// cpu. This usually used when an remote task is waitting an event in local cpu.
OS_MEM_PTR OSMemoryManager              @ 0x024;
OS_MEM_PTR OSMemoryBase                 @ 0x026;
OS_ADDRESS OSHSSrcAddress               @ 0x028;
OS_ADDRESS OSHSDstAddress               @ 0x029;
OS_ADDRESS OSLSSrcAddress               @ 0x02A;
OS_ADDRESS OSLSDstAddress               @ 0x02B;
OS_MEM_PTR OSEventOccurList             @ 0x02C;
OS_TASK_PTR OSTaskCnt                   @ 0x02E;
UINT8 Reserve                           @ 0x02F;

OS_MEM_PTR OSHSSendBuffer               @ 0x030;
OS_MEM_PTR OSHSRecvBuffer               @ 0x032;
OS_MEM_PTR OSLSSendBuffer               @ 0x034;
OS_MEM_PTR OSLSRecvBuffer               @ 0x036;
UINT8 OSHSSendBufferLen                 @ 0x038;
UINT8 OSHSRecvBufferLen                 @ 0x039;
UINT8 OSLSSendBufferLen                 @ 0x03A;
UINT8 OSLSRecvBufferLen                 @ 0x03B;
volatile UINT8 OSHSSendStatus           @ 0x03C;
volatile UINT8 OSHSRecvStatus           @ 0x03D;
volatile UINT8 OSLSSendStatus           @ 0x03E;
volatile UINT8 OSLSRecvStatus           @ 0x03F;

OS_CPU_PTR OSHSBusSend                  @ 0x040;
OS_CPU_PTR OSHSBusRecv                  @ 0x044;
OS_CPU_PTR OSLSBusSend                  @ 0x048;
OS_CPU_PTR OSLSBusRecv                  @ 0x04C;