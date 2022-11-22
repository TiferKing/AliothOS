
#ifndef __CPU_H
#define __CPU_H

#include "stm8s.h"

#define OS_ENABLE_INT           enableInterrupts()
#define OS_DISABLE_INT          disableInterrupts()

#define OS_SWITCH_CONTEXT       asm(" TRAP")
#define OS_SWITCH_CONTEXT_IT    OSSwCtxInt()

//#define OS_RET_IT               asm(" IRET")

#define OS_GET_STACK            asm(" LDW   X, SP \n LDW   OSStackPtr, X")

#define OS_SET_STACK            asm(" LDW   X, OSStackPtr \n LDW   SP, X")

#define OS_CPU_CC_INT           0x20

#define __weak __attribute__((weak))

/************************************/

/* Signed integer types  */
typedef   signed char                                   INT8;
typedef   signed short                                  INT16;
typedef   signed long                                   INT32;

/* Unsigned integer types  */
typedef unsigned char                                   UINT8;
typedef unsigned short                                  UINT16;
typedef unsigned long                                   UINT32;

/************************************/

// Config
#define OS_CPU_SP                                       UINT16        //Define the type of sp in task array
#define OS_CPU_PTR                                      UINT32        //Define the type of pointer in task array
#define OS_MEM_PTR                                      UINT16        //Define the type of pointer in task array
#define OS_MEM_LEN                                      UINT8
#define OS_TASK_PTR                                     UINT8
#define OS_TASK_ID                                      UINT16
#define OS_TASK_PRI                                     UINT8
#define OS_TASK_STATUS                                  UINT8
#define OS_TASK_TIMEOUT                                 UINT16
#define OS_TASK_ERRVEC                                  UINT16
#define OS_EVENT_PTR                                    UINT8
#define OS_EVENT_ID                                     UINT8
#define OS_EVENT_TYPE                                   UINT8
#define OS_ADDRESS                                      UINT8
#define OS_ERROR                                        UINT8
#define OS_TASK_ARRAY_SIZE                              8             //Size of task array
#define OS_TASK_REMOTE_ARRAY_SIZE                       8
#define OS_EVENT_LOCAL_SIZE                             16
#define OS_EVENT_REMOTE_SIZE                            8
#define OS_EVENT_MASK_SIZE                              ((OS_EVENT_LOCAL_SIZE + OS_EVENT_REMOTE_SIZE - 1) / 8 + 1)
#define OS_EVENT_VEC_SIZE                               ((OS_EVENT_LOCAL_SIZE + OS_EVENT_REMOTE_SIZE - 1) / 8 + 1)
#define OS_CPU_SP_TOP                                   0x3FF
#define OS_MEMORY_BLOCK_SIZE                            16
#define OS_MEMORY_BLOCK_NUM                             16

#define OS_TASKARRAY_BASE                               0x50
#define OS_TASKREMOTEARRAY_BASE                         (OS_TASKARRAY_BASE + OS_TASK_ARRAY_SIZE * sizeof(OS_Task))
#define OS_LOCALEVENTARRAY_BASE                         (OS_TASKREMOTEARRAY_BASE + OS_TASK_REMOTE_ARRAY_SIZE * sizeof(OS_Task_Remote))
#define OS_REMOTEEVENTARRAY_BASE                        (OS_LOCALEVENTARRAY_BASE + OS_EVENT_LOCAL_SIZE * sizeof(OSEvent))
#define OS_EVENT_OCCUR_VEC_BASE                         (OS_TASKARRAY_BASE + OS_EVENT_LOCAL_SIZE * sizeof(OSEvent) + OS_EVENT_REMOTE_SIZE * sizeof(OSEvent) + OS_TASK_ARRAY_SIZE * sizeof(OS_Task) + OS_TASK_REMOTE_ARRAY_SIZE * sizeof(OS_Task_Remote))
#define OS_BASE_ARRAY_OFFSET                            OS_TASKARRAY_BASE
#define OS_BASE_ARRAY_SIZE                              (OS_EVENT_LOCAL_SIZE * sizeof(OSEvent) + OS_EVENT_REMOTE_SIZE * sizeof(OSEvent) + OS_TASK_ARRAY_SIZE * sizeof(OS_Task) + OS_TASK_REMOTE_ARRAY_SIZE * sizeof(OS_Task_Remote) + OS_EVENT_VEC_SIZE)
#define OS_MEMORY_MGR_BASE                              (((OS_BASE_ARRAY_OFFSET + OS_BASE_ARRAY_SIZE - 1) & 0xFF0) + 0x010)
#define OS_MEMORY_FREE_BASE                             (OS_MEMORY_MGR_BASE + OS_MEMORY_BLOCK_NUM)

#define OS_PORT_FUNC(x)                                 ((void (*)(UINT8*, UINT8, UINT8))(x))

typedef struct OS_Stack_Type
{
  unsigned char PCE_INT;
  unsigned char  PCH_INT;
  unsigned char  PCL_INT;
  unsigned char  CriticalCounter;
  unsigned char  VirtualReg[16];
  unsigned char  CC;
  unsigned char  A;
  unsigned char  XL;
  unsigned char  XH;
  unsigned char  YL;
  unsigned char  YH;
  unsigned char  PCE;
  unsigned char  PCH;
  unsigned char  PCL;
} OS_Stack;

typedef struct OS_Task_Type OS_Task_t;

typedef struct OS_Task_Remote_Type OS_Task_Remote_t;
typedef struct OSEvent_Type OSEvent_t;
// Suffix "_t" is keep out warning of typedef  

extern UINT8 OSCriticalCounter                 @ 0x010;
extern UINT8 OSRunning                         @ 0x011;
extern UINT8 OSTimerLeft                       @ 0x012;
extern UINT16 OSTimerMiss                      @ 0x013;
extern OS_CPU_SP OSStackPtr                    @ 0x015;
extern OSEvent_t* OSLocalEventArray            @ 0x017;
extern OSEvent_t* OSRemoteEventArray           @ 0x019;

extern OS_TASK_PTR OSTaskArraySize             @ 0x01B;
extern OS_TASK_PTR OSTaskRemoteArraySize       @ 0x01C;
extern OS_TASK_PTR OSTaskRunning               @ 0x01D;
extern OS_CPU_SP OSStackTop                    @ 0x01E;

extern OS_Task_t* OSTaskArray                  @ 0x020;
// An array store the information of local running task or remote controlled task 
// but actually running in local cpu.
extern OS_Task_Remote_t* OSTaskRemoteArray     @ 0x022;
// An array store the task controlled by this cpu but actually running in remote
// cpu. This usually used when an remote task is waitting an event in local cpu.
extern OS_MEM_PTR OSMemoryManager              @ 0x024;
extern OS_MEM_PTR OSMemoryBase                 @ 0x026;
extern OS_ADDRESS OSHSSrcAddress               @ 0x028;
extern OS_ADDRESS OSHSDstAddress               @ 0x029;
extern OS_ADDRESS OSLSSrcAddress               @ 0x02A;
extern OS_ADDRESS OSLSDstAddress               @ 0x02B;
extern OS_MEM_PTR OSEventOccurList             @ 0x02C;
extern OS_TASK_PTR OSTaskCnt                   @ 0x02E;
extern UINT8 Reserve                           @ 0x02F;

extern OS_MEM_PTR OSHSSendBuffer               @ 0x030;
extern OS_MEM_PTR OSHSRecvBuffer               @ 0x032;
extern OS_MEM_PTR OSLSSendBuffer               @ 0x034;
extern OS_MEM_PTR OSLSRecvBuffer               @ 0x036;
extern UINT8 OSHSSendBufferLen                 @ 0x038;
extern UINT8 OSHSRecvBufferLen                 @ 0x039;
extern UINT8 OSLSSendBufferLen                 @ 0x03A;
extern UINT8 OSLSRecvBufferLen                 @ 0x03B;
extern volatile UINT8 OSHSSendStatus           @ 0x03C;
extern volatile UINT8 OSHSRecvStatus           @ 0x03D;
extern volatile UINT8 OSLSSendStatus           @ 0x03E;
extern volatile UINT8 OSLSRecvStatus           @ 0x03F;

extern OS_CPU_PTR OSHSBusSend                  @ 0x040;
extern OS_CPU_PTR OSHSBusRecv                  @ 0x044;
extern OS_CPU_PTR OSLSBusSend                  @ 0x048;
extern OS_CPU_PTR OSLSBusRecv                  @ 0x04C;

void OSInitStack(OS_Stack *stk, unsigned long func, unsigned long arg);

void OSSwCtxInt(void);

void I2C_IRQService(void);

#endif /* __CPU_H */