# Arduino WiFi Shield 101 and MKR1000 Setup

The following document describes the process of connecting an Arduino WiFi Shield 101 or MKR1000 based system to the Azure IoT Hub.

## Table of Contents

- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Setup the development environment](#setup)
- [Build the sample](#build)
- [Deploy the sample](#deploy)
- [Run the sample](#run)

<a name="requirements"/>
## Requirements

  - A computer with the [Arduino IDE](https://www.arduino.cc/en/Main/Software) version 1.6.6 or later installed.
  - One of the following:
    - [Arduiono or Genuino MKR1000](https://www.arduino.cc/en/Main/ArduinoMKR1000)
    - [Arduino or Genuino Zero board](https://www.arduino.cc/en/Main/ArduinoBoardZero) and [Arduino or Genuino WiFi Shield 101](https://www.arduino.cc/en/Main/ArduinoWiFiShield101).

<a name="beforebegin"/>
## Before you begin

Before you begin you will need to create and configure an IoT hub to connect to.

  1. [Set up your IoT Hub][setup-iothub].
  2. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  3. Make note of the Connection String for your device from the previous step.

Note: You can skip this step if you just want to build the sample application without running it.

<a name="setup"/>
## Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK with the Arduino or Genuino WiFi 101 shield or MKR1000 board.

1. Open the Arduino IDE and install the following libraries using the ```Library Manager```, which can be accessed via ```Sketch -> Include Library -> Managed Libraries ...```
  * ```WiFi101```
  * ```RTCZero```
  * ```AzureIoT```

2. Flash the root certificate of the Azure IoT hub host, ```<name>.azure-devices.net``` found in Connection String from [earlier](#beforebegin), using the instructions available on the ["Firmware/Certificates updater for the Arduino Wifi 101 Shield"](https://github.com/arduino-libraries/WiFi101-FirmwareUpdater#usage) page.

<a name="build"/>
## Build the sample

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
6. Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.
7. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.
8. Press the ```Verify``` button in the Arduino IDE to build the sample sketch.  

<a name="deploy"/>
## Deploy the sample

1. Follow the steps from the [build](#build) section.
2. If you are using the Zero, connect the board to the WiFi Shield 101 and then connect the programming port of the board to your computer with a USB cable. For the MKR1000, connect the board to your computer with a USB cable.
3. Select serial port associated with the board in the ```Tools -> Port``` menu.
4. Press the ```Upload``` button in the Arduino IDE to build and upload the sample sketch.

<a name="run"/>
## Run the sample


1. Follow the steps from the [deploy](#deploy) section.
2. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_http** application.
3. See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility.

[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ../../tools/iothub-explorer/doc/provision_device.md
