/** @file /hal/micro/cortexm3/button.c
 * @brief button APIs
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#include PLATFORM_HEADER
#include BOARD_HEADER
#include "hal/micro/button.h"
#include "hal/micro/micro-common.h"
#include "hal/micro/cortexm3/micro-common.h"


void halInitButton(void)
{
#ifdef PLANET_ROLE
   /* Set GPIO pin to PUD (input pull-up or pull-down) for button S1 */

  halGpioConfig(BUTTON_S3,GPIOCFG_IN_PUD);

  //halGpioConfig(BUTTON_S8,GPIOCFG_IN_PUD);
 /* Set the button S1 gpio pin to pull-up */
  BUTTON_S3_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S3_GPIO_PIN;
#elif SUN_ROLE
  halGpioConfig(BUTTON_S2,GPIOCFG_IN_PUD);
  halGpioConfig(BUTTON_S3,GPIOCFG_IN_PUD);
  halGpioConfig(BUTTON_S4,GPIOCFG_IN_PUD);
  halGpioConfig(BUTTON_S5,GPIOCFG_IN_PUD);
  //halGpioConfig(BUTTON_S8,GPIOCFG_IN_PUD);
 /* Set the button S1 gpio pin to pull-up */
  BUTTON_S2_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S2_GPIO_PIN;
  BUTTON_S3_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S3_GPIO_PIN;
  BUTTON_S4_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S4_GPIO_PIN;
  BUTTON_S5_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S5_GPIO_PIN;
#endif
 // BUTTON_S8_OUTPUT_GPIO |= GPIOOUT_PULLUP << BUTTON_S8_GPIO_PIN;
}/* end halInitButton() */


int8u halGetButtonStatus(HalBoardButton button)
{
#ifdef PLANET_ROLE
  if (button == BUTTON_S3)
    return (BUTTON_S3_INPUT_GPIO & (1<<BUTTON_S3_GPIO_PIN)) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else 
    return BUTTON_UNKNOWN;
#elif SUN_ROLE
  if (button == BUTTON_S2)
    return (BUTTON_S2_INPUT_GPIO & (1<<BUTTON_S2_GPIO_PIN)) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else if (button == BUTTON_S3)
    return (BUTTON_S3_INPUT_GPIO & (1<<BUTTON_S3_GPIO_PIN)) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else if (button == BUTTON_S4)
    return (BUTTON_S4_INPUT_GPIO & (1<<BUTTON_S4_GPIO_PIN)) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else if (button == BUTTON_S5)
    return (BUTTON_S5_INPUT_GPIO & (1<<BUTTON_S5_GPIO_PIN)) ? BUTTON_RELEASED : BUTTON_PRESSED;
  else 
    return BUTTON_UNKNOWN;
#endif
}/* end halGetButtonStatus()*/


