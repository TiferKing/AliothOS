
#ifndef __ALIOTHOS_H
#define __ALIOTHOS_H

#include "cpu.h"

#define OS_VERSION                                      0x0001

#define OS_MUTEX_EN
//#define OS_FLAG_EN

// OS address 
#define OS_ADDRESS_LOCAL                                0x00

// OS task status bit
#define OS_TASK_BIT_RDY                                 0x01
#define OS_TASK_BIT_TIMEOUT                             0x02
#define OS_TASK_BIT_SUSPENDED                           0x04
#define OS_TASK_BIT_PEND                                0x08
//#define OS_TASK_BIT_START                               0x10
//#define OS_TASK_BIT_REGISTER                            0x10
//#define OS_TASK_BIT_SYNC                                0x20
#define OS_TASK_BIT_CONVERT                             0x40
#define OS_TASK_BIT_REMOTE                              0x80
#define OS_TASK_BIT_ALL                                 0xFF

// OS task status
#define OS_TASK_DELETE                                  0x00
// Local task status define
#define OS_TASK_LOCAL_RDY                               0x01
#define OS_TASK_LOCAL_DLY                               0x02
#define OS_TASK_LOCAL_DLY_SUSPENDED                     0x06
#define OS_TASK_LOCAL_SUSPENDED                         0x04
#define OS_TASK_LOCAL_PEND                              0x08
#define OS_TASK_LOCAL_PEND_SUSPENDED                    0x0C
#define OS_TASK_LOCAL_DLY_PEND                          0x0A
#define OS_TASK_LOCAL_DLY_PEND_SUSPENDED                0x0E
//#define OS_TASK_LOCAL_START                             0x10
#define OS_TASK_LOCAL_CONVERT                           0x40
// Remote task status define
#define OS_TASK_REMOTE_RDY                              0x81
#define OS_TASK_REMOTE_DLY                              0x82
#define OS_TASK_REMOTE_DLY_SUSPENDED                    0x86
#define OS_TASK_REMOTE_SUSPENDED                        0x84
#define OS_TASK_REMOTE_PEND                             0x88
#define OS_TASK_REMOTE_PEND_SUSPENDED                   0x8C
#define OS_TASK_REMOTE_DLY_PEND                         0x8A
#define OS_TASK_REMOTE_DLY_PEND_SUSPENDED               0x8E
//#define OS_TASK_REMOTE_REGISTER                         0x90
//#define OS_TASK_REMOTE_CONFIRM                          0x80
//#define OS_TASK_REMOTE_SYNC                             0xA0
#define OS_TASK_REMOTE_CONVERT                          0xC0
//#define OS_TASK_REMOTE_UNREGISTER                       0xD0
// Task status mask
#define OS_TASK_MASK_RDY                                0x7F
#define OS_TASK_MASK_START                              0x50
// Task priority pend
#define OS_TASK_PEND_NULL                               0x00
#define OS_TASK_PTR_NULL                                0xFF

// OS function return value
#define OS_SUCCESS                                      0x00
#define OS_FAIL_UNKNOWN                                 0x01
// Task function fail define
#define OS_FAIL_TASK_FULL                               0x10
#define OS_FAIL_TASK_NULL                               0x11
#define OS_FAIL_TASK_ID_EXIST                           0x12
#define OS_FAIL_TASK_PRIORITY_EXIST                     0x13
#define OS_FAIL_TASK_STACK_INSUFFICIENT                 0x14
#define OS_FAIL_EVENT_FULL                              0x20
#define OS_FAIL_EVENT_NULL                              0x21
#define OS_FAIL_EVENT_ID_EXIST                          0x22
#define OS_FAIL_EVENT_PEND                              0x23
#define OS_FAIL_EVENT_TYPE_ERROR                        0x24
#define OS_FAIL_EVENT_TIMEOUT                           0x25
#define OS_FAIL_EVENT_REJECT                            0x26
#define OS_FAIL_FLAG_WAITING                            0x30

#define OS_SWITCH_PERIOD                                10

#define OS_EVENT_DELETED                                0x00
#define OS_EVENT_MUTEX                                  0x01
#define OS_EVENT_SEMAPHORE                              0x02
#define OS_EVENT_MAILBOX                                0x03
#define OS_EVENT_FLAG                                   0x04
#define OS_EVENT_QUEUE                                  0x05

#define OS_EVENT_LOCAL_RAW                              0x7F
#define OS_EVENT_REMOTE_RAW                             0xFF

#define OS_EVENT_BIT_REMOTE                             0x80
#define OS_EVENT_BIT_OCCUR                              0x40
#define OS_EVENT_BIT_FLAG                               0x20
#define OS_EVENT_TYPE_MASK                              0x0F
//#define OS_EVENT_MASK_TYPE                              0xF0
//#define OS_EVENT_MASK_DETAIL                            0x0F

#define OS_EVENT_ID_REMOTE                              0x80

#define OS_EVENT_NULL                                   0xFF
#define OS_EVENT_OPT_ONE                                0x01
#define OS_EVENT_OPT_ALL                                0x00

#define OS_MUTEX_FREE                                   0x00
#define OS_MUTEX_LOCK                                   0x01
#define OS_MUTEX_OPT_NORMAL                             0x00
#define OS_MUTEX_OPT_NOBLOCK                            0x01
#define OS_MUTEX_OPT_FORCE                               0x02

#define OS_FLAG_TYPE_AND                                0x00
#define OS_FLAG_TYPE_OR                                 0x01
#define OS_FLAG_OPT_NORMAL                              0x00
#define OS_FLAG_OPT_NOBLOCK                             0x01
#define OS_FLAG_OPT_FORCE                               0x02

#define OS_MEMORY_NULL                                  0x0000
#define OS_MEMORY_ALLOCATE_FREE                         0xFF

//////////////////////////////////////////////

#define OSEnterCritical() OSCriticalCounter++
#define OSExitCritical() OSCriticalCounter--

//////////////////////////////////////////////

typedef struct OS_Task_Type
{
  OS_TASK_ID ID;                     //Task ID
  OS_TASK_STATUS Status;             //Task status for both local and remote
  OS_TASK_PRI Priority;              //Task priority
  OS_TASK_PTR TaskPend;              //Task pend a lower priority task
  OS_EVENT_PTR EventPend;            //Task pend an event to occur
  OS_TASK_TIMEOUT Timeout;           //Task delay timeout
  OS_CPU_SP StackPtr;                //Task sp storage
  OS_CPU_SP StackBottom;             //Task stack bottom
  OS_CPU_SP StackTop;                //Task stack top
  OS_TASK_ERRVEC ErrorVector;        //Task error vector
} OS_Task;

typedef struct OS_Task_Remote_Type
{
  OS_TASK_ID ID;                     //Task ID
  OS_ADDRESS Address;                //Task remote address
  OS_TASK_STATUS Status;             //Task status for remote
  OS_EVENT_PTR EventPend;            //Task pend an local event to occur
} OS_Task_Remote;

OS_ERROR OSTaskInsert(OS_TASK_PTR pos, OS_TASK_ID id, OS_TASK_PRI priority, UINT8 stack_size, OS_CPU_PTR func, OS_CPU_PTR arg_ptr, UINT8 is_local);
OS_ERROR OSTaskRemove(OS_TASK_PTR pos);
OS_ERROR OSTaskCreate(OS_TASK_ID id, void (*func)(void *arg_ptr), void *arg_ptr, UINT8 stack_size, OS_TASK_PRI priority, UINT8 is_local);
OS_ERROR OSTaskDelete(OS_TASK_ID id);
OS_TASK_PTR OSTaskGetPtr(OS_TASK_ID id);
OS_ERROR OSTaskPendPtr(OS_TASK_PTR task);
OS_ERROR OSTaskPendID(OS_TASK_ID id);
OS_ERROR OSTaskSuspendPtr(OS_TASK_PTR task);
OS_ERROR OSTaskSuspendID(OS_TASK_ID id);
OS_ERROR OSTaskResumePtr(OS_TASK_PTR task);
OS_ERROR OSTaskResumeID(OS_TASK_ID id);
//OS_TASK_PTR OSTaskGetPend(OS_EVENT_PTR event);

//////////////////////////////////////////////

void OSDelay(UINT16 ticks);

//////////////////////////////////////////////

#ifdef OS_FLAG_EN
typedef struct OSFlag_Type
{
  UINT8 Type;
  UINT8 Mask[OS_EVENT_MASK_SIZE];
} OSFlag;
#endif // OS_FLAG_EN

#ifdef OS_MUTEX_EN
typedef struct OSMutex_Type
{
  UINT8 Lock;
  OS_TASK_ID Task;
} OSMutex;
#endif // OS_MUTEX_EN

typedef struct OSSemaphore_Type
{
  UINT8 Counter;
} OSSemaphore;

typedef struct OSMailbox_Type
{
  void *Mail;
  UINT8 Size;
} OSMailbox;

typedef struct OSQueue_Type
{
  void *Queue;
  UINT8 Size;
} OSQueue;

typedef union OSEventPayload_Type
{
#ifdef OS_FLAG_EN
  OSFlag      Flag;
#endif // OS_FLAG_EN

#ifdef OS_MUTEX_EN
  OSMutex     Mutex;
#endif // OS_MUTEX_EN
  
  OSSemaphore Semaphore;
  OSMailbox   Mailbox;
  OSQueue     Queue;
} OSEventPayload;

typedef struct OSEvent_Type
{
  OS_EVENT_ID ID;
  OS_EVENT_TYPE Type;
  OSEventPayload Payload;
} OSEvent;

//////////////////////////////////////////////

OS_EVENT_PTR OSEventCreate(OS_EVENT_ID id, UINT8 is_local, OS_ERROR *err);
OS_EVENT_PTR OSEventGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err);
void OSEventWait(OS_EVENT_PTR event, OS_TASK_TIMEOUT timeout, OS_ERROR *err);
void OSEventClear(OS_EVENT_PTR event);
void OSEventOccur(OS_EVENT_PTR event, UINT8 is_one, OS_ERROR *err);
void OSEventDelete(OS_EVENT_PTR event, OS_ERROR *err);
void OSEventDeleteForce(OS_EVENT_PTR event, OS_ERROR *err);


void OSTimerInt(void);
void OSSoftInt(void);
void OSSetTimerLeft(UINT8 tick);

//////////////////////////////////////////////

void OSRun(void);
void OSRunInt(void);
UINT8 OSTimeoutUpdate(UINT8 tick);
void OSStart(void);
void OSMemcpy(char *dest, char *src, UINT16 size);
void OSMemcpyDec(char *dest, char *src, UINT16 size);
void OSMemset(char *dest, char ch, UINT16 size);

//////////////////////////////////////////////

void OSErrorStack(void);

//////////////////////////////////////////////

#ifdef OS_MUTEX_EN
OS_EVENT_PTR OSMutexCreate(OS_EVENT_ID id, UINT8 is_local, OS_ERROR *err);
OS_EVENT_PTR OSMutexGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err);
void OSMutexLock(OS_EVENT_PTR mutex, OS_TASK_TIMEOUT timeout, UINT8 option, OS_ERROR *err);
void OSMutexFree(OS_EVENT_PTR mutex, OS_ERROR *err);
void OSMutexDelete(OS_EVENT_PTR mutex, UINT8 option, OS_ERROR *err);
#endif // OS_MUTEX_EN

//////////////////////////////////////////////

OS_MEM_PTR OSMalloc(OS_MEM_LEN size);
void OSFree(OS_MEM_PTR pmem);

//////////////////////////////////////////////

#ifdef OS_FLAG_EN
OS_EVENT_PTR OSFlagCreate(OS_EVENT_ID id, UINT8 type, UINT8 is_local, OS_ERROR *err);
OS_EVENT_PTR OSFlagGet(OS_EVENT_ID id, OS_ADDRESS address, OS_ERROR *err);
OS_ERROR OSFlagStatus(OS_EVENT_PTR flag);
void OSFlagWait(OS_EVENT_PTR flag, OS_TASK_TIMEOUT timeout, UINT8 option, OS_ERROR *err);
void OSFlagSet(OS_EVENT_PTR event);
void OSFlagClear(OS_EVENT_PTR event);
void OSFlagOccur(OS_EVENT_PTR event, OS_ERROR *err);
void OSFlagListen(OS_EVENT_ID flag, OS_EVENT_PTR listen, OS_ERROR *err);
void OSFlagCancel(OS_EVENT_ID flag, OS_EVENT_PTR listen, OS_ERROR *err);
void OSFlagDelete(OS_EVENT_PTR flag, UINT8 option, OS_ERROR *err);
#endif // OS_FLAG_EN

//////////////////////////////////////////////

void OSMain(void *ptr);

#endif /* __ALIOTHOS_H */