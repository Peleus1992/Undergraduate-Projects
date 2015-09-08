/** @file bl_user.c
 * @brief User defined OTA bootloader functions x MB851 board
 *        This must be a very low level code simple, reliable and small.
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#include "regs.h"

typedef unsigned char  int8u;
typedef unsigned int   int32u;

int8u bootloaderOTAHardwareActivation(void)
{
  /* MB851 button S1 is on GPIO A7 */
  volatile int i;
  /* PA7 in input pull up-down: 
     register address: PACFGH = 0xB004, 
     PA7 in input,pulled up or down: 0x8,
     Others (PA6,5,4) in input floating: 0x4.
  */
  GPIO_PACFGH = 0x8444;
  
  /* PA7 pull-up value to 1,
     register address: PAOUT = B00C
  */
  GPIO_PAOUT |= (1 <<7);
  
  /* added delay before reading the button status */
  for (i = 0; i < 1000; i++);
  
  /* Return button status (PA7):
     register address: PAIN = B008, 
     get bit 7
  */
  return (GPIO_PAIN & (1<<7)) ? 0:1;
}

void bootloaderOTAIndicateActivation(void)
{
  /* MB851 LED D3 is  on GPIO PB5 */
  
  /* PB5 in output: 
     register address: PBCFGH = 0xB404, 
     PB5 in output, push pull: 0x1,
     Others (PB7,6,4) in input floating: 0x4
  */
  GPIO_PBCFGH = 0x4414;
  //*((volatile int32u *)0x4000B404) |= (1 << 5);
  
  /* turn on led D3 (PB5)*/
  GPIO_PBCFGH &= ~(1 << 5);
  
}
