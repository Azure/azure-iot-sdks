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
/** ============================================================================
 *  @file       C3200_LP.h
 *
 *  @brief      CC3200 Board Specific APIs
 *
 *  The CC3200_LAUNCHXL header file should be included in an application as
 *  follows:
 *  @code
 *  #include <CC3200_LAUNCHXL.h>
 *  @endcode
 *
 *  ============================================================================
 */
#ifndef __CC3200_LAUNCHXL_H
#define __CC3200_LAUNCHXL_H

#ifdef __cplusplus
extern "C" {
#endif

#define CC3200_LAUNCHXL_LED_OFF (0)
#define CC3200_LAUNCHXL_LED_ON  (1)

/*!
 *  @def    CC3200_LAUNCHXL_GPIOName
 *  @brief  Enum of GPIO names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_GPIOName {
    CC3200_LAUNCHXL_SW2 = 0,
    CC3200_LAUNCHXL_SW3,
    CC3200_LAUNCHXL_LED_D7,

    /*
     *  CC3200_LAUNCHXL_LED_D5 and CC3200_LAUNCHXL_LED_D6 are shared with the
     *  I2C and PWM peripherals. In order for those examples to work, these
     *  LEDs are  taken out of gpioPinCOnfig[]
     */
    //CC3200_LAUNCHXL_LED_D6,
    //CC3200_LAUNCHXL_LED_D5,

    CC3200_LAUNCHXL_GPIOCOUNT
} CC3200_LAUNCHXL_GPIOName;

/*!
 *  @def    CC3200_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_I2CName {
    CC3200_LAUNCHXL_I2C0 = 0,

    CC3200_LAUNCHXL_I2CCOUNT
} CC3200_LAUNCHXL_I2CName;

/*!
 *  @def    CC3200_LAUNCHXL_I2SName
 *  @brief  Enum of I2S names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_I2SName {
    CC3200_LAUNCHXL_I2S0 = 0,

    CC3200_LAUNCHXL_I2SCOUNT
} CC3200_LAUNCHXL_I2SName;

/*!
 *  @def    CC3200_LAUNCHXL_PWMName
 *  @brief  Enum of PWM names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_PWMName {
    CC3200_LAUNCHXL_PWM6 = 0,
    CC3200_LAUNCHXL_PWM7,

    CC3200_LAUNCHXL_PWMCOUNT
} CC3200_LAUNCHXL_PWMName;

/*!
 *  @def    CC3200_LAUNCHXL_SDSPIName
 *  @brief  Enum of SDSPI names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_SDSPIName {
    CC3200_LAUNCHXL_SDSPI0 = 0,

    CC3200_LAUNCHXL_SDSPICOUNT
} CC3200_LAUNCHXL_SDSPIName;

/*!
 *  @def    CC3200_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_SPIName {
    CC3200_LAUNCHXL_SPI0 = 0,

    CC3200_LAUNCHXL_SPICOUNT
} CC3200_LAUNCHXL_SPIName;

/*!
 *  @def    CC3200_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_UARTName {
    CC3200_LAUNCHXL_UART0 = 0,
    CC3200_LAUNCHXL_UART1,

    CC3200_LAUNCHXL_UARTCOUNT
} CC3200_LAUNCHXL_UARTName;

/*!
 *  @def    CC3200_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs on the CC3200_LAUNCHXL dev board
 */
typedef enum CC3200_LAUNCHXL_WatchdogName {
    CC3200_LAUNCHXL_WATCHDOG0 = 0,

    CC3200_LAUNCHXL_WATCHDOGCOUNT
} CC3200_LAUNCHXL_WatchdogName;

/*!
 *  @brief  Initialize board specific DMA settings
 *
 *  This function creates a hwi in case the DMA controller creates an error
 *  interrupt, enables the DMA and supplies it with a uDMA control table.
 */
extern void CC3200_LAUNCHXL_initDMA(void);

/*!
 *  @brief  Initialize the general board specific settings
 *
 *  This function initializes the general board specific settings.
 */
extern void CC3200_LAUNCHXL_initGeneral(void);

/*!
 *  @brief  Initialize board specific GPIO settings
 *
 *  This function initializes the board specific GPIO settings and
 *  then calls the GPIO_init API to initialize the GPIO module.
 *
 *  The GPIOs controlled by the GPIO module are determined by the GPIO_PinConfig
 *  variable.
 */
extern void CC3200_LAUNCHXL_initGPIO(void);

/*!
 *  @brief  Initialize board specific I2C settings
 *
 *  This function initializes the board specific I2C settings and then calls
 *  the I2C_init API to initialize the I2C module.
 *
 *  The I2C peripherals controlled by the I2C module are determined by the
 *  I2C_config variable.
 */
extern void CC3200_LAUNCHXL_initI2C(void);

/*!
 *  @brief  Initialize board specific I2S settings
 *
 *  This function initializes the board specific I2S settings and then calls
 *  the I2S_init API to initialize the I2S module.
 *
 *  The I2S peripherals controlled by the I2S module are determined by the
 *  I2S_config variable.
 */
extern void CC3200_LAUNCHXL_initI2S(void);

/*!
 *  @brief  Initialize board specific PWM settings
 *
 *  This function initializes the board specific PWM settings and then calls
 *  the PWM_init API to initialize the PWM module.
 *
 *  The PWM peripherals controlled by the PWM module are determined by the
 *  PWM_config variable.
 */
extern void CC3200_LAUNCHXL_initPWM(void);

/*!
 *  @brief  Initialize board specific SDSPI settings
 *
 *  This function initializes the board specific SDSPI settings and then calls
 *  the SDSPI_init API to initialize the SDSPI module.
 *
 *  The SDSPI peripherals controlled by the SDSPI module are determined by the
 *  SDSPI_config variable.
 */
extern void CC3200_LAUNCHXL_initSDSPI(void);

/*!
 *  @brief  Initialize board specific SPI settings
 *
 *  This function initializes the board specific SPI settings and then calls
 *  the SPI_init API to initialize the SPI module.
 *
 *  The SPI peripherals controlled by the SPI module are determined by the
 *  SPI_config variable.
 */
extern void CC3200_LAUNCHXL_initSPI(void);

/*!
 *  @brief  Initialize board specific UART settings
 *
 *  This function initializes the board specific UART settings and then calls
 *  the UART_init API to initialize the UART module.
 *
 *  The UART peripherals controlled by the UART module are determined by the
 *  UART_config variable.
 */
extern void CC3200_LAUNCHXL_initUART(void);

/*!
 *  @brief  Initialize board specific Watchdog settings
 *
 *  This function initializes the board specific Watchdog settings and then
 *  calls the Watchdog_init API to initialize the Watchdog module.
 *
 *  The Watchdog peripherals controlled by the Watchdog module are determined
 *  by the Watchdog_config variable.
 */
extern void CC3200_LAUNCHXL_initWatchdog(void);

#ifdef __cplusplus
}
#endif

#endif /* __CC3200_LAUNCHXL_H */
