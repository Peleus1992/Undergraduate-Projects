/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
* File Name          : talk.c
* Author             : MCD Application Team
* Version            : V1.0.1
* Date               : April 2010
* Description        : SimpleMac demo application. 
*                      Download the same application image on two STM32W108 MB851 
*                      platforms.
*                      Open 2 hyper terminals on the related USB Virtual COMs.
*                      What you you write on an hyperterminal is displayed on 
*                      the other and viceversa. 
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#include PLATFORM_HEADER
#include BOARD_HEADER
#include "hal/hal.h"
#include "hal/micro/cortexm3/uart.h"
#include "include/phy-library.h"
#include "hal/error.h"
#include "stdio.h"
#include "stdlib.h"

/* Private define ------------------------------------------------------------*/
#define TIMEOUT_VALUE 100
#define BUFFER_SIZE 64

/* Private variables ---------------------------------------------------------*/

/* radioTransmitConfig variables required from the SimpleMac library for packet 
   transmission */
RadioTransmitConfig radioTransmitConfig = {
  TRUE,  // waitForAck;
  FALSE, // checkCca;
  4,     // ccaAttemptMax;
  3,     // backoffExponentMin;
  5,     // backoffExponentMax;
  TRUE   // appendCrc;
};

/* generic data packet */
#pragma data_alignment=2
u8 txPacket[128] = { 
  0x0a, // length
  0x61, // fcf - intra pan, ack request, data
  0x08, // fcf - src, dst mode
  0x00, // seq
  0x04, // dst pan l
  0x16, // dst pan h
  0x04, // dst addr l
  0x16, // dst addr h
  0x00  // data
};

/* buffer for received packet */
u8 rxPacket[128];
/* received packet flag */
boolean packetReceived = FALSE;

/* packet transmission complete flag */
boolean txComplete = TRUE;

static unsigned int ledTime = 0;

/* Private Functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : sendSerialData
* Description    : It allows to transmit the data
* Input          : - lenght of the data
*                  - data to be transmitted
* Output         : None
* Return         : None
*******************************************************************************/
void sendSerialData(u8 length, u8 *data)
{

  while (txComplete == FALSE);

  halCommonMemCopy(txPacket+8, data, length);
  txPacket[0] = length + 2 + 7;
  txPacket[3]++; /* increment sequence number */

  txComplete = FALSE;
  ST_RadioTransmit(txPacket);
}/* end sendSerialData() */

/*******************************************************************************
* Function Name  : sendSerialData
* Description    : It processes serial commands
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void processSerialInput(void)
{
  static u8 buffer[BUFFER_SIZE];
  static u8 bufferSize = 0;
  static u32 bufferTimeout = TIMEOUT_VALUE;
  
  if (bufferSize ==0)
     bufferTimeout = TIMEOUT_VALUE;
  if (__io_getcharNonBlocking(buffer+bufferSize))
  {
     bufferSize+=1;
  }
  bufferTimeout--;
  if ((bufferTimeout == 0) || (bufferSize == BUFFER_SIZE)) {
    if (bufferSize > 0) {
      sendSerialData(bufferSize, buffer);
      bufferSize = 0;
    }
  } 
}/* end processSerialInput() */

/* ********************* SimpleMac callbacks functions ************************/

/*******************************************************************************
* Function Name  : ST_RadioReceiveIsrCallback
* Description    : Radio Receiver callback function
* Input          : - packet: received packet
*                  - ackFramePendingSet: frame pending bit in the received packet
*                  - time: MAC timer when the SFD was received
*                  - errors: numbers of correlator erros in the packet
*                  - rssi: energy detected in the packet
* Output         : None
* Return         : None
*******************************************************************************/
void ST_RadioReceiveIsrCallback(u8 *packet,
                                boolean ackFramePendingSet,
                                u32 time,
                                u16 errors,
                                s8 rssi)
{
  /* note this is executed from interupt context */
  u8 i;

  /* Copy the packet to a buffer that can be accessed from the main loop;
     don't do the copy if there is already a packet there being processed */
  if(packetReceived == FALSE) {
    for(i=0; i<=packet[0]; i++) {
      rxPacket[i] = packet[i];
    }
    packetReceived = TRUE;
  }
}/* end ST_RadioReceiveIsrCallback() */

/*******************************************************************************
* Function Name  : ST_RadioTransmitCompleteIsrCallback
* Description    : Radio Transmit callback function
* Input          : - status: status of the packet transmission
*                  - sfdSentTime: MAC timer when the SFD was sent 
*                  - framePending: TRUE if the received ACK indicates that data 
*                    is pending for this node
* Output         : None
* Return         : None
*******************************************************************************/
void ST_RadioTransmitCompleteIsrCallback(StStatus status,
                                         u32 sfdSentTime,
                                         boolean framePending)
{
  switch(status) {
    case ST_SUCCESS:
      break;
    case ST_PHY_TX_CCA_FAIL:
    case ST_MAC_NO_ACK_RECEIVED:
    case ST_PHY_ACK_RECEIVED:
      txComplete = TRUE;
      break;
    default:
      break;
  }
}/* end ST_RadioTransmitCompleteIsrCallback() */

/*******************************************************************************
* Function Name  : ST_RadioDataPendingShortIdIsrCallback
* Description    : Callback for Radio Short Id data pending
* Input          : shortId address
* Output         : TRUE/FALSE
* Return         : None
*******************************************************************************/
boolean ST_RadioDataPendingShortIdIsrCallback(u16 shortId)
{
  return FALSE;
}/* end ST_RadioDataPendingShortIdIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioDataPendingLongIdIsrCallback
* Description    : Callback for Radio Long  Id data pending 
* Input          : long id address
* Output         : TRUE/FALSE
* Return         : None
*******************************************************************************/
boolean ST_RadioDataPendingLongIdIsrCallback(u8* longId)
{
  return FALSE;
}/* ST_RadioDataPendingShortIdIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioOverflowIsrCallback
* Description    : Radio overflow event callback
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ST_RadioOverflowIsrCallback(void)
{
 
}/* end ST_RadioOverflowIsrCallback() */

/*******************************************************************************
* Function Name  : ST_RadioSfdSentIsrCallback
* Description    : Radio SFD sent event callback
* Input          : sfdSentTime:MAC timer when the SFD was sent 
* Output         : None
* Return         : None
*******************************************************************************/
void ST_RadioSfdSentIsrCallback(u32 sfdSentTime)
{
}/* end ST_RadioSfdSentIsrCallback() */


/*******************************************************************************
* Function Name  : ST_RadioMacTimerCompareIsrCallback
* Description    : Radio MAC timer comapre Event callback
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ST_RadioMacTimerCompareIsrCallback(void)
{
}/* end ST_RadioMacTimerCompareIsrCallback() */

/*******************************************************************************
* Function Name  : main.
* Description    : talk main routine.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main(void)
{
  u8 i;
  u32 seed;
  StStatus status = ST_SUCCESS;
  
  /* Initialization */
  halInit();
  ST_RadioGetRandomNumbers((u16 *)&seed, 2);
  halCommonSeedRandom(seed);
  uartInit(115200, 8, PARITY_NONE, 1);
  INTERRUPTS_ON();
 
  /* init leds */
  halInitLed(); 
  
  /* Initialize radio (analog section, digital baseband and MAC).
  Leave radio powered up in non-promiscuous rx mode */
  status = ST_RadioInit(ST_RADIO_POWER_MODE_RX_ON);
  assert(status==ST_SUCCESS); 

  /* Setup some node and pan ids.  The packet above is also sent to a device
     with the same node and pan id so that two nodes running this same image
     will talk to each other, even though its not right to have two nodes
     with the same node id */
  ST_RadioSetNodeId(0x1604);
  ST_RadioSetPanId(0x1604);

  printf("\r\nSimpleMAC (%s) Talk Application\r\n",SIMPLEMAC_VERSION_STRING);

  while(1) {

    processSerialInput();

    /* print out any packets that were received */
    if(packetReceived == TRUE) {
      for (i = 8; i <= rxPacket[0]; i++)
        putchar(rxPacket[i]);
      /* The packet has been processed, so free the single entry queue up */
      packetReceived = FALSE;
    }
    ledTime++;
    if (ledTime > 20000) 
    {
      halToggleLed(LED_D1);
      ledTime = 0;
    }
  }
}/* end main ()*/

