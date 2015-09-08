/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : serial_utils.h
* Author             : MCD Application Team
* Version            : V1.0
* Date               : November 2009
* Description        : Header file x for serial utilities  
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef _SERIAL_UTILS_H_
#define _SERIAL_UTILS_H_

/* Private define ------------------------------------------------------------*/

typedef boolean  (printfFlushHandler)(int8u *contents, 
                                      int8u length);


/* Functions Prototypes ------------------------------------------------------*/

int8u *writeHex(int8u *charBuffer, int16u value, int8u charCount);
boolean serialWriteString(const char * string);
boolean serialWaitSend(void);
boolean serialReadPartialLine(char *data, int8u max, int8u * index);
boolean serialWriteData(int8u *data, int8u length);
boolean serialWriteByte(int8u dataByte);
boolean serialReadByte(int8u *dataByte);

#endif /* _SERIAL_UTILS_H_*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
