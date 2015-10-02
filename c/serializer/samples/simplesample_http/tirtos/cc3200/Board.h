/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "CC3200_LAUNCHXL.h"

#define Board_initDMA                CC3200_LAUNCHXL_initDMA
#define Board_initGeneral            CC3200_LAUNCHXL_initGeneral
#define Board_initGPIO               CC3200_LAUNCHXL_initGPIO
#define Board_initI2C                CC3200_LAUNCHXL_initI2C
#define Board_initI2S                CC3200_LAUNCHXL_initI2S
#define Board_initPWM                CC3200_LAUNCHXL_initPWM
#define Board_initSDSPI              CC3200_LAUNCHXL_initSDSPI
#define Board_initSPI                CC3200_LAUNCHXL_initSPI
#define Board_initUART               CC3200_LAUNCHXL_initUART
#define Board_initWatchdog           CC3200_LAUNCHXL_initWatchdog

#define Board_LED_ON                 CC3200_LAUNCHXL_LED_ON
#define Board_LED_OFF                CC3200_LAUNCHXL_LED_OFF
#define Board_LED0                   CC3200_LAUNCHXL_LED_D7
 /*
  *  CC3200_LAUNCHXL_LED_D5 and CC3200_LAUNCHXL_LED_D6 are shared with the I2C
  *  and PWM peripherals. In order for those examples to work, these LEDs are
  *  taken out of gpioPinCOnfig[]
  */
#define Board_LED1                   CC3200_LAUNCHXL_LED_D7
#define Board_LED2                   CC3200_LAUNCHXL_LED_D7
#define Board_BUTTON0                CC3200_LAUNCHXL_SW2
#define Board_BUTTON1                CC3200_LAUNCHXL_SW3

#define Board_I2C0                   CC3200_LAUNCHXL_I2C0
#define Board_I2C_TMP                CC3200_LAUNCHXL_I2C0

#define Board_I2S0                   CC3200_LAUNCHXL_I2S0

#define Board_PWM0                   CC3200_LAUNCHXL_PWM6
#define Board_PWM1                   CC3200_LAUNCHXL_PWM7

#define Board_SDSPI0                 CC3200_LAUNCHXL_SDSPI0

#define Board_SPI0                   CC3200_LAUNCHXL_SPI0

#define Board_UART0                  CC3200_LAUNCHXL_UART0
#define Board_UART1                  CC3200_LAUNCHXL_UART1

#define Board_WATCHDOG0              CC3200_LAUNCHXL_WATCHDOG0

/* Board specific I2C addresses */
#define Board_TMP006_ADDR            (0x41)

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
