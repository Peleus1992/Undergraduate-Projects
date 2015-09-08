/** @file board.h
 * @brief Header file x STM32W108 Kits boards 
 *
 * <!--(C) COPYRIGHT 2010 STMicroelectronics. All rights reserved.        -->
 */
#ifndef _BOARD_H_
#define _BOARD_H_

#ifdef BOARD_MB851

/** Description uart definition */
#define UART_TX PORTB_PIN(1)
#define UART_RX PORTB_PIN(2)
#define UART_RX_WAKE_SOURCE   0x00000400

/** Description temperature sensor GPIO */
#define TEMPERATURE_SENSOR_GPIO  PORTB_PIN(7)
#endif /* BOARD_MB851 */

#endif /* _BOARD_H_ */

