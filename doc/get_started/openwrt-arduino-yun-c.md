---
platform: linux
device: Arduino Yun
language: c
---

Run a simple C sample on Arduino Yun device
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Sample](#Step-3-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

The following document describes the process of connecting an Arduino Yun system to Azure IoT Hub.This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
  - Arduino Yun board.
  - Ubuntu x86 machine (for cross compiling)
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
-  Connect the Arduino Yun using the mini-USB cable.
-  If you need to setup your Arduino Yun device, please refer the getting started instructions [here](<https://www.arduino.cc/en/Guide/ArduinoYun>) .

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

## Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Arduino Yun.

Install dependencies under root/sudo. 

``` 
apt-get install curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre libssl-dev libncurses-dev subversion gawk
```

- Clone this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to the machine you are using.
- Navigate to the folder **c/build_all/arduino** in your local copy of the repository.
- Run the `./setup.sh` script to install the OpenWRT SDK and prerequisites. By default, the SDK will be installed at **~/openwrt/sdk**
- (Optional) Enter 'Y' to build the Azure IoT SDK.
- When prompted to build proton, enter 'N'

 <a name="build"/>
## Build the sample

- Open the file **c/serializer/samples/simplesample_http/simplesample_http.c** in a text editor (for example nano)
- Locate the following code in the file:
```
static const char* connectionString = "[device connection string]";
```
- Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.
- The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.
- Run the `./build.sh` script in the **c/build_all/arduino** directory.   

<a name="deploy"/>
## Deploy the sample

- Open a shell and navigate to the installed OpenWRT SDK folder. By default, it is **~/openwrt/sdk**.
- Transfer the sample executable.

OpenWRT Yun Image:

```
scp ~/openwrt/sdk/build_dir/target-mips_r2_uClibc-0.9.33.2/azure-iot-sdks-1/serializer/samples/simplesample_http/simplesample_http root@arduino.local:/tmp
```

LininoIO Yun Image:

```
scp ~/openwrt/sdk/build_dir/target-mips_r2_uClibc-0.9.33.2/azure-iot-sdks-1/serializer/samples/simplesample_http/simplesample_http root@linino.local:/tmp
```

***Note: The uClibc version might be different on your setup and you might need to adjust the path accordingly.***

## Make sure the certificates are installed

On the Arduino Yun device, install the ca-certificates package like below:

```
wget https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base/ca-certificates_20160104_all.ipk --no-check-certificate
opkg install ca-certificates_20160104_all.ipk
```
You might get an error message at this step(return code 127), but the certificates will be installed.

***Note: The certificate name mention above may change when newer version of certificate is released. If you get a 404 error while downloading the certificate file, please double check the CA certificate name under the base path [here](https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base) and update the certificate path accordingly.***

<a name="run"/>
## Run the sample

- Run the sample **/tmp/simplesample_http**
- Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_http** application.
- See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility.

***Note: To send a command to the device from iothub-explorer or DeviceExplorer, the command should be like {"Name":"TurnFanOff", "Parameters":{}}***

[setup-devbox-linux]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md

