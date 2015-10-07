/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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

#include <stdbool.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>

#include "Board.h"
#include "wificonfig.h"

#include <simplelink.h>
#include <osi.h>

static uint32_t deviceConnected = false;
static uint32_t ipAcquired = false;
static uint32_t currButton, prevButton;

/*
 *  ======== SimpleLinkWlanEventHandler ========
 *  SimpleLink Host Driver callback for handling WLAN connection or
 *  disconnection events.
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_WLAN_CONNECT_EVENT:
            deviceConnected = true;
            break;

        case SL_WLAN_DISCONNECT_EVENT:
            deviceConnected = false;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkNetAppEventHandler ========
 *  SimpleLink Host Driver callback for asynchoronous IP address events.
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
            ipAcquired = true;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkSockEventHandler ========
 */
void SimpleLinkSockEventHandler(SlSockEvent_t *pArgs)
{

}

/*
 *  ======== SimpleLinkHttpServerCallback ========
 *  SimpleLink Host Driver callback for HTTP server events.
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpServerEvent,
        SlHttpServerResponse_t *pHttpServerResponse)
{
}

/*
 *  ======== smartConfigFxn ========
 */
void smartConfigFxn()
{
  uint8_t policyVal;

  /* Set auto connect policy */
  sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1,0,0, 0, 0),
            &policyVal, sizeof(policyVal));

  /* Start SmartConfig using unsecured method. */
  sl_WlanSmartConfigStart(0, SMART_CONFIG_CIPHER_NONE, 0, 0, 0,
            NULL, NULL, NULL);
}

/*
 *  ======== setStationMode ========
 *  Sets the SimpleLink Wi-Fi in station mode and enables DHCP client
 */
void setStationMode(void)
{
    int           mode;
    int           response;
    unsigned char param;

    mode = sl_Start(0, 0, 0);
    if (mode < 0) {
        System_abort("Could not initialize SimpleLink Wi-Fi");
    }

    /* Change network processor to station mode */
    if (mode != ROLE_STA) {
        sl_WlanSetMode(ROLE_STA);

        /* Restart network processor */
        sl_Stop(0);
        mode = sl_Start(0, 0, 0);
        if (mode < 0) {
            System_abort("Failed to set SimpleLink Wi-Fi to Station mode");
        }
    }

    sl_WlanDisconnect();
    /* Set auto connect policy */
    response = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    if (response < 0) {
        System_abort("Failed to set connection policy to auto");
    }

    /* Enable DHCP client */
    param = 1;
    response = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &param);
    if(response < 0) {
       System_abort("Could not enable DHCP client");
    }

    sl_Stop(0);

    /* Set connection variables to initial values */
    deviceConnected = false;
    ipAcquired = false;
}

/*
 *  ======== wlanConnect =======
 *  Secure connection parameters
 */
static int wlanConnect()
{
    SlSecParams_t secParams = {0};
    int ret = 0;

    secParams.Key = (signed char *)SECURITY_KEY;
    secParams.KeyLen = strlen((const char *)secParams.Key);
    secParams.Type = SECURITY_TYPE;

    ret = sl_WlanConnect((signed char*)SSID, strlen((const char*)SSID),
            NULL, &secParams, NULL);

    return (ret);
}

/*
 *  ======== NetWifi_init =======
 *  Initialize Wifi
 */
void NetWiFi_init()
{
    SlNetCfgIpV4Args_t ipV4;
    uint8_t           len = sizeof(ipV4);
    uint8_t           dhcpIsOn;

    /* Turn LED OFF. It will be used as a connection indicator */
    GPIO_write(Board_LED0, Board_LED_OFF);
    GPIO_write(Board_LED1, Board_LED_OFF);
    GPIO_write(Board_LED2, Board_LED_OFF);

    setStationMode();

    /* Host driver starts the network processor */
    if (sl_Start(NULL, NULL, NULL) < 0) {
        System_abort("Could not initialize WiFi");
    }

    if (wlanConnect() < 0) {
        System_abort("Could not connect to WiFi AP");

    }

    /*
     * Wait for the WiFi to connect to an AP. If a profile for the AP in
     * use has not been stored yet, press Board_BUTTON0 to start SmartConfig.
     */
    while ((deviceConnected != true) || (ipAcquired != true)) {
        /*
         *  Start SmartConfig if a button is pressed. This could be done with
         *  GPIO interrupts, but for simplicity polling is used to check the
         *  button.
         */
        currButton = GPIO_read(Board_BUTTON0);
        if ((currButton == 0) && (prevButton != 0)) {
            smartConfigFxn();
        }
        prevButton = currButton;
        Task_sleep(50);
    }

    /* Retrieve & print the IP address */
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len,
            (unsigned char *)&ipV4);
    System_printf("CC3200 has connected to AP and acquired an IP address.\n");
    System_printf("IP Address: %d.%d.%d.%d\n", SL_IPV4_BYTE(ipV4.ipV4,3),
            SL_IPV4_BYTE(ipV4.ipV4,2), SL_IPV4_BYTE(ipV4.ipV4,1),
            SL_IPV4_BYTE(ipV4.ipV4,0));
    System_flush();

    GPIO_write(Board_LED0, Board_LED_ON);
}

/*
 *  ======== NetWifi_exit =======
 */
void NetWiFi_exit()
{
}
