/*******************************************************************************
* SimpleMAC sample application: transmit.c
*
* transmit.c handles packet transmission for the SimpleMAC sample application.
*
* COPYRIGHT 2010 STMicroelectronics. All rights reserved. 
*******************************************************************************/

#include PLATFORM_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hal/hal.h"
#include "include/phy-library.h"

#include "transmit.h"

//NOTE: The packet itself (and therefore TransmitCompleteIsrCallback) knows
//      if the packet needs an ACK and if the packet should be retransmitted.
//-inUse says a tx slot has a valid packet and the slot cannot be reused
//-sendNow tells the tick to send the packet now
//-dsShortAddr is the short address the packet is being sent to
//-packet is the actual data buffer being transmitted
//-sfdPtr says where SfdSentIsrCallback should write the SFD
typedef struct {
  boolean inUse;
  boolean sendNow;
  u16 dstShortAddr;
  u8 packet[128];
  u8 * sfdPtr;
} TxQueueType;
u8 txDirectQHead=0;
u8 txDirectQTail=0;
TxQueueType txDirectQ[TX_DIRECT_QUEUE_SIZE]={0,};
//The indirect transmit queue is exactly size 1.  This application can
//only hold a single indirect packet regardless of the number of planets.
TxQueueType txIndirectQ={0,};

//These variables are for the packet currently being transmitted.
boolean * txPacketInUse; //points to the relevant variable in the Q
u8   * txPacket;      //points to the relevant variable in the Q
u8   * txPacketSfd;   //points to the relevant variable in the Q
u8     txPacketAttemptsLeft=TX_ATTEMPTS_COUNT;
boolean   txPacketInFlight=FALSE;
boolean   txPacketIsDirect=TRUE;
u32    txPacketDroppedNotifcation=0;
boolean   framePendingPktEnroute=FALSE;
u16    framePendingMsTick;


void clearDirectQueue(void)
{
  u8 i;
  
  if(txPacketInFlight && txPacketIsDirect) {
    printf("Direct transmission in progess.  txDirectQ NOT cleared.\r\n");
    return;
  }
  
  txPacketAttemptsLeft=0;
  for(i=0;i<TX_DIRECT_QUEUE_SIZE;i++) {
    txDirectQ[i].inUse=FALSE;
  }
  txDirectQHead=0;
  txDirectQTail=0;
}


void clearIndirectQueue(void)
{
  if(txPacketInFlight && !txPacketIsDirect) {
    printf("Indirect transmission in progess.  txIndirectQ NOT cleared.\r\n");
    return;
  }
  
  txPacketAttemptsLeft=0;
  txIndirectQ.inUse=FALSE;
}


void packetIsDone(void)
{
  *txPacketInUse=FALSE;
  txPacketInFlight=FALSE;
}

void advanceHead(void)
{
  txDirectQHead++;
  if(txDirectQHead>=TX_DIRECT_QUEUE_SIZE) {
    txDirectQHead=0;
  }
}

void advanceTail(void)
{
  if(txPacketIsDirect) {
    txDirectQTail++;
    if(txDirectQTail>=TX_DIRECT_QUEUE_SIZE) {
      txDirectQTail=0; 
    }
  }
}


void indirectTxTick(void)
{
  StStatus status = ST_SUCCESS;

  //Send next available packet if there already isn't a packet in flight.
  if(txIndirectQ.inUse && txIndirectQ.sendNow && txPacketInFlight==FALSE) {
    //Linkup our current transmission to the Q.
    txPacketInUse = &txIndirectQ.inUse;
    txPacket = txIndirectQ.packet;
    txPacketSfd = txIndirectQ.sfdPtr;
    txPacketIsDirect = FALSE;
    txPacketInFlight = TRUE;
    txPacketAttemptsLeft = TX_ATTEMPTS_COUNT;
    //Now send the packet!
    if(txPacketSfd != NULL) {
      //Since we're going to inject the TX SFD into the packet, we have to
      //enable the SfdSentIsrCallback.  The SfdSentIsrCallback will then
      //disable itself when it is done injecting the SFD.
      ST_RadioEnableSfdSentNotification(TRUE);
    }
    status = ST_RadioTransmit(txPacket);
    assert(status==ST_SUCCESS);
    txPacketAttemptsLeft--;
  }
}


void directTxTick(void)
{
  StStatus status = ST_SUCCESS;
  //Send next available packet if there already isn't a packet in flight.
  if(txDirectQ[txDirectQTail].inUse==TRUE && txPacketInFlight==FALSE) {
    //Linkup our current transmission to the Q.
    txPacketInUse = &txDirectQ[txDirectQTail].inUse;
    txPacket = txDirectQ[txDirectQTail].packet;
    txPacketSfd = txDirectQ[txDirectQTail].sfdPtr;
    txPacketIsDirect = TRUE;
    txPacketInFlight = TRUE;
    txPacketAttemptsLeft = TX_ATTEMPTS_COUNT;
    //Now send the packet!
    if(txPacketSfd != NULL) {
      //Since we're going to inject the TX SFD into the packet, we have to
      //enable the SfdSentIsrCallback.  The SfdSentIsrCallback will then
      //disable itself when it is done injecting the SFD.
      ST_RadioEnableSfdSentNotification(TRUE);
    }
    status = ST_RadioTransmit(txPacket);
    assert(status==ST_SUCCESS);
    txPacketAttemptsLeft--;
  }
}


void txTick(void)
{
  indirectTxTick();
  directTxTick();
  if(txPacketDroppedNotifcation) {
    printf("Failed all transmit attempts.  Dropping the packet.\r\n");
    txPacketDroppedNotifcation--;
  }
}


void transmitIndirectPacketNow(void)
{
  txIndirectQ.sendNow=TRUE;
  indirectTxTick();
}


void directTransmit(u16 dstShortAddr, u8 * packet, u8 sfdPtrOffset)
{
  TxQueueType * headPkt;
  
  headPkt = &txDirectQ[txDirectQHead];
  if(headPkt->inUse) {
    printf("txDirectQ is full!  Dropping new TX packet.\r\n");
    return;
  }
  
  //Place new packet into the Q
  headPkt->sendNow=TRUE;
  headPkt->dstShortAddr=dstShortAddr;
  memcpy(headPkt->packet, packet, packet[0]+1);
  if(sfdPtrOffset > 0) {
    headPkt->sfdPtr=headPkt->packet+sfdPtrOffset;
  } else {
    headPkt->sfdPtr=NULL;
  }
  
  //Mark the new packet ready to be sent.
  headPkt->inUse=TRUE;
  
  //Make room for another packet in the Q.
  advanceHead();
}


void indirectTransmit(u16 dstShortAddr, u8 * packet, u8 sfdPtrOffset)
{
  if(txIndirectQ.inUse) {
    printf("txIndirectQ is full!  Dropping new TX packet.\r\n");
    return;
  }
  
  //Place new packet into the Q
  txIndirectQ.sendNow=FALSE;
  txIndirectQ.dstShortAddr=dstShortAddr;
  memcpy(txIndirectQ.packet, packet, packet[0]+1);
  if(sfdPtrOffset > 0) {
    txIndirectQ.sfdPtr=txIndirectQ.packet+sfdPtrOffset;
  } else {
    txIndirectQ.sfdPtr=NULL;
  }
  
  //Mark the new packet ready to be sent.
  txIndirectQ.inUse=TRUE;
}


void enqueueTxPacket(boolean direct,
                     u16 dstShortAddr,
                     u8 * packet,
                     u8 sfdPtrOffset)
{
  if(direct) {
    directTransmit(dstShortAddr, packet, sfdPtrOffset);
  } else {
    indirectTransmit(dstShortAddr, packet, sfdPtrOffset);
    
  }
  
  txTick();
}


void ST_RadioTransmitCompleteIsrCallback(StStatus status,
                                           u32 sfdSentTime,
                                           boolean framePending)
{
  StStatus status_code = ST_SUCCESS;
  TX_DETAILS(printf("ST_RadioTransmitCompleteIsrCallback\r\n");)
  TX_DETAILS(printf("sfdSentTime = 0x%05X\r\n", sfdSentTime);)
  TX_DETAILS(printf("framePending = %d\r\n", framePending);)
  TX_DETAILS(printf("status = 0x%02X - ", status);)
  
  switch(status) {
    case ST_SUCCESS:
      TX_DETAILS(printf("ST_SUCCESS\r\n");)
      //The inFlight packet required no ACK and was successfully sent,
      //therefore I am done with this packet.
      packetIsDone();
      advanceTail();
      break;
    case ST_PHY_ACK_RECEIVED:
      TX_DETAILS(printf("ST_PHY_ACK_RECEIVED\r\n");)
      //The inFlight packet required an ACK and is done
      packetIsDone();
      advanceTail();
      //Save the framePending bit and calculate the framePending timeout
      //so that sleepy devices stay awake to accept incoming packets.
      if(framePending) {
        framePendingPktEnroute = TRUE;
        framePendingMsTick = halCommonGetInt16uMillisecondTick();
      }
      break;
    case ST_MAC_NO_ACK_RECEIVED:
      TX_DETAILS(printf("ST_MAC_NO_ACK_RECEIVED\r\n");)
      //The inFlight packet required and ACK, which we didn't get, so
      //it must be retransmitted
      if(txPacketAttemptsLeft>0) {
        if(txPacketSfd != NULL) {
          ST_RadioEnableSfdSentNotification(TRUE);
        }
        status_code = ST_RadioTransmit(txPacket);
        assert(status_code==ST_SUCCESS);
        txPacketAttemptsLeft--;
      } else {
        txPacketDroppedNotifcation++;
        //Failed all transmit attempts.  Dropping the packet.
        packetIsDone();
        advanceTail();
      }
      break;
    case ST_PHY_TX_CCA_FAIL:
      TX_DETAILS(printf("ST_PHY_TX_CCA_FAIL\r\n");)
      //The inFlight packet never got sent and must be retransmitted
      if(txPacketAttemptsLeft>0) {
        if(txPacketSfd != NULL) {
          ST_RadioEnableSfdSentNotification(TRUE);
        }
        status_code = ST_RadioTransmit(txPacket);
        assert(status_code==ST_SUCCESS);
        txPacketAttemptsLeft--;
      } else {
        txPacketDroppedNotifcation++;
        //Failed all transmit attempts.  Dropping the packet.
        packetIsDone();
        advanceTail();
      }
      break;
    case ST_PHY_TX_UNDERFLOW:
      assert(FALSE);
      break;
    case ST_PHY_TX_INCOMPLETE:
      assert(FALSE);
      break;
    default:
      assert(FALSE);
  };
}


void ST_RadioSfdSentIsrCallback(u32 sfdSentTime)
{
  //Inject the SFD time into the packet being transmitted.
  txPacketSfd[0] = (sfdSentTime>> 0)&0xFF;
  txPacketSfd[1] = (sfdSentTime>> 8)&0xFF;
  txPacketSfd[2] = (sfdSentTime>>16)&0xFF;
  //Mark the SFD time as valid.  This is important to do because if
  //this ISR doesn't operate fast enough the packet might be gone before
  //the time is fully injected.
  txPacketSfd[2] |= 0x80;
  
  //To prevent this IsrCallback from being triggered more times than desired,
  //it is only enabled immediately before specific transmissions and then
  //immediately disabled here.
  ST_RadioEnableSfdSentNotification(FALSE);
}


boolean isDataPendingForShortId(u16 shortId)
{
  return ((shortId==txIndirectQ.dstShortAddr)&&txIndirectQ.inUse) ? TRUE:FALSE;
}


boolean ST_RadioDataPendingShortIdIsrCallback(u16 shortId)
{
  return isDataPendingForShortId(shortId);
}


//This application does not use long IDs with indirect transmissions.
boolean ST_RadioDataPendingLongIdIsrCallback(u8* longId)
{
  return FALSE;
}

