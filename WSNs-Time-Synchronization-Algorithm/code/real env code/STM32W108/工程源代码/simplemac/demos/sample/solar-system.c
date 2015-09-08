/*******************************************************************************
* SimpleMAC sample application: solar-system.c
*
* It supports sun and planet roles. 
* Sun role allows user to form a network. 
* Planet role allows an user to join to a network created by a sun.
*
* COPYRIGHT 2010 STMicroelectronics. All rights reserved.                 
*******************************************************************************/
/*****************************************************************
文件名：solar-system.c
版本号：v1.0
创建日期：2012-4-1
作者：大连飞翔科技有限公司,  www.fesxp.com, support@fesxp.com
硬件描述：STM32W108,FES2440
主要函数描述：main()函数实现SUN节点和多个PLANET节点通信
修改日志：2012-4-28 by Cheng Zhu 审核，并删除不必要的部分
          2012-5-16 by Cheng Zhu 删除main函数中变量lastPeriodicEventsQsTick
          2012-5-16 by Cheng Zhu 注释JOIN_DENIED_PACKET中的无限循环
*****************************************************************/
#include PLATFORM_HEADER
#include BOARD_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "hal/hal.h"
#include "include/phy-library.h"
#include "hal/micro/cortexm3/iap_bootloader.h"

#include "transmit.h"

//RX_DETAILS is a debugging macro for printing receive details.
//WARNING:  THIS MACRO WILL CAUSE PRINTS FROM ISR CONTEXT!
#define RX_DETAILS(x)   
//This macro causes the main loop to print details of the periodic events
//and sleep events.
#define MAIN_DETAILS(x) x

//This mask is passed into halSleepForQsWithOptions, indicating that
//the chip should wake on UART receive activity (GPIO PB2).
#define UART_ACTIVE_MASK (0x00000400)

//Define the number of quarter seconds between periodic maintenance events.
//The periodic events do not need to occur very often, so run these
//events once a minute.
#define PERIODIC_MAINTENANCE_EVENTS_RATE (1*60*4)

//Define the number of entries in the planet table.
//#define PLANET_TABLE_SIZE (5)
#define PLANET_TABLE_SIZE (10)

//Define the number of energy readings that shold be taken on a given
//channel when forming a network and looking for a channel with the
//least ammount of activity.
#define ENERGY_SAMPLES_COUNT (32)

//每套实验箱对应一个PANID值，不同实验箱不应相同，以防干扰
#define MyPANID 0x101

RadioTransmitConfig radioTransmitConfig = {
  TRUE, //waitForAck
  TRUE, //checkCca
  RADIO_CCA_ATTEMPT_MAX_DEFAULT, //ccaAttemptMax
  RADIO_BACKOFF_EXPONENT_MIN_DEFAULT, //backoffExponentMin
  RADIO_BACKOFF_EXPONENT_MAX_DEFAULT, //backoffExponentMax
  TRUE  //appendCrc
};

//******************************************************************************
// Define values used in constructing and decoding packets

//These define the magic values needed when constructing the FCF of the
//packets.  802.15.4 defines these numbers.
#define FCF_DATA     (0x01)
#define FCF_MACCMD   (0x03)
#define FCF_ACKREQ   (0x20)
#define FCF_INTRAPAN (0x40)
#define FCF_NODST    (0x00)
#define FCF_SHORTDST (0x08)
#define FCF_LONGDST  (0x0C)
#define FCF_NOSRC    (0x00)
#define FCF_SHORTSRC (0x80)
#define FCF_LONGSRC  (0xC0)

//This app uses only two frame types: MAC Command and Data.
#define FT_DATA          (FCF_DATA)
#define FT_MAC_COMMAND   (FCF_MACCMD)

//The following seven payload types are used with FT_DATA and the numerical
//values are custom to this app.
#define PT_GENERIC_DATA  (0x00)
#define PT_SUN_SEARCH    (0x01)
#define PT_SUN_AVAILABLE (0x02)
#define PT_JOIN_REQUEST  (0x03)
#define PT_JOIN_ACCEPTED (0x04)
#define PT_JOIN_DENIED   (0x05)
#define PT_LEAVING       (0x06)
#define PT_TIME_DATA     (0x07)
#define PT_SLEEPING      (0x08)
#define PT_LED           (0x09)
#define PT_TRSEND        (0x0A)
//This payload type is used with FT_MAC_COMMAND and the numerical value
//is defined by 802.15.4.
#define PT_POLLING       (0x04)

//The packet type is identified by its frame type and its payload type.
//These are stored in different parts of the packet.  Since payload type
//requires a specific frame type, I roll these two values into definitions
//that are easier to read when switching on a decoded packet.  Frame type
//occupies the upper nibble and payload type occupies the lower.
#define GENERIC_DATA_PACKET  ((FT_DATA       <<4) | (PT_GENERIC_DATA <<0))
#define CHANGE_TIME_PACKET    ((FT_DATA       <<4) | (PT_TIME_DATA  <<0))
#define SYN_SLEEPING_WAITTIME ((FT_DATA       <<4) | (PT_SLEEPING   <<0))
#define SYN_LED_WAITTIME     ((FT_DATA       <<4) | (PT_LED   <<0))   
#define TRSEND_PACKET        ((FT_DATA       <<4) | (PT_TRSEND   <<0))  
#define SUN_SEARCH_PACKET    ((FT_DATA       <<4) | (PT_SUN_SEARCH   <<0))
#define SUN_AVAILABLE_PACKET ((FT_DATA       <<4) | (PT_SUN_AVAILABLE<<0))
#define JOIN_REQUEST_PACKET  ((FT_DATA       <<4) | (PT_JOIN_REQUEST <<0))
#define JOIN_ACCEPTED_PACKET ((FT_DATA       <<4) | (PT_JOIN_ACCEPTED<<0))
#define JOIN_DENIED_PACKET   ((FT_DATA       <<4) | (PT_JOIN_DENIED  <<0))
#define LEAVING_PACKET       ((FT_DATA       <<4) | (PT_LEAVING      <<0))
#define POLLING_PACKET       ((FT_MAC_COMMAND<<4) | (PT_POLLING      <<0))

//******************************************************************************
// Define data structures
typedef struct {
  boolean packetBeingProcessed;
  u8 packet[128];
  boolean ackFramePendingSet;
  u32 time;
  u16 errors;
  s8 rssi;
  u8 lqi;
} RadioReceiveType;
RadioReceiveType rxData = {FALSE,};

typedef struct {
  boolean active;
  u16 shortAddr;
  u8 longAddr[8];
} PlanetTableType;


//******************************************************************************
// Define global radio state and convenience macros
RadioPowerMode radioPowerState = ST_RADIO_POWER_MODE_OFF;
#define TURN_RADIO_ON()                             \
  do {                                              \
    ST_RadioWake();                               \
    radioPowerState = ST_RADIO_POWER_MODE_RX_ON; \
  } while(0)
#define TURN_RADIO_OFF()                          \
  do {                                            \
    ST_RadioSleep();                            \
    radioPowerState = ST_RADIO_POWER_MODE_OFF; \
  } while(0)
//Derek: 函数申明
void replySync();
//Derek: Sync消息所用全局变量
u32 T1=-1, T2=-1, T3=-1, T4=-1, T5=-1;
u32 back=-1;
double diff=0,p11=0,x=0;
double TT1=0, TT2=0, TT3=0, TT4=0, TT5=0;
double timeoffset;
void replySyncback();
void sendSync();
//******************************************************************************
// Define global network state; initialized when forming/joining/leaving
boolean activeInNetwork = FALSE;
boolean availableSunFound = FALSE;
boolean networkJoinedStopSearching = FALSE;
u8 shortAddrCounter = 0;
u8 currSeqNum = 0;
u8 autoSendRate = 0;
u16 autoSendRateDstShortAddr = 0x0000;
u16 autoSendLastQsTick;
u8 autoPollRate = 0;
u16 autoPollLastQsTick;
PlanetTableType planetTable[PLANET_TABLE_SIZE];
u16 counter=0;
u16 counter2=0;
//static u8 currnum=0;
//u16 shortaddr[PLANET_TABLE_SIZE];

#ifdef PLANET_ROLE
u8 planet_join_by_button = 0;
#endif

void sendVddDataPacket(u16 vddMillivolts,u16 dstShortAddr,boolean sendDirectly);

void initNetworkState(void)
{
  u8 i;
  
  activeInNetwork = FALSE;
  networkJoinedStopSearching = FALSE;
  shortAddrCounter = 0;
  currSeqNum = halCommonGetRandom()&0xFF; //802.15.4 says to start with a random seq num
  autoSendRate = 0;
  autoPollRate = 0;
  txPacketInFlight = FALSE;
  framePendingPktEnroute = FALSE;
  for(i=0;i<PLANET_TABLE_SIZE;i++) {
    planetTable[i].active = FALSE;
    planetTable[i].shortAddr = 0x0000;
    memset(planetTable[i].longAddr, 0, sizeof(planetTable[i].longAddr));
  }
  ST_RadioSetPanId(0xFFFF);  //return to broadcast PAN ID
  ST_RadioSetNodeId(0xFFFE); //return to long addressing node ID
  clearDirectQueue();
  clearIndirectQueue();
}


//******************************************************************************
// LQI calculation.
u8 calculateLqi(u16 chipErrors, u8 packetLength)
{
  //The chipErrors parameter is the 10 bit chip error count from the radio.
  //The packetLength parameter is the byte size of the packet, including
  //  the length byte at the start and the 2 crc bytes at the end.
  u32 lqi;
  
  //To obtain the LQI, convert the total chip errors in a packet (err/packet)
  //to the chip errors per byte (err/byte).  Scale up the chipErrors value
  //to maintain reasonable precision during the division.  We don't
  //care about packets that have more than 4 chip errors per byte (6.25% chip
  //error rate), so we scale up by a factor of 64.
  lqi = chipErrors << 6;
  lqi /= packetLength;
  
  //802.15.4 says that an LQI should represent the best link quality as 0xFF
  //and the worst LQI as 0x00.
  return (0xFF - (u8)((lqi > 0xFF) ? (0xFF) : (lqi)));
}


//******************************************************************************
// Packet reception processing

//ST_RadioReceiveIsrCallback executes in ISR context and copies the
//incoming packet into the rxData structure so it can be processed
//outside of ISR context by processRxPacket.
void ST_RadioReceiveIsrCallback(u8 *packet,
                                  boolean ackFramePendingSet,
                                  u32 time,
                                  u16 errors,
                                  s8 rssi)
{
  u8 i;
  RX_DETAILS(printf("#");)
  if(activeInNetwork) {
    //If there isn't already one being processed, save the
    //incoming packet into the app's buffer for processing
    //outside of ISR context.
    if(!rxData.packetBeingProcessed) {
      for(i=0;i<=packet[0];i++) {
        rxData.packet[i] = packet[i];
        //printf("%02X ",rxData.packet[i]);
      }
      //printf("\r\n");
   
      rxData.ackFramePendingSet = ackFramePendingSet;
      rxData.time = time;
      rxData.errors = errors;
      rxData.rssi = rssi;
      rxData.packetBeingProcessed = TRUE;
      //processRxPacket() in the main loop handles the rest
    }
  }
}

//processRxPacket handles decoding all received packets and taking action
//in response to received packets.  This function executes outside of
//ISR context and operates on the rxData structure.  The rxData structure
//is populated by ST_RadioReceiveIsrCallback.
void processRxPacket(void)
{
  boolean pktHasSrcPanId = FALSE;
  u8 srcAddrOffset = 0;
  u16 srcPanId=0xFFFF;
  u16 shortSrcAddr=0;
  u8 longSrcAddr[8]={0,};
  u8 payloadStart;
  u8 packetType;
  u8 packet[128];
 
  if(!rxData.packetBeingProcessed) {
    return;
  }
#ifdef PLANET_ROLE

  if(rxData.packet[13]=='S')
  { //LGZ:b1
    //printf("LGZ has reply");
    T2=halCommonGetInt32uMillisecondTick();
    replySync();
  }
/*
  if(rxData.packet[13]=='L')
    { 
      counter++;
   // printf("p11 already,This is the %d cycle\r\n",counter);
    printf("%d   ",counter);
    T2=0;
    T2=((((((T2|rxData.packet[17])<<8)|rxData.packet[16])<<8)|rxData.packet[15])<<8)|rxData.packet[14];
    
    
    u16 ww1,ww2,ww3,ww4,ww5,ww6,ww7,ww8;
    double mm;
    TT2=T2;
    TT5=T5;
    
    p11=TT2-TT5-diff;
    diff=TT2-TT5;
    if(p11<0)
    {
      p11=-p11;
      printf("-");
    }
    /*
    mm=diff;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("diff: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    
    
    mm=T5;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T5: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    mm=T2;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T2: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    //*0.9769625
    mm=p11;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    //printf("T2-T5-diff: %d",ww1); 
    printf("%d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    }
 else if(rxData.packet[13]=='S')
  { //LGZ:p11
    T5=halCommonGetInt32uMillisecondTick();
    //printf("T5 has start;\r\n");
    //rxData.packetBeingProcessed = 0;
    
  }*/
  
#endif
 
#ifdef SUN_ROLE
 //LGZ
  if((rxData.packet[13]=='r')&&(rxData.packet[14]=='p')&&(rxData.packet[15]=='0')&&(rxData.packet[16]=='y'))
  { 
    counter2++;
    //printf("This is the %d cycle\r\n",counter2);
    printf("%d   ",counter2);
    T4=halCommonGetInt32uMillisecondTick();
    T2=0;
    T2=((((((T2|rxData.packet[20])<<8)|rxData.packet[19])<<8)|rxData.packet[18])<<8)|rxData.packet[17];
    
    back=T2;
    //replySyncback();//LGZ:立刻回复p11
    //sendSync();
    
    T3=0;
    T3=((((((T3|rxData.packet[24])<<8)|rxData.packet[23])<<8)|rxData.packet[22])<<8)|rxData.packet[21];
    TT1=T1;
    TT2=T2;
    TT3=T3;
    TT4=T4;
    
    /*timeoffset=((TT2-TT1)+(TT3-TT4))-2*x;
    x=(((TT2-TT1)+(TT3-TT4)))/2;*/
    
    timeoffset=((TT2-TT1)+(TT3-TT4));
    
    
    if(timeoffset<0)
    {
      timeoffset=-timeoffset;
      printf("-");
    }
    //timeoffset=timeoffset*100;
    u16 ww1,ww2,ww3,ww4,ww5,ww6,ww7,ww8;
    double mm;
    /*
   printf("PLANET0:\r\n");
    mm=T1;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T1: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    mm=T2;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T2: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    mm=T3;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T3: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
    mm=T4;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("T4: %d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    */
      mm=timeoffset;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    //printf("((T2-T1)+(T3-T4)): %d",ww1); 
    printf("%d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
  } 

#endif
 
  
 /* RX_DETAILS(
    printf("\r\n");
    printf("RX:");
    for(i=0;i<=rxData.packet[0];i++) {
      printf(" %02X", rxData.packet[i]);
    }
    printf("\r\n");
    printf("ackFramePendingSet = %d\r\n", rxData.ackFramePendingSet);
    printf("time = 0x%05X\r\n", rxData.time);
    printf("errors = %d\r\n", rxData.errors);
    printf("rssi = %d\r\n", rxData.rssi);
  );*/
  //Derek 检测Sync消息

  //At a minimum there must be a dst PAN, a short dst addr, a short src
  //addr, and a payload/command type.  That's a minimum of 10 bytes.
  if(rxData.packet[0]<10) {
    RX_DETAILS(printf("Length byte too short\r\n");)
    goto stopProcessing;
  }
  
  //Determine the frame type.
  switch(rxData.packet[1]&0x7) {
    case FT_DATA:
      RX_DETAILS(printf("FT_DATA\r\n");)
      packetType = (FT_DATA<<4);
      break;
    case FT_MAC_COMMAND:
      RX_DETAILS(printf("FT_MAC_COMMAND\r\n");)
      packetType = (FT_MAC_COMMAND<<4);
      break;
    default:
      RX_DETAILS(printf("Unknown frame type\r\n");)
      goto stopProcessing;
  };
  
  //Is there a source PAN ID in the packet?
  if((rxData.packet[1]&0x40)!=0x40) {
    pktHasSrcPanId=TRUE;
    srcAddrOffset = 2;
  }
  
  //There are 4 possible addressing modes in our application:
  switch(rxData.packet[2]) {
    case FCF_SHORTDST + FCF_SHORTSRC:
      RX_DETAILS(printf("short dst, short src\r\n");)
      shortSrcAddr = (rxData.packet[8+srcAddrOffset+1]<<8)|
                     (rxData.packet[8+srcAddrOffset+0]<<0);
      payloadStart = 10+srcAddrOffset;
      if(pktHasSrcPanId) {
        srcPanId = (rxData.packet[9]<<8)|(rxData.packet[8]<<0);
      }
      break;
    case FCF_SHORTDST + FCF_LONGSRC:
      RX_DETAILS(printf("short dst, long src\r\n");)
      memcpy(longSrcAddr, (rxData.packet+8+srcAddrOffset), 8);
      payloadStart = 16+srcAddrOffset;
      if(pktHasSrcPanId) {
        srcPanId = (rxData.packet[9]<<8)|(rxData.packet[8]<<0);
      }
      break;
    case FCF_LONGDST + FCF_SHORTSRC:
      RX_DETAILS(printf("long dst, short src\r\n");)
      shortSrcAddr = (rxData.packet[14+srcAddrOffset+1]<<8)|
                     (rxData.packet[14+srcAddrOffset+0]<<0);
      payloadStart = 16+srcAddrOffset;
      if(pktHasSrcPanId) {
        srcPanId = (rxData.packet[15]<<8)|(rxData.packet[14]<<0);
      }
      break;
    case FCF_LONGDST + FCF_LONGSRC:
      RX_DETAILS(printf("long dst, long src\r\n");)
      memcpy(longSrcAddr, (rxData.packet+14+srcAddrOffset), 8);
      payloadStart = 22+srcAddrOffset;
      if(pktHasSrcPanId) {
        srcPanId = (rxData.packet[15]<<8)|(rxData.packet[14]<<0);
      }
      break;
    default:
      RX_DETAILS(printf("Unknown addressing mode\r\n");)
      goto stopProcessing;
  }
  
  RX_DETAILS(
    if(pktHasSrcPanId) {
      printf("src pan = 0x%04X\r\n", srcPanId);
    }
  )
  
  //Make sure the packet is long enough to obtain a payload type
  if(rxData.packet[0]<payloadStart) {
    RX_DETAILS(printf("Length byte too short\r\n");)
    goto stopProcessing;
  }
  
  //Finish deriving the packet type by obtaining the payload type.
  packetType |= (rxData.packet[payloadStart]<<0);
  RX_DETAILS(printf("packet type = 0x%02X\r\n", packetType);)
  
  switch(packetType) {
    case (GENERIC_DATA_PACKET): //executes on SUN and PLANET
      RX_DETAILS(printf("GENERIC_DATA_PACKET\r\n");)
#ifdef SUN_ROLE
    halToggleLed(LED_D1);
    halCommonDelayMilliseconds(500);
    halClearLed(LED_D1);
#endif
#ifdef PLANET_ROLE
    halToggleLed(LED_D3);
    halCommonDelayMilliseconds(500);
    halClearLed(LED_D3);    
#endif
    //The LQI calculation wants the entire packet length.  The length
    //byte in the received packet does not include the length byte
    //itself or the two CRC bytes.
    rxData.lqi = calculateLqi(rxData.errors, (rxData.packet[0]+3));
    /*printf("RX: Addr=0x%04X, VDD=%dmV, RxSFD=0x%05X, ",
             shortSrcAddr,
             ((rxData.packet[payloadStart+1]<<0)|
              (rxData.packet[payloadStart+2]<<8)),
             rxData.time);
    if(rxData.packet[payloadStart+5]&0x80) {
        //Only show the TX SFD if the transmitter was capable of getting
        //the SFD into the packet in time.
        rxData.packet[payloadStart+5] &= ~0x80;
        printf("TxSFD=0x%05X, ",
               ((rxData.packet[payloadStart+3]<< 0)|
                (rxData.packet[payloadStart+4]<< 8)|
                (rxData.packet[payloadStart+5]<<16)));
      } else {
        printf("TxSFD=-------, ");
      }
      
      printf("RSSI=%ddBm, LQI=0x%02X\r\n",
             rxData.rssi,
             rxData.lqi);*/
      break;
#ifdef PLANET_ROLE
    case (SYN_SLEEPING_WAITTIME):
        printf("SYN_SLEEPING_WAITTIME\r\n");
	halToggleLed(LED_D3);    
	halCommonDelayMilliseconds(1000);
        halClearLed(LED_D3);                
        break;
    case (SYN_LED_WAITTIME):
        printf("SYN_LED_WAITTIME\r\n");
	halToggleLed(LED_D3);    //连续3次
	halCommonDelayMilliseconds(500);
        halClearLed(LED_D3); 
        halCommonDelayMilliseconds(500);
        halToggleLed(LED_D3);    
	halCommonDelayMilliseconds(500);
        halClearLed(LED_D3); 
        halCommonDelayMilliseconds(500);
        halToggleLed(LED_D3);    
	halCommonDelayMilliseconds(500);
        halClearLed(LED_D3); 
        break;
#endif
    case (TRSEND_PACKET):
        sendVddDataPacket(0x0000,0x0001,TRUE);
        //printf("trsend success\r\n");
        break;
#ifdef SUN_ROLE
    case (SUN_SEARCH_PACKET): //executes on SUN
      RX_DETAILS(printf("SUN_SEARCH_PACKET\r\n");)
      //As a sun, if I have room in my planet table I transmit that
      //I am available.  Otherwise, I don't transmit anything.
      for(int i=0;i<PLANET_TABLE_SIZE;i++) {
        if(!planetTable[i].active) {
          //Length
          packet[0] = (24+2);
          packet[1] = FCF_DATA;
          packet[2] = FCF_LONGDST + FCF_LONGSRC;
          //New sequence number
          currSeqNum++;
          packet[3]=currSeqNum;
          //dst PAN ID
          packet[4] = (0xFFFF>>0)&0xFF;
          packet[5] = (0xFFFF>>8)&0xFF;
          //long dst addr
          memcpy((packet+6), longSrcAddr, 8);
          //src PAN ID
          packet[14] = (ST_RadioGetPanId()>>0)&0xFF;
          packet[15] = (ST_RadioGetPanId()>>8)&0xFF;
          //Long src addr
          memcpy((packet+16), ST_RadioGetEui64(), 8);
          //payload type
          packet[24] = PT_SUN_AVAILABLE;
          //Broadcast response
          enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
          break;
        }
      }
      break;
#endif
#ifdef PLANET_ROLE
    case (SUN_AVAILABLE_PACKET): //executes on PLANET
      RX_DETAILS(printf("SUN_AVAILABLE_PACKET\r\n");)
      //If I already found a sun, ignore any subsequent available messages
      if(availableSunFound) {
        return;
      }
      if(srcPanId!=MyPANID)
      {
         goto stopProcessing;
      }
      availableSunFound=TRUE;
      //As a planet, send a unicast join request to the sun.  But first,
      //switch to the sun's PAN in anticipation of completing the join.
      ST_RadioSetPanId(srcPanId);
      //Length
      packet[0] = (22+2);
      packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
      packet[2] = FCF_LONGDST + FCF_LONGSRC;
      //New sequence number
      currSeqNum++;
      packet[3]=currSeqNum;
      //dst PAN ID
      packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
      packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
      //long dst addr
      memcpy((packet+6), longSrcAddr, 8);
      //long src addr
      memcpy((packet+14), ST_RadioGetEui64(), 8);
      //payload type
      packet[22] = PT_JOIN_REQUEST;
      //Unicast join request
      enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
      break;
#endif
#ifdef SUN_ROLE
      case (JOIN_REQUEST_PACKET): //executes on SUN   // 收到请求加网的包
      RX_DETAILS(printf("JOIN_REQUEST_PACKET\r\n");)  //串口终端显示
      {
        u8 flag=0;
        u8 pt = PT_JOIN_DENIED;
        u8 assignedShortId[2] = {0xFE, 0xFF};
        //sun responds with unicast packet to long ID, acceptance, and the
        //assigned short ID as payload.  denied packets have a short ID 0xFFFE
        //Length
        packet[0] = (24+2);
        packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
        packet[2] = FCF_LONGDST + FCF_LONGSRC;
        //New sequence number
        currSeqNum++;
        packet[3]=currSeqNum;
        //dst PAN ID
        packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
        packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
        //long dst addr
        memcpy((packet+6), longSrcAddr, 8);
        //long src addr
        memcpy((packet+14), ST_RadioGetEui64(), 8);
        
        //sun looks for a free spot in the planet table
        //if none, sun sends a unicast join denied, then idles
        //if one, sun allocates the planet a short ID and sends a unicast
        //  join accepted to long id with short id in payload, then idles
        for(int i=0;i<PLANET_TABLE_SIZE;i++) {
          u8 k=0;                                            //搜寻表中是否存在与加网节点相同的64位长地址，如果有则覆盖，若没有则继续遍历表
          while(k<8)
          { if(planetTable[i].longAddr[k]!=rxData.packet[14+k])
              break;
            k++;
          }
          if(k==8)
          {
            planetTable[i].active = TRUE;
            shortAddrCounter++;
            planetTable[i].shortAddr = shortAddrCounter;
            //memcpy(planetTable[i].longAddr, longSrcAddr, 8);
            pt = PT_JOIN_ACCEPTED;
            assignedShortId[0] = (shortAddrCounter>>0)&0xFF;
            assignedShortId[1] = (shortAddrCounter>>8)&0xFF;
            printf("Join: Planet 0x%04X (index %d) has joined the network\r\n", shortAddrCounter, i);
            flag=1;
            break;
          }
        }

        if(flag==0) //如果没有找到相同长地址，则查找空缺位置加进去
        {
       
          for(int i=0;i<PLANET_TABLE_SIZE;i++)
          {            
            if(!planetTable[i].active) 
            { 
              planetTable[i].active = TRUE;
              shortAddrCounter++;
              planetTable[i].shortAddr = shortAddrCounter;
              memcpy(planetTable[i].longAddr, longSrcAddr, 8);
              pt = PT_JOIN_ACCEPTED;
              assignedShortId[0] = (shortAddrCounter>>0)&0xFF;
              assignedShortId[1] = (shortAddrCounter>>8)&0xFF;
              printf("Join: Planet 0x%04X (index %d) has joined the network\r\n", shortAddrCounter, i);
              break;
            }
          }
        }
        //payload type
        packet[22] = pt;
        packet[23] = assignedShortId[0];
        packet[24] = assignedShortId[1];
        //Unicast join request response
        enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
      }
      break;	
#endif
#ifdef PLANET_ROLE
    case (JOIN_ACCEPTED_PACKET): //executes on PLANET
      RX_DETAILS(printf("JOIN_ACCEPTED_PACKET\r\n");)
      //pan ID is already set during PT_SUN_AVAILABLE
      //set my short ID to the one assigned to me via the payload
      ST_RadioSetNodeId((rxData.packet[payloadStart+1]<<0)|(rxData.packet[payloadStart+2]<<8));
      //join complete, halt the search
      networkJoinedStopSearching = TRUE;
      break;
    case (JOIN_DENIED_PACKET): //executes on PLANET
      RX_DETAILS(printf("JOIN_DENIED_PACKET\r\n");)
      //return to broadcast PAN ID and try other suns/channels
      ST_RadioSetPanId(0xFFFF);
      break;
#endif
#ifdef SUN_ROLE
    case (POLLING_PACKET): //executes on SUN
      RX_DETAILS(printf("POLLING_PACKET\r\n");)
      //Remember: ST_RadioDataPendingShortIdIsrCallback already did
      //the hard work of determining is the polling node has data.  If
      //ackFramePendingSet is true then we definitely have data from
      //the polling device.  Send the packet now.
      if(rxData.ackFramePendingSet) {
        //When transmitting a packet in response to receiving a packet,
        //we must wait 1ms to ensure our new transmission does not disrupt
        //the ACK automatically sent in response to the reception.
        halCommonDelayMilliseconds(1);
        transmitIndirectPacketNow();
      }
      break;
      
    case (LEAVING_PACKET): //executes on SUN
      RX_DETAILS(printf("LEAVING_PACKET\r\n");)
      //Try to remove the child from the table (but don't worry about it
      //if we can't).
      for(int i=0;i<PLANET_TABLE_SIZE;i++) {
        if(planetTable[i].active && (planetTable[i].shortAddr==shortSrcAddr)) {
          printf("Leave: Planet 0x%04X (index %d) has left the network\r\n", shortSrcAddr, i);
          planetTable[i].active = FALSE;
          planetTable[i].shortAddr=0x0000;
          memcpy(planetTable[i].longAddr, 0x0000000000000000, 8);
          break;
        }
      }
      break;
#endif     
    default:
      RX_DETAILS(printf("Unknown payload type\r\n");)
      goto stopProcessing;
  }
  
stopProcessing:
  rxData.packetBeingProcessed = FALSE;
}

//This callback must be defined.  This callback is enabled using the function
//ST_RadioEnableOverflowNotification but should never be called by the phy
//library.  If it is called, something went wront with receive.  Probably
//long latency.
void ST_RadioOverflowIsrCallback(void)
{
  assert(FALSE);
}

//Command invoked with 'f'.  Form a network and become a sun.
void formCmd(void)
{
  u8 i;
  u8 searchChannel;
  s8 searchEnergy;
  u8 minEnergyChannel = ST_MIN_802_15_4_CHANNEL_NUMBER;
  s8 minEnergy = 127;
  StStatus status = ST_SUCCESS;

  printf("\r\n");
  if(activeInNetwork) {
    printf("Already in network\r\n");
    return;
  }
  printf("Inactive node forming network and becoming the sun\r\n");
  
  initNetworkState();
  
  TURN_RADIO_ON();
  
  //Loop across all channels
  for(searchChannel=ST_MIN_802_15_4_CHANNEL_NUMBER;
      searchChannel<=ST_MAX_802_15_4_CHANNEL_NUMBER;
      searchChannel++) {
    s8 maxEnergy=-128;
    status = ST_RadioSetChannel(searchChannel);
    assert(status==ST_SUCCESS);
    
    for(i=0;i<ENERGY_SAMPLES_COUNT;i++) {
      //Wait 16 symbol periods for energy readings to settle.
      halCommonDelayMicroseconds(256);
      //Find the max energy for this channel
      searchEnergy = ST_RadioEnergyDetection();
      if(searchEnergy > maxEnergy) {
        maxEnergy = searchEnergy;
      }
    }
    //Find the channel with the lowest max energy
    if(maxEnergy < minEnergy) {
      minEnergy = maxEnergy;
      minEnergyChannel = searchChannel;
    }
  }
  
  //Select the channel with the lowest max energy
  status = ST_RadioSetChannel(minEnergyChannel);
  assert(status==ST_SUCCESS);
  printf("Selected channel %d with energy %d dBm\r\n",
         minEnergyChannel, minEnergy);
  
  //Give our new network a random PAN ID
  ST_RadioSetPanId(MyPANID);
  printf("Randomly chose PAN ID: 0x%04X\r\n", ST_RadioGetPanId());
  
  //When forming a network, the sun always assigns itself Node ID 0.
  //(shortAddrCounter is initialized to 0 by initNetworkState)
  ST_RadioSetNodeId(shortAddrCounter);
  
  //Address filtering was already enabled during  app initialization
  //Planet table was already cleared out by initNetworkState()
  
  //The new network is now formed and ready
  activeInNetwork = TRUE;
}


//Command invoked with 'j'.  Join a network and become a planet.  After
//the first packet is transmitted here, the rest of the join process
//is handled by processRxPacket.
void joinCmd(void)
{
  u8 packet[128];
  u8 searchChannel;
  u32 lastTime;
  StStatus status = ST_SUCCESS;

  printf("\r\n");
  if(activeInNetwork) {
    printf("Already in network\r\n");
    return;
  }
  printf("Inactive node joining network and becoming a planet\r\n");
  
  initNetworkState();
  
  TURN_RADIO_ON();
  //While we're not joined to a network yet, the receive ISR needs to be able
  //to process packets.  This state will either stay TRUE if the join
  //succeeds or return to FALSE if it doesn't.
  activeInNetwork = TRUE;
  
  //Length
  packet[0] = (18+2);
  packet[1] = FCF_DATA;
  packet[2] = FCF_SHORTDST + FCF_LONGSRC;
  //packet[3] - New sequence number is handled immediately before transmission
  //dst PAN ID
  packet[4] = (0xFFFF>>0)&0xFF;
  packet[5] = (0xFFFF>>8)&0xFF;
  //Short dst addr
  packet[6] = (0xFFFF>>0)&0xFF;
  packet[7] = (0xFFFF>>8)&0xFF;
  //src PAN ID
  packet[8] = (0xFFFF>>0)&0xFF;
  packet[9] = (0xFFFF>>8)&0xFF;
  //Long src addr
  memcpy((packet+10), ST_RadioGetEui64(), 8);
  //payload type
  packet[18] = PT_SUN_SEARCH;
  
  printf("Trying channel");
  //Loop across all channels
  for(searchChannel=ST_MIN_802_15_4_CHANNEL_NUMBER;
      searchChannel<=ST_MAX_802_15_4_CHANNEL_NUMBER;
      searchChannel++) {
    //Since we wait 200ms per channel while trying to join, the watchdog
    //must be reset to prevent it from triggering.
    halResetWatchdog();
    
    printf(" %d", searchChannel);
    status = ST_RadioSetChannel(searchChannel);
    assert(status==ST_SUCCESS);

    //Populate the join packet with a new sequence number
    currSeqNum++;
    packet[3]=currSeqNum;
    
    //Broadcast searching for any suns
    availableSunFound = FALSE;
    enqueueTxPacket(TRUE, 0xFFFF, packet, 0);
    
    //Dwell for 200ms trying to complete a join with any sun on this channel
    lastTime = halCommonGetInt32uMillisecondTick();
    do {
      processRxPacket();
      txTick();
    } while(elapsedTimeInt32u(lastTime, halCommonGetInt32uMillisecondTick())<
            200);
    
    //If our join was accepted and we've successfully transmitted our join
    //acceptence acknowledgment, we can exit the join channel search.
    if(networkJoinedStopSearching) {
      printf("\r\n");
      printf("Joined on channel %d with PAN ID 0x%04X.  My ID is now 0x%04X.\r\n",
             ST_RadioGetChannel(),
             ST_RadioGetPanId(),
             ST_RadioGetNodeId());
      activeInNetwork = TRUE;
      //shortaddr[currnum++]=ST_RadioGetNodeId();//!!!!!!!!!!11.22 20:41
#ifdef PLANET_ROLE
      autoSendRate = 60;
      halSetLed(LED_D4);
     // shortaddr[currnum++]=ST_RadioGetNodeId();//!!!!!!!!!!11.22 20:41
#endif
      return;
    }
  }
  
  printf("\r\n");
  printf("Did not join.  Returning to inactive state.\r\n");
  //Getting here means we did not successful join a network.
  activeInNetwork = FALSE;
}
/**************************************************************************
Derek
*************************************************************************/
#ifdef SUN_ROLE  
boolean fer = TRUE;
void sendSync()
{
  u16 dstShortAddr=0xffff;//将此处的地址改为0xffff，即广播地址
  u16 vddMillivolts = 0x0000;
  boolean sendDirectly = TRUE;
  
  u8 packet[128];
  
  //Length
  packet[0] = (19+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_GENERIC_DATA;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Derek: Sync 消息的标识
 //if(fer)
 //{
   packet[13]='S';
   packet[14]=0;
   packet[15]=0;
   packet[16]=0;
   packet[17]=0;
   //fer=!fer;
 //}
 /*else
 {
   packet[13]='L';
   packet[14]=(back>>0)&0xff;
   packet[15]=(back>>8)&0xff;
   packet[16]=(back>>16)&0xff;
   packet[17]=(back>>24)&0xff;
    
   back=-1;
   fer=!fer;
 }*/
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  packet[18] = 0;
  packet[19] = 0;
  packet[20] = 0;
  /*printf("\r\n");
  printf("syn packet:\r\n");
  u8 i;
  for(i=0;i<=packet[0];i++) {
        printf("%02X ",packet[i]);
      }
  printf("\r\n");
  */
  T1=halCommonGetInt32uMillisecondTick();
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 18);
  //printf("send Sync already!\r\n");
}

#endif
#ifdef PLANET_ROLE
//reply
void replySync()
{
  u16 vddMillivolts=0xffff;
  u16 dstShortAddr = 0x0000;
  boolean sendDirectly = TRUE;
  //
  u8 not[4];
  not[0]='r';
  not[1]='p';
  not[2]='0';//LGZ：0对应planet0，1对应planet1，2对应planet2，3对应planet3
  not[3]='y';
  u8 packet[128];
  
  //Length
  packet[0] = (27+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_TRSEND;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  
  for(int i=0; i < 4; i++)
    packet[13+i]=not[i];
  
  packet[17]=(T2>>0)&0xff;
  packet[18]=(T2>>8)&0xff;
  packet[19]=(T2>>16)&0xff;
  packet[20]=(T2>>24)&0xff;
  T3=halCommonGetInt32uMillisecondTick();
  packet[21]=(T3>>0)&0xff;
  packet[22]=(T3>>8)&0xff;
  packet[23]=(T3>>16)&0xff;
  packet[24]=(T3>>24)&0xff;
  
  packet[25] = 0;
  packet[26] = 0;
  packet[27] = 0;
  //halCommonDelayMilliseconds(200);//单位ms//LGZ：0ms对应planet0，30ms对应planet1，60ms对应planet2，90ms对应planet3
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 25);
 // printf("send Reply already!\r\n");
}

#endif
/**************************************************************************
功能描述：向参数中传入的地址发送类型负载类型为PT_GENERIC_DATA，类型为GENERIC_DATA_PACKET的数据包
输入参数：vddMillivolts为发送的16位数据，dstShortAddr为目的地址，sendDirectly为调用不同发送函数的表示符。
输出参数：无
*************************************************************************/

//This function is indirectly invoked by the command 's' to send a data packet.
void sendVddDataPacket(u16 vddMillivolts,
                       u16 dstShortAddr,
                       boolean sendDirectly)
{
  u8 packet[128];
  
  //Length
  packet[0] = (15+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_GENERIC_DATA;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Derek
  
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  packet[13] = 0;
  packet[14] = 0;
  packet[15] = 0;
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 13);
 // printf("send already!\r\n");
}

/**************************************************************************
功能描述：向参数中传入的地址发送类型负载类型为PT_SLEEPING，类型为SYN_SLEEPING_WAITTIME的数据包
输入参数：vddMillivolts为发送的16位数据，dstShortAddr为目的地址，sendDirectly为调用不同发送函数的表示符。
输出参数：无
*************************************************************************/
void sendVddDataPacket1(u16 vddMillivolts,u16 dstShortAddr,boolean sendDirectly)
{
  u8 packet[128];
  
  //Length
  packet[0] = (15+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_SLEEPING;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  packet[13] = 0;
  packet[14] = 0;
  packet[15] = 0;
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 13);
#ifdef SUN_ROLE  
  halToggleLed(LED_D2);
  halCommonDelayMilliseconds(500);
  halClearLed(LED_D2);
#endif
  printf("send sleeping packet to every planet!\r\n");
}

/**************************************************************************
功能描述：向参数中传入的地址发送类型负载类型为PT_LED，类型为SYN_LED_WAITTIME的数据包
输入参数：vddMillivolts为发送的16位数据，dstShortAddr为目的地址，sendDirectly为调用不同发送函数的表示符。
输出参数：无
*************************************************************************/
void sendVddDataPacket2(u16 vddMillivolts,u16 dstShortAddr,boolean sendDirectly)
{
  u8 packet[128];
  
  //Length
  packet[0] = (15+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_LED;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  packet[13] = 0;
  packet[14] = 0;
  packet[15] = 0;
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 13);
#ifdef SUN_ROLE
  halToggleLed(LED_D1);
  halCommonDelayMilliseconds(500);
  halClearLed(LED_D1);
#endif
  printf("send message to every planet!\r\n");
}

//函数实现向dstShortAddr表示的节点发送数据，数据内容为vddMillivolts
void sendVddDataPacket3(u16 vddMillivolts,u16 dstShortAddr,boolean sendDirectly)
{
  u8 packet[128];
  
  //Length
  packet[0] = (15+2);
  packet[1] = FCF_DATA + FCF_ACKREQ + FCF_INTRAPAN;
  packet[2] = FCF_SHORTDST + FCF_SHORTSRC;
  //New sequence number
  currSeqNum++;
  packet[3] = currSeqNum;
  //dst PAN ID
  packet[4] = (ST_RadioGetPanId()>>0)&0xFF;
  packet[5] = (ST_RadioGetPanId()>>8)&0xFF;
  //Short dst addr
  packet[6] = (dstShortAddr>>0)&0xFF;
  packet[7] = (dstShortAddr>>8)&0xFF;
  //Short src addr
  packet[8] = (ST_RadioGetNodeId()>>0)&0xFF;
  packet[9] = (ST_RadioGetNodeId()>>8)&0xFF;
  //payload type
  packet[10] = PT_TRSEND;
  //my VDD_PADS
  packet[11] = (vddMillivolts>>0)&0xFF;
  packet[12] = (vddMillivolts>>8)&0xFF;
  //Zero out the TX SFD payload.  The MSB is used to indicate SFD valid.
  packet[13] = 0;
  packet[14] = 0;
  packet[15] = 0;
  
  enqueueTxPacket(sendDirectly, dstShortAddr, packet, 13);
  #ifdef PLANET_ROLE
  halToggleLed(LED_D3);
  halCommonDelayMilliseconds(500);
  halToggleLed(LED_D3);
  halCommonDelayMilliseconds(500);
#endif
  //printf("trsend already!\r\n");
}

//Command invoked with 'p'.  Print the planet table.
void planetTableCmd(void)
{
  u8 i,k;
  
  printf("\r\n");
  if(!activeInNetwork) {
    printf("Not active in a network\r\n");
    return;
  }
  if(ST_RadioGetNodeId() != 0x0000) {
    printf("Not a sun\r\n");
    return;
  }
  
  printf("Planet Table\r\n");
  printf("Active | DataPending | Short Address |    Long Address\r\n");
  for(i=0;i<PLANET_TABLE_SIZE;i++) {

    printf("   %d   | ", planetTable[i].active);
    printf("      %d     | ", isDataPendingForShortId(planetTable[i].shortAddr));
    printf("    0x%04X    | 0x", planetTable[i].shortAddr);
    k=8;
    while(k--) {
      printf("%02X", planetTable[i].longAddr[k]);
    }
    printf("\r\n");
  }
}

//******************************************************************************
// Application main:  Entry point, initialization, command line implementation,
// and event control.

/**************************************************************************
功能描述：实现SUN节点组网，PLANET节点加网。多节点通信。
输入参数：无
输出参数：无
*************************************************************************/
int main(void)
{
    u32 seed;
    StStatus status = ST_SUCCESS;
    
    //Initialize the HAL.
    halInit();
  
    //Initialize the UART.  uartInit will configure PB1=OUT_ALT and PB2=IN.
    uartInit(115200, 8, PARITY_NONE, 1);
  
    //Configure PA4 and PA5 as alternate outputs for Packet Trace.
    halGpioConfig(PORTA_PIN(4),GPIOCFG_OUT_ALT);
    halGpioConfig(PORTA_PIN(5),GPIOCFG_OUT_ALT);
    //No other GPIO initialization is performed for this application.
  
    //Connect IRQD to PB2/SC1RXD
    GPIO_IRQDSEL = PORTB_PIN(2);
    //Allow the IRQD flag to set on any IRQD activity.  This does not
    //enable the top level interrupt since no interrupt is desired; just
    //an activity flag is needed to know when to avoid deep sleep.
    GPIO_INTCFGD = (3<<GPIO_INTMOD_BIT);
    INT_GPIOFLAG = INT_IRQDFLAG;
    INT_PENDCLR = INT_IRQD;
    INTERRUPTS_ON();
    
    //LGZ：
    //int32u time = 0;
   // settime(time);
    u32 getTime;
    getTime=halCommonGetInt32uMillisecondTick();

   u16 count = 0;
    
    
#ifdef PLANET_ROLE
    halInitLed();   //初始化LED灯
    halInitButton(); //初始化按键
    printf("\r\nSimpleMAC (%s) Sample Application: 'Planet role'!\r\n",SIMPLEMAC_VERSION_STRING);
#endif
  

#ifdef SUN_ROLE
    halInitLed();   //初始化LED灯
    halInitButton(); //初始化按键
    printf("\r\nSimpleMAC (%s) Sample Application: 'Sun role'!\r\n",SIMPLEMAC_VERSION_STRING);
#endif
  
    //Initialize random number generation.
    ST_RadioGetRandomNumbers((u16 *)&seed, 2);
    halCommonSeedRandom(seed);
  
    //Initialize the radio.
    ST_RadioEnableOverflowNotification(TRUE);
    status = ST_RadioInit(radioPowerState);
    assert(status==ST_SUCCESS);  
    printf("Enter ? for list of commands\r\n");
    printf("\r\n> ");
    
#ifdef SUN_ROLE
    u8 ch;
    formCmd(); //SUN节点首先创建网络
    u32 icou = 0;
    do
        {
            processRxPacket();//检测收到数据包并处理
           
            halCommonDelayMilliseconds(10);//延时10毫秒
            icou++;
     }while ((!__io_getcharNonBlocking(&ch))&&icou<500); 
    settime(0);
    while(TRUE)
    { 
      /*  
      do
        {
            processRxPacket();//检测收到数据包并处理
           
            halCommonDelayMilliseconds(10);//延时10毫秒
        }while ((!__io_getcharNonBlocking(&ch))); //获取串口发送的命令
        
        
        if(ch=='x')//Derek:同步开始命令'x'
        {
           // printf("x command is running!\n");
            sendSync();
        }
         if(ch=='t')//Derek:回传T2
        {
          //  printf("T2 back!\n");
            sendSync();
        }
        else
            //printf("Unknown Commamd\r\n");
        INT_GPIOFLAG = INT_IRQDFLAG;
        INT_PENDCLR = INT_IRQD;
      */
      
    getTime=halCommonGetInt32uMillisecondTick();
    u16 ww1,ww2,ww3,ww4,ww5,ww6,ww7,ww8;
    double mm;
    
    if(getTime%5000==0)
    {
      sendSync();
      count++;
      printf("A%d\r\n",count);
    /*
    u16 ww1,ww2,ww3,ww4,ww5,ww6,ww7,ww8;
    double mm;
    mm=getTime;
    ww1=mm/10000000;
    ww2=(mm-ww1*10000000)/1000000;
    ww3=((mm-ww1*10000000)-ww2*1000000)/100000;
    ww4=(((mm-ww1*10000000)-ww2*1000000)-ww3*100000)/10000;
    ww5=((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)/1000;
    ww6=(((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)/100;
    ww7=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)/10;
    ww8=((((((mm-ww1*10000000)-ww2*1000000)-ww3*100000)-ww4*10000)-ww5*1000)-ww6*100)-ww7*10;
    printf("%d",ww1); 
    printf("%d",ww2); 
    printf("%d",ww3);
    printf("%d",ww4); 
    printf("%d",ww5);
    printf("%d",ww6); 
    printf("%d",ww7);
    printf("%d\r\n",ww8); 
    ww1=0;ww2=0;ww3=0;ww4=0;ww5=0;ww6=0;ww7=0;ww8=0;
    
     printf("get LGZ");*/ 
      halCommonDelayMilliseconds(50);
      processRxPacket();//检测收到数据包并处理     
      //halCommonDelayMilliseconds(10);//延时10毫秒  
          
    }
     INT_GPIOFLAG = INT_IRQDFLAG;
     INT_PENDCLR = INT_IRQD;
    }
    
#endif

#ifdef PLANET_ROLE
    activeInNetwork = FALSE;
    printf("pl has start\r\n");
    do
    {
        joinCmd(); //请求加入网络
    }while(!activeInNetwork);
    u16 tiger=0;
    settime(0);
    while(TRUE)
    {
      //LGZ:
        getTime=halCommonGetInt32uMillisecondTick();
        if(getTime/5000>tiger)
        {
         
         printf("B%d\r\n",count);
         count++;
         
        } 
        tiger=getTime/5000;
        processRxPacket();
        halCommonDelayMilliseconds(10);
        /*if(halGetButtonStatus(BUTTON_S3) == BUTTON_PRESSED) //如果S3被按下，向Sun节点发送信息
        {
            sendVddDataPacket3(0x0001, 0x0000, TRUE);
            halCommonDelayMilliseconds(400); //延迟400ms
        }*/        
        INT_GPIOFLAG = INT_IRQDFLAG;
        INT_PENDCLR = INT_IRQD;
    } 
     
#endif
}


//******************************************************************************
//The MAC Timer compare functionality is not used in this app.
void ST_RadioMacTimerCompareIsrCallback(void) {}

