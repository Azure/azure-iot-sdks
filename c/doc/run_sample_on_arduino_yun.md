# Arduino Yun Setup

The following document describes the process of connecting an Arduino Yun system to Azure IoT Hub.

## Table of Contents

- [Requirements](#requirements)
- [Before you begin](#beforebegin)
- [Setup the OpenWRT SDK](#setup)
- [Build the sample](#build)
- [Deploy the sample](#deploy)
- [Run the sample](#run)

<a name="requirements"/>
## Requirements

  - Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
  - Arduino Yun board.
  - Debian x86

<a name="beforebegin"/>
## Before you begin

Before you begin you will need to create and configure an IoT hub to connect to.

  1. [Set up your IoT Hub][setup-iothub].
  2. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  3. Make note of the Connection String for your device from the previous step.

Note: You can skip this step if you just want to build the sample application without running it.

<a name="setup"/>
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

This script builds the **iothub_client** and **serializer** libraries and their associated samples.

Note: you will not be able to run the samples until you configure them with a valid IoT Hub device connection string. For more information, see [Run sample on Linux](run_sample_on_desktop_linux.md).

 <a name="build"/>
## Build the sample

- Open the file **c/serializer/samples/simplesample_http/simplesample_http.c** in a text editor.
- Locate the following code in the file:
```
static const char* connectionString = "[device connection string]";
```
- Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.
- The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.
- Run the `build.sh` script in the **c/build_all/arduino** directory.   

<a name="deploy"/>
## Deploy the sample

- Open a shell and navigate to the installed OpenWRT SDK folder. By default, it is **~/openwrt/sdk**.
- Transfer the package.

OpenWRT Yun Image:

```
scp ~/openwrt/sdk/build_dir/target-mips_r2_uClibc-0.9.33.2/azure-iot-sdks-1/serializer/samples/simplesample_http/simplesample_http root@arduino.local:/tmp
```

LininoIO Yun Image:

```
scp ~/openwrt/sdk/build_dir/target-mips_r2_uClibc-0.9.33.2/azure-iot-sdks-1/serializer/samples/simplesample_http/simplesample_http root@linino.local:/tmp
```

Note that the uClibc version might be different on your setup and you might need to adjust the path accordingly.

## Make sure the certificates are installed

Install the ca-certificates package like below:

```
wget https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base/ca-certificates_20151214_ar71xx.ipk --no-check-certificate
opkg install ca-certificates_20151214_ar71xx.ipk
```

You might get an error message at this step(return code 127), but the certificates will be installed.

<a name="run"/>
## Run the sample

The following instructions assumes the device is flashed with OpenWRT Yun. If you are using LininoIO, substitute `arduino` with `linino` and use the default password `doghunter`.

- Open shell and enter `ssh root@arduino.local` Enter the device password. By default it is, `arduino` for OpenWRT Yun Image or `doghunter` for LininoIO.
- Run the sample **/tmp/simplesample_http**
- Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_http** application.
- See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility.

[setup-devbox-linux]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
