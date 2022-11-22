
#include "aliothboot.h"
#include "aliothos.h"

__weak void OSBootClock(void)
{
  /* Init timer */
}

void IIC_Send(UINT8 *buffer, UINT8 size, UINT8 address)
{
  OSLSSendBuffer = (OS_MEM_PTR)buffer;
  OSLSSendBufferLen = size;
  OSLSDstAddress = address;
  OSLSSendStatus = 0x01;
  I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , ENABLE);
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
  I2C_GenerateSTART(ENABLE);
  while(OSLSSendStatus)
  {
    OSDelay(5);
  }
}

void IIC_Recv(UINT8 *buffer, UINT8 size, UINT8 address)
{
  OSLSRecvBuffer = (OS_MEM_PTR)buffer;
  OSLSRecvBufferLen = size;
  OSLSDstAddress = address;
  OSLSRecvStatus = 0x01;
  I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , DISABLE);
  I2C_GenerateSTART(ENABLE);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(address, I2C_DIRECTION_RX);
  while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
  I2C_AcknowledgeConfig(I2C_ACK_CURR);
  while(OSLSRecvBufferLen > 0)
  {
    if (OSLSRecvBufferLen <= 1)
    {
      I2C_AcknowledgeConfig(I2C_ACK_NONE);
      I2C_GenerateSTOP(ENABLE);
      while ((I2C_GetFlagStatus(I2C_FLAG_RXNOTEMPTY) == RESET));
      *((UINT8 *)OSLSRecvBuffer) = I2C_ReceiveData();
      OSLSRecvBuffer++;
      OSLSRecvBufferLen--;
    }
    if (I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED) )
    {
      *((UINT8 *)OSLSRecvBuffer) = I2C_ReceiveData();
      OSLSRecvBuffer++;
      OSLSRecvBufferLen--;
    }
  }
}

//OS_Task Debug[4] @ 0x050;

__weak void OSBootVariable(void)
{
  /* Init system Variable */
  OSMemset((char *)0x10, 0x00, 0x40);
  OSStackTop = OS_CPU_SP_TOP;
  OSTaskArray = (OS_Task*)OS_TASKARRAY_BASE;
  OSTaskRemoteArray = (OS_Task_Remote*)OS_TASKREMOTEARRAY_BASE;
  OSLocalEventArray = (OSEvent*)OS_LOCALEVENTARRAY_BASE;
  OSRemoteEventArray = (OSEvent*)OS_REMOTEEVENTARRAY_BASE;
  
  OSMemoryManager = OS_MEMORY_MGR_BASE;
  OSMemoryBase = OS_MEMORY_FREE_BASE;
  OSHSSrcAddress = 0x01;
  OSHSDstAddress = 0x00;
  OSLSSrcAddress = 0x01;
  OSLSDstAddress = 0x00;

  OSHSSendBuffer = 0x0000;
  OSHSRecvBuffer = 0x0000;
  OSLSSendBuffer = 0x0000;
  OSLSRecvBuffer = 0x0000;
  OSHSSendBufferLen = 0x00;
  OSHSRecvBufferLen = 0x00;
  OSLSSendBufferLen = 0x00;
  OSLSRecvBufferLen = 0x00;
  OSEventOccurList = OS_EVENT_OCCUR_VEC_BASE;

  OSHSBusSend = 0x0;
  OSHSBusRecv = 0x0;
  OSLSBusSend = (OS_CPU_PTR)IIC_Send;
  OSLSBusRecv = (OS_CPU_PTR)IIC_Recv;
  
  OSMemset((char *)OS_BASE_ARRAY_OFFSET, 0x00, OS_BASE_ARRAY_SIZE);
  
  OSMemset((char *)OS_MEMORY_MGR_BASE, OS_MEMORY_ALLOCATE_FREE, OS_MEMORY_BLOCK_NUM);
  OSMemset((char *)OS_MEMORY_FREE_BASE, 0x00, OS_MEMORY_BLOCK_SIZE * OS_MEMORY_BLOCK_NUM);
  //Debug[1] = Debug[0];
}

__weak void OSBootPeripheral(void)
{
  /* Init peripheral devices */
}

__weak void OSBootStackSwitch(void)
{
  /* Set Bootloader SP to secure position */
}

__weak void OSBootTask(void)
{
  /* Init system main task */
  OSTaskCreate(0x00, OSMain, 0x00, 0x40, 0, 1);
}

void OSBoot(void)
{
  OSBootClock();
  OSBootVariable();
  OSBootPeripheral();
  OSBootStackSwitch();
  OSBootTask();

  OSStart();
}