/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "bl.h"
#include "timer.h"

/* Private defines -----------------------------------------------------------*/
#define APP_BUFF_SIZE 1024
#define APPLICATION_BASE_ADDRESS 0x08003000

#define ACK  0x79
#define NACK 0x1f

#define BTL_ACK_TIMEOUT 4000000 // 4 sec.

#define MAX_WRITE_PACKET_SIZE 96

#define TIMER_EXPIRED(startTime, timeout) (TIME_CurrentTime() > (startTime + timeout))

/* variables ----------------------------------------------------------*/

static blStateType bootloadState;
static blStateType nextBootloadState;
static const u8 *image;
static u32 imageSize;
static readFunctionPointerType readByte;
static writeFunctionPointerType writeByte;
extern void halCommonMemCopy(void *dest, const void *src, u8 bytes);


/* Private functions ---------------------------------------------------------*/

//calculates checksum of n bytes located at address a
static u8 checksum(u8 *a, u16 n)
{
  u16 k;
  u8 result = 0;

  if (n == 1)
    result = (~a[0]) & 0xFF;
  else
    for(k=0; k<n; k++)
      result^= *(a+k);
  
  return result;
}

static u8 readBytesWithTimeout (u8 *data, u8 dataLength, u32 timeout)
{
  u8 returnValue = 1;
  u32 startTime;
  u8 i;
  
  for (i = 0; (i < dataLength) && (returnValue == 1); i++) {
    startTime = TIME_CurrentTime();
    while (1) {
      if (readByte(data + i)) {
	break;
      }
      if (TIMER_EXPIRED(startTime, timeout)) {
	returnValue = 0;
	break;
      }
    }
  }
  return returnValue;
}

u8 bootloadCommand(u8 cmdCode, btlCommandArgType *args, u8 replyLength, u8 *reply)
{
  u8 c;
  u8 returnValue = BOOTLOAD_COMMAND_SUCCESS;
  btlCommandArgType *arg;

  /* Send command code */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    writeByte(&cmdCode, 0);
    c = 255 - cmdCode;
    writeByte(&c, 1);

    if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
      returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  /* Send optional arguments */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    for (arg = args; arg->length != 0; arg++) {
      u8 i, c;
      for (i = 0; i < arg->length; i++)
	writeByte(&(arg->data[i]), 0);
      c = checksum(arg->data, arg->length);
      writeByte(&c, 1);

      if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
	returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
      } else {
	returnValue = BOOTLOAD_COMMAND_TIMEOUT;
      }
    }
  }
  
  /* Read possible reply */
  if (returnValue == BOOTLOAD_COMMAND_SUCCESS) {
    if (readBytesWithTimeout(reply, replyLength, BTL_ACK_TIMEOUT)) {
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  if ((returnValue == BOOTLOAD_COMMAND_SUCCESS) && 
      (cmdCode != READ) &&
      (cmdCode != WRITE) &&
      (cmdCode != WRITE_INCREMENTAL) &&
      (cmdCode != ERASE)) {
    if (readBytesWithTimeout(&c, 1, BTL_ACK_TIMEOUT)) {
      returnValue = (c != ACK) ? BOOTLOAD_COMMAND_NACKED : BOOTLOAD_COMMAND_SUCCESS;
    } else {
      returnValue = BOOTLOAD_COMMAND_TIMEOUT;
    }
  }

  return returnValue;
}

//rounds to power of 2 the remaining image fragment to upload towards target
u16 resizeImageUnit(u32 FragmentSize)
{
  if (FragmentSize >= MAX_WRITE_PACKET_SIZE)
    return MAX_WRITE_PACKET_SIZE;
  else
    return ((FragmentSize + 1) & ~1);
}  

void blInit (const u8* imagePtr, u32 imageSizeValue, writeFunctionPointerType writeByteFunc, readFunctionPointerType readByteFunc)
{
  bootloadState = BL_STATE_INIT;
  image = imagePtr;
  imageSize = imageSizeValue;
  writeByte = writeByteFunc;
  readByte = readByteFunc;
}

blStateType blStateMachine(u32 *bytesWritten)
{
#ifdef OTA_BL
  static u8 c;
#endif
  static u8 params[128];
  static u32 sentBytes = 0;     

  nextBootloadState = bootloadState;

  switch(bootloadState) { 
  case BL_STATE_INIT:
    { 
      sentBytes = 0;
#ifdef OTA_BL
      //enter boot mode
      resetExtensionBoard(0);
      //send 0x7F signal to startup the transfer speed calculation of the bootloader
      c = 0x7f;
      writeByte(&c);    
#endif             
      nextBootloadState = BL_STATE_INIT_OK;

    }
    break;
  case BL_STATE_INIT_OK:
#ifdef OTA_BL
    if (readByte(&c))
      {  
	//ack received here after 0x7F (sent at UNDEF state) is taken in account as initialization succeeded          
	if (c == ACK)
	  {
	    nextBootloadState = BL_STATE_GET;            
	  }    
      }
#else          
    nextBootloadState = BL_STATE_GET;            
#endif
    break;
  case BL_STATE_GET:
    {
      u8 reply[10];

      u8 state = bootloadCommand(GET, NULL, sizeof(reply), reply); 
      // u8 state = btlCommand(GET,NULL);
      if (state == BOOTLOAD_COMMAND_SUCCESS)
	{    
	  nextBootloadState = BL_STATE_GET_ID;       
	} else 
	{
	  nextBootloadState = BL_STATE_FAILURE;
	}
    }
    break;
  case BL_STATE_GET_ID:
    {
      u8 reply[3];

      //u8 state = btlCommand(GET_ID,NULL);
      u8 state = bootloadCommand(GET_ID, NULL, sizeof(reply), reply); 

      if (state == BOOTLOAD_COMMAND_SUCCESS) {            
	u8 y;
	u8 s = 0;
	u8 pages = ((u8) ((imageSize+1023) / 1024));
	
	params[0] = pages - 1;
	for(y=0;y < pages ;y++) 
	  { 
	    params[y+1] = y + 12;
	    s += y;
	  }
	s+=params[0];
	nextBootloadState = BL_STATE_ERASE;         
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_ERASE:
    {
      btlCommandArgType args[2];  
      args[0].length = params[0]+2;
      args[0].data = params;
      args[1].length = 0;
      //if (btlCommand(ERASE,params) == COMMAND_SUCCESS) {    
      if (bootloadCommand(ERASE,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {    
	nextBootloadState = BL_STATE_WRITE_SETUP;          
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_WRITE_SETUP:
    {
      u32 curraddress = APPLICATION_BASE_ADDRESS + sentBytes;
          
      params[3] = (u8) curraddress;
      params[2] = (u8)(curraddress >> 8);
      params[1] = (u8)(curraddress >> 16);
      params[0] = (u8)(curraddress >> 24);
          
      nextBootloadState = BL_STATE_WRITE;
    }
    break;
  case BL_STATE_WRITE: //Here is where we start writing the image
    {
      btlCommandArgType args[3];
      u8 *data = params + 4;
      
      args[0].length = 4;
      args[0].data = (u8 *) params;
      args[1].length = (resizeImageUnit(imageSize - sentBytes)) + 1;
      halCommonMemCopy(data + 1, image + sentBytes, args[1].length - 1); 
      data[0] = args[1].length - 2;
      args[1].data = data;
      args[2].length = 0;
      if (bootloadCommand(WRITE,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {
	sentBytes += data[0] + 1;
	if (sentBytes >= imageSize) {
	  u32 curraddress = APPLICATION_BASE_ADDRESS;
	  
	  params[3] = (u8) curraddress;
	  params[2] = (u8)(curraddress >> 8);
	  params[1] = (u8)(curraddress >> 16);
	  params[0] = (u8)(curraddress >> 24);
	  
	  nextBootloadState = BL_STATE_GO;
	} else {
	  nextBootloadState = BL_STATE_WRITE_INCREMENTAL;
	}
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_WRITE_INCREMENTAL: //Here is where we start writing the image
    {
      btlCommandArgType args[2];
      u8 *data = params + 4;
      
      args[0].length = (resizeImageUnit(imageSize - sentBytes)) + 1;
      halCommonMemCopy(data + 1, image + sentBytes, args[0].length - 1); 
      data[0] = args[0].length - 2;
      args[0].data = data;
      args[1].length = 0;
      if (bootloadCommand(WRITE_INCREMENTAL,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS) {
	sentBytes += data[0] + 1;
	if (sentBytes >= imageSize) {
	  u32 curraddress = APPLICATION_BASE_ADDRESS;
	  
	  params[3] = (u8) curraddress;
	  params[2] = (u8)(curraddress >> 8);
	  params[1] = (u8)(curraddress >> 16);
	  params[0] = (u8)(curraddress >> 24);
	  
	  nextBootloadState = BL_STATE_GO;
	} else {
	  nextBootloadState = BL_STATE_WRITE_INCREMENTAL;
	}
      } else {
	nextBootloadState = BL_STATE_FAILURE;
      }
    }
    break;
  case BL_STATE_GO:
    {
      btlCommandArgType args[2];
      args[0].length = 4;
      args[0].data = params;
      args[1].length = 0;

      if (bootloadCommand(GO,args, 0, NULL) == BOOTLOAD_COMMAND_SUCCESS)
	nextBootloadState = BL_STATE_IDLE;
      else
	nextBootloadState = BL_STATE_FAILURE;
    }
    break;
  case BL_STATE_FAILURE:
    nextBootloadState = BL_STATE_IDLE;
    break;
  case BL_STATE_IDLE:
    break;
  default:
    break;
  }
  bootloadState = nextBootloadState;
  *bytesWritten = sentBytes;
  
  return (bootloadState);
}

