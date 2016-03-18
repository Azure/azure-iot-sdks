---
platform: arduino ide
device: arduino wifi shield 101
language: c
---

Run a simple C sample on Arduino WiFi Shield 101 and MKR1000 device
===
---

# Table of Contents

- [Introduction](#Introduction)
- [Step 1: Prerequisites](#Step-1-Prerequisites)
- [Step 2: Prepare your Device](#Step-2-PrepareDevice)
- [Step 3: Build and Run the Sample](#Step-3-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes the process of connecting an Arduino WiFi Shield 101 or MKR1000 based system to the Azure IoT Hub. This multi-step process includes:
- Configuring Azure IoT Hub
- Registering your IoT device
- Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
- Computer with Git client installed and access to the
  [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
- A computer with the [Arduino IDE](https://www.arduino.cc/en/Main/Software) version 1.6.6 or later installed.
- One of the following:
  - [Arduiono or Genuino MKR1000](https://www.arduino.cc/en/Main/ArduinoMKR1000)
  - [Arduino or Genuino Zero board](https://www.arduino.cc/en/Main/ArduinoBoardZero) and [Arduino or Genuino WiFi Shield 101](https://www.arduino.cc/en/Main/ArduinoWiFiShield101).
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

#### IMPORTANT NOTE
The sample won't run on older 8-bit AVR-based Arduinos (like the UNO or Mega 2560). A modern 32-bit ARM Cortex M0+ MCU is required (e.g. Atmel’s SAMD21 MCU).

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

This section shows you how to set up a development environment for the Azure IoT device SDK with the Arduino or Genuino WiFi 101 shield or MKR1000 board.

1. Open the Arduino IDE and install the following libraries using the ```Library Manager```, which can be accessed via ```Sketch -> Include Library -> Managed Libraries ...```
  * ```WiFi101```
  * ```RTCZero```
  * ```AzureIoT```

2. Flash the root certificate of the Azure IoT hub host, ```<name>.azure-devices.net``` found in Connection String from [earlier](#beforebegin), using the instructions available on the ["Firmware/Certificates updater for the Arduino Wifi 101 Shield"](https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#usage) page.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

1. In the Arduino IDE, select ```Arduino/Genuino MKR1000``` or ```Arduino/Genuino Zero (Programming Port)``` as the board in the ```Tools -> Board``` menu.
2. Open the ```simplesample_http``` example in the Arduino IDE, ```File -> Examples -> AzureIoT -> sdk -> simplesample_http```
3. In ```simplesample_http.ino```, update the following line with your WiFi accesspoint's SSID:
   ```
   char ssid[] = "yourNetwork"; //  your network SSID (name)
   ```
4. Update the following line with your WiFi accesspoint's password:
   ```
   char pass[] = "secretPassword";    // your network password (use for WPA, or use as key for WEP)
   ```
5. Locate the following code in the ```simplesample_http.c```:
   ```
   static const char* connectionString = "[device connection string]";
   ```
6. Replace `[device connection string]` with the device connection string you noted [earlier](#Step-1-Prerequisites). 
7. Locate the following code in the ```simplesample_http.c```:
   ```
   myWeather->DeviceId = "myFirstDevice";
   ```
8. Replace `"myFirstDevice"` with the name you gave to the device [earlier](#Step-1-Prerequisites). 
9. Save the changes. Press the ```Verify``` button in the Arduino IDE to build the sample sketch.  

<a name="deploy"/>
## Deploy the sample

1. Follow the steps from the [build](#Step-3-Build) section.
2. If you are using the Zero, connect the board to the WiFi Shield 101 and then connect the programming port of the board to your computer with a USB cable. For the MKR1000, connect the board to your computer with a USB cable.
3. Select serial port associated with the board in the ```Tools -> Port``` menu.
4. Press the ```Upload``` button in the Arduino IDE to build and upload the sample sketch.

<a name="run"/>
## Run the sample

1. Follow the steps from the [deploy](#deploy) section.

7.   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the **simplesample_http** application and how to send cloud-to-device messages to the **simplesample_http** application.

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
