// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


// Use Arduino IDE 1.6.8 or later.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <SPI.h>
#ifdef ARDUINO_ARCH_ESP8266
// for ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#elif ARDUINO_SAMD_FEATHER_M0
// for Adafruit WINC1500
#include <Adafruit_WINC1500.h>
#include <Adafruit_WINC1500SSLClient.h>
#include <Adafruit_WINC1500Udp.h>
#include <NTPClient.h>
#else
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#endif

#ifdef ARDUINO_SAMD_FEATHER_M0
// For the Adafruit WINC1500 we need to create our own WiFi instance
// Define the WINC1500 board connections below.
#define WINC_CS   8
#define WINC_IRQ  7
#define WINC_RST  4
#define WINC_EN   2     // or, tie EN to VCC
// Setup the WINC1500 connection with the pins above and the default hardware SPI.
Adafruit_WINC1500 WiFi(WINC_CS, WINC_IRQ, WINC_RST);
#endif

#include <AzureIoTHub.h>
#include <AzureIoTUtility.h>
#include <AzureIoTProtocol_HTTP.h>

#include "simplesample_http.h"

static char ssid[] = "yourNetwork";     // your network SSID (name)
static char pass[] = "yourPassword";    // your network password (use for WPA, or use as key for WEP)

// In the next line we decide each client ssl we'll use.
#ifdef ARDUINO_ARCH_ESP8266
static WiFiClientSecure sslClient; // for ESP8266
#elif ARDUINO_SAMD_FEATHER_M0
static Adafruit_WINC1500SSLClient sslClient; // for Adafruit WINC1500
#else
static WiFiSSLClient sslClient;
#endif

static AzureIoTHubClient iotHubClient;

void setup() {
    initSerial();
    initWifi();
    initTime();

    iotHubClient.begin(sslClient);
}

void loop() {
    simplesample_http_run();
}

void initSerial() {
    // Start serial and initialize stdout
    Serial.begin(115200);
    Serial.setDebugOutput(true);
}

void initWifi() {
#ifdef ARDUINO_SAMD_FEATHER_M0
    // for the Adafruit WINC1500 we need to enable the chip
    pinMode(WINC_EN, OUTPUT);
    digitalWrite(WINC_EN, HIGH);
#endif

    // check for the presence of the shield :
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present");
        // don't continue:
        while (true);
    }

    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // unsuccessful, retry in 4 seconds
        Serial.print("failed ... ");
        delay(4000);
        Serial.print("retrying ... ");
    }

    Serial.println("Connected to wifi");
}

void initTime() {
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_FEATHER_M0)
#ifdef ARDUINO_SAMD_FEATHER_M0
    Adafruit_WINC1500UDP ntpUdp; // for Adafruit WINC1500
#else
    WiFiUDP ntpUdp;
#endif
    NTPClient ntpClient(ntpUdp);

    ntpClient.begin();

    while (!ntpClient.update()) {
        Serial.println("Fetching NTP epoch time failed! Waiting 5 seconds to retry.");
        delay(5000);
    }

    ntpClient.end();

    unsigned long epochTime = ntpClient.getEpochTime();

    Serial.print("Fetched NTP epoch time is: ");
    Serial.println(epochTime);

    iotHubClient.setEpochTime(epochTime);

#elif ARDUINO_ARCH_ESP8266
    time_t epochTime;

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true) {
        epochTime = time(NULL);

        if (epochTime == 0) {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        } else {
            Serial.print("Fetched NTP epoch time is: ");
            Serial.println(epochTime);
            break;
        }
    }
#endif
}
