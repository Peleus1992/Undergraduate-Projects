/** @file hal/micro/led.h
 *  @brief Header for led APIs
 *  
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

/** @addtogroup led 
 *  @brief  Sample API funtions for controlling LEDs.
 *
 * When specifying an LED to use, always use the BOARDLEDx definitions that
 * are defined within the BOARD_HEADER.
 * 
 * See led.h for source code.
 *@{
 */
#ifndef __LED_H__
#define __LED_H__

/* leds definitions */
#ifdef PLANET_ROLE
#define LED_D4 PORTB_PIN(5) //LED4->PB5
#define LED_D3 PORTB_PIN(6) //LED3-PB6

#elif SUN_ROLE
#define LED_D1 PORTA_PIN(3)//LED1->PA3
#define LED_D2 PORTC_PIN(2)//LED2->PC2
#endif


 
/** @brief Configures GPIOs pertaining to the control of LEDs.
 */
void halInitLed(void);

/** @brief Ensures that the definitions from the BOARD_HEADER
 *  are always used as parameters to the LED functions.
 */
  typedef int8u HalBoardLed;
// Note: Even though many compilers will use 16 bits for an enum instead of 8, 
//  we choose to use an enum here.  The possible compiler inefficiency does not 
//  affect stack-based parameters and local variables, which is the
//  general case for led paramters.

/** @brief Atomically wraps an XOR or similar operation for a single GPIO
 *  pin attached to an LED.
 * 
 *  @param led  Identifier (from BOARD_HEADER) for the LED to be toggled.
 */
void halToggleLed(HalBoardLed led);

/** @brief Turns on (sets) a GPIO pin connected to an LED so that the LED 
 *  turns on.
 * 
 *  @param led  Identifier (from BOARD_HEADER) for the LED to turn on.
 */
void halSetLed(HalBoardLed led);

/** @brief Turns off (clears) a GPIO pin connected to an LED, which turns 
 *  off the LED.
 *  
 *  @param led  Identifier (from BOARD_HEADER) for the LED to turn off.
 */
void halClearLed(HalBoardLed led);

/** @} // END addtogroup 
 */
#endif

