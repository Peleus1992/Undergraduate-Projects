/** @file /hal/micro/button.h
 * @brief Header for button driver
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#ifdef PLANET_ROLE
/** Description buttons definition */
#define BUTTON_S3             PORTA_PIN(6)/////////////////////////PA6-->S2
#define BUTTON_S3_INPUT_GPIO  GPIO_PAIN
#define BUTTON_S3_OUTPUT_GPIO GPIO_PAOUT
#define BUTTON_S3_GPIO_PIN    PA6_BIT
#define BUTTON_S3_WAKE_SOURCE 0x00000040

#elif SUN_ROLE
#define BUTTON_S2             PORTC_PIN(0)/////////////////////////PC0-->S2
#define BUTTON_S2_INPUT_GPIO  GPIO_PCIN
#define BUTTON_S2_OUTPUT_GPIO GPIO_PCOUT
#define BUTTON_S2_GPIO_PIN    PC0_BIT

#define BUTTON_S3             PORTC_PIN(3)/////////////////////////PC3-->S3
#define BUTTON_S3_INPUT_GPIO  GPIO_PCIN
#define BUTTON_S3_OUTPUT_GPIO GPIO_PCOUT
#define BUTTON_S3_GPIO_PIN    PC3_BIT

#define BUTTON_S4             PORTB_PIN(6)/////////////////////////PB6-->S4
#define BUTTON_S4_INPUT_GPIO  GPIO_PBIN
#define BUTTON_S4_OUTPUT_GPIO GPIO_PBOUT
#define BUTTON_S4_GPIO_PIN    PB6_BIT
#define BUTTON_S4_WAKE_SOURCE 0x00000080

#define BUTTON_S5             PORTB_PIN(0)/////////////////////////PB0-->S5
#define BUTTON_S5_INPUT_GPIO  GPIO_PBIN
#define BUTTON_S5_OUTPUT_GPIO GPIO_PBOUT
#define BUTTON_S5_GPIO_PIN    PB0_BIT
#define BUTTON_S5_WAKE_SOURCE 0x00000080
#endif

/* button status */
#define BUTTON_PRESSED   0
#define BUTTON_RELEASED  1
#define BUTTON_UNKNOWN   3


typedef int8u HalBoardButton;

/* Functions -----------------------------------------------------------------*/

/** @brief Init buttons */
void halInitButton(void);

/** @brief Get button status */
int8u halGetButtonStatus(HalBoardButton button);

#endif /* _BUTTON_H_ */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
