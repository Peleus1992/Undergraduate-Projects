/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : timer.c
* Author             : MCD Application Team
* Version            : V0.0.1
* Date               : 05/05/2009
* Description        : This file provides the timer support for RF4CE
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#ifdef PCWIN32
#include <sys/time.h> 
static struct timeval init;
static struct timezone tz;
#else
#include PLATFORM_HEADER
#include "hal/hal.h"
#include "hal/error.h"
#include "phy-library.h"
#include "timer.h"
static u32 init;
#endif

static u32 timeCur=0;

#if 0
static u32 TIME_Current(struct timeval *current)
{
  struct timeval diff;

  diff.tv_sec = current->tv_sec - init.tv_sec;
  diff.tv_usec = current->tv_usec - init.tv_usec;
  
  if (diff.tv_usec < 0) {
    diff.tv_usec += 1000000;
    diff.tv_sec--;
  }
  return (diff.tv_usec + (diff.tv_sec * 1000000));
}
#endif

/* This timer can use SysTick */
u32 TIME_CurrentTime()
{
  TIMER_Tick();
  return timeCur;
}

u32 TIME_ELAPSED(u32 startTime)
{
  return TIME_CurrentTime() - startTime;
}

void TIMER_Init()
{
#ifdef PCWIN32
  gettimeofday(&init, &tz);
#else
  init = ST_RadioGetMacTimer();
#endif
  return;
}

void TIMER_Tick(void) {
#ifndef PCWIN32
  u32 diff, newTime;

  newTime = ST_RadioGetMacTimer();
  if (init<newTime)
    diff = newTime - init;
  else
    diff = newTime + ((1 << 20) - init); //1048576 = 2^20 usec ~1 sec

  timeCur += diff;
#else
  struct timeval diff, newTime;
  gettimeofday(&newTime, &tz);

  diff.tv_sec = newTime.tv_sec - init.tv_sec;
  diff.tv_usec = newTime.tv_usec - init.tv_usec;
  
  if (diff.tv_usec < 0) {
    diff.tv_usec += 1000000;
    diff.tv_sec--;
  }
  timeCur += diff.tv_usec + (diff.tv_sec * 1000000);
#endif
  init = newTime;
  return;

}
