# AzureIoTProtocol_HTTP - Azure IoT HTTP protocol library for Arduino

[![Build Status](https://travis-ci.org/arduino-libraries/AzureIoTProtocol_HTTP.svg?branch=master)](https://travis-ci.org/arduino-libraries/AzureIoTProtocol_HTTP)

This library is a port of the compact implementation of the HTTP protocol from [Microsoft Azure IoT device SDK for C](https://github.com/Azure/azure-iot-sdks/blob/master/c/readme.md) to Arduino. Together with the AzureIoTHub, it allows you to use several Arduino compatible boards with Azure IoT Hub. Please submit any contribution directly to [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks).

Currently supported hardware:
- Atmel SAMD Based boards
  - Arduino/Genuino [MKR1000](https://www.arduino.cc/en/Main/ArduinoMKR1000)
  - Arduino/Genuino [Zero](https://www.arduino.cc/en/Main/ArduinoBoardZero) and [WiFi Shield 101](https://www.arduino.cc/en/Main/ArduinoWiFiShield101) 
  - Adafruit [Feather M0](https://www.adafruit.com/products/3010)
- ESP8266 based boards with [esp8266/arduino](https://github.com/esp8266/arduino)
  - SparkFun [Thing](https://www.sparkfun.com/products/13711)
  - Adafruit [Feather Huzzah](https://www.adafruit.com/products/2821)

## Prerequisites

You should have the following ready before beginning with any board:
-   [Setup your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md)
-   [Provision your device and get its credentials](https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md)
-   [Arduino IDE 1.6.8](https://www.arduino.cc/en/Main/Software)
-   Install the `AzureIoTHub` library via the Arduino IDE Library Manager
-   Install the `AzureIoTUtility` library via the Arduino IDE Library Manager
-   Install the `AzureIoTProtocol_HTTP` library via the Arduino IDE Library Manager

# Simple Sample Instructions

## MKR1000 or Zero + Wifi101
1. Open the `simplesample_http` example from the Arduino IDE File->Examples->AzureIoTHub menu.
2. Install the `WiFi101` library from the Arduino IDE Library Manager.
3. Install the `NTPClient` library from the Arduino IDE Library Manager.
4. Install the `RTCZero` library from the Arduino IDE Library Manager.
5. Update Wifi SSID/Password in simplesample_http.ino 
    * Ensure you are using a wifi network that does not require additional manual steps after connection, such as opening a web browser.
6. Update IoT Hub Connection string in simplesample_http.c

## ESP8266
##### Sparkfun Thing, Adafruit Feather Huzzah, or generic ESP8266 board

1. Install esp8266 board support into your Arduino IDE.
    * Start Arduino and open Preferences window.
    * Enter `http://arduino.esp8266.com/stable/package_esp8266com_index.json` into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
    * Open Boards Manager from Tools > Board menu and install esp8266 platform 2.2.0 or later 
    * Select your ESP8266 board from Tools > Board menu after installation
    
2. Open the `simplesample_http` example from the Arduino IDE File->Examples->AzureIoTHub menu.
3. Update the sketch as directed by comments in the sample to support the ESP8266 board.
4. Update Wifi SSID/Password in simplesample_http.ino 
    * Ensure you are using a wifi network that does not require additional manual steps after connection, such as opening a web browser.
5. Update IoT Hub Connection string in simplesample_http.c
6. Access the [SparkFun Get Started](https://azure.microsoft.com/en-us/documentation/samples/iot-hub-c-thingdev-getstartedkit/) tutorial to learn more about Microsoft Sparkfun Dev Kit.
7. Access the [Huzzah Get Started](https://azure.microsoft.com/en-us/documentation/samples/iot-hub-c-huzzah-getstartedkit/) tutorial to learn more about Microsoft Huzzah Dev Kit.


## Adafruit Feather M0
1. Install Feather M0 board support into your Arduino IDE.
    * Start Arduino and open Preferences window.
    * Enter `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json` into Additional Board Manager URLs field. You can add multiple URLs, separating them with commas.
    * Open Boards Manager from Tools > Board menu and install Adafruit SAMD Boards 1.0.7 or later.
    * Select your Adafruit Feather M0 from Tools > Board menu after installation
2. Install the [Adafruit WINC1500 wifi library](https://learn.adafruit.com/adafruit-feather-m0-wifi-atwinc1500/using-the-wifi-module)
3. Install the `RTCZero` library from the Arduino IDE Library Manager.
4. Install the `NTPClient` library from the Arduino IDE Library Manager.
5. Open the `simplesample_http` example from the Arduino IDE File->Examples->AzureIoTHub menu.
6. Update the sketch as directed by comments in the sample to support the ESP8266 board.
7. Update Wifi SSID/Password in simplesample_http.ino 
    * Ensure you are using a wifi network that does not require additional manual steps after connection, such as opening a web browser.
8. Update IoT Hub Connection string in simplesample_http.c
9. Access the [Feather M0 WiFi Get Started](https://azure.microsoft.com/en-us/documentation/samples/iot-hub-c-m0wifi-getstartedkit/) tutorial to learn more about Microsoft Feather M0 WiFi Dev Kit.

## License

See [LICENSE](LICENSE) file.

[azure-certifiedforiot]:  http://azure.com/certifiedforiot 
[Microsoft-Azure-Certified-Badge]: images/Microsoft-Azure-Certified-150x150.png (Microsoft Azure Certified)
