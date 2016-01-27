/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
/*
 *  ======== main.c ========
 */
#include <string.h>
#include <time.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Seconds.h>

#include <ti/drivers/GPIO.h>

#include <simplesample_http.h>

#include "Board.h"
#include "UARTUtils.h"

#include <simplelink.h>
#include <ti/net/http/sssl.h>

/* USER STEP: update to current date-time */
#define DAY     6
#define MONTH   1
#define YEAR    2016
#define HOUR    19
#define MINUTE  0
#define SECOND  0

/* USER STEP: Flash the CA root certificate using UniFlash tool */
#define SL_SSL_CA_CERT "/cert/ms.der"

/* TI time apis return seconds since 1900 */
#ifdef __ti__
/* Years (70 * 365 * 24 * 60 * 60) +  Leap Years (17 * 1 * 24 * 60 * 60) */
#define TIMEDIFF   (2208988800)
#else
#define TIMEDIFF   (0)
#endif

#define YEARDIFF  1900
#define MONTHDIFF 1

/* The following params should be persistant for the lifetime of the app */
SlSockSecureMethod method;
SlSockSecureMask mask;
char cafile[32] = {0};

extern void NetWiFi_init();

/*
 *  ======== setTime ========
 */
static void setTime(void)
{
    SlDateTime_t dt;
    struct tm startTime = {0};
    time_t ts;

    /* Set system clock on network processor to validate certificate */
    dt.sl_tm_day  = DAY;
    dt.sl_tm_mon  = MONTH;
    dt.sl_tm_year = YEAR;
    dt.sl_tm_hour = HOUR;
    dt.sl_tm_min  = MINUTE;
    dt.sl_tm_sec  = SECOND;
    sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
            SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
            sizeof(SlDateTime_t), (unsigned char *)(&dt));

    /* Set system clock on application microcontroller */
    startTime.tm_mday = DAY;
    startTime.tm_mon  = MONTH - MONTHDIFF;
    startTime.tm_year = YEAR - YEARDIFF;
    startTime.tm_hour = HOUR;
    startTime.tm_min  = MINUTE;
    startTime.tm_sec  = SECOND;
    ts = mktime(&startTime);
    Seconds_set(ts - (unsigned int) TIMEDIFF);
}

/*
 *  ======== slTask ========
 */
void slTask(unsigned int arg0, unsigned int arg1)
{
    Task_Handle taskHandle;
    Task_Params taskParams;

    /*
     *  Add the UART device to the system.
     *  All UART peripherals must be setup and the module must be initialized
     *  before opening.  This is done by Board_initUART().  The functions used
     *  are implemented in UARTUtils.c.
     */
    add_device("UART", _MSA, UARTUtils_deviceopen,
               UARTUtils_deviceclose, UARTUtils_deviceread,
               UARTUtils_devicewrite, UARTUtils_devicelseek,
               UARTUtils_deviceunlink, UARTUtils_devicerename);

    /* Open UART0 for writing to stdout and set buffer */
    freopen("UART:0", "w", stdout);
    setvbuf(stdout, NULL, _IOLBF, 128);

    /* Open UART0 for writing to stderr and set buffer */
    freopen("UART:0", "w", stderr);
    setvbuf(stderr, NULL, _IOLBF, 128);

    /* Open UART0 for reading from stdin and set buffer */
    freopen("UART:0", "r", stdin);
    setvbuf(stdin, NULL, _IOLBF, 128);

    /*
     *  Initialize UART port 0 used by SysCallback.  This and other SysCallback
     *  UART functions are implemented in UARTUtils.c. Calls to System_printf
     *  will go to UART0, the same as printf.
     */
    UARTUtils_systemInit(0);

    System_printf("Starting the simplesample_http example\n");

    /* Initialize SimpleLink */
    NetWiFi_init();

    setTime();

    /* Secure params */
    method.secureMethod = SL_SO_SEC_METHOD_TLSV1_1;
    mask.secureMask  = SL_SEC_MASK_TLS_RSA_WITH_AES_256_CBC_SHA;

    if (sizeof(cafile) < sizeof(SL_SSL_CA_CERT)) {
        System_abort("slTask: Certificate name too long!\n");
    }

    strncpy(cafile, SL_SSL_CA_CERT, sizeof(SL_SSL_CA_CERT));
    /* End Secure Params */
    SSSl_setContext(&method, &mask, cafile, NULL, NULL, NULL);

    Task_Params_init(&taskParams);
    taskParams.stackSize = 4096;

    taskHandle = Task_create((Task_FuncPtr)simplesample_http_run, &taskParams,
             NULL);
    if (taskHandle == NULL) {
        System_abort("slTask: failed to create a Task!\n");
    }
}

/*
 *  ======== main ========
 */
int main(int argc, char *argv[])
{
    Task_Handle taskHandle;
    Task_Params taskParams;

    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    GPIO_write(Board_LED0, Board_LED_ON);

    /* Initialize the Spawn Task Mailbox */
    VStartSimpleLinkSpawnTask(3);

    Task_Params_init(&taskParams);
    taskParams.stackSize = 1024;

    taskHandle = Task_create((Task_FuncPtr)slTask, &taskParams, NULL);
    if (taskHandle == NULL) {
        System_abort("main: failed to create a Task!\n");
    }

    BIOS_start();
}
