---
platform: linux
device: kiwi mini
language: c
---

Run a simple C sample on Kiwi Mini device running OpenWrt
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

The following document describes the process of connecting an Kiwi Mini system to Azure IoT Hub.This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with a Git client installed so that you can access the azure-iot-sdks code on GitHub.
-   Kiwi Mini board.
-   Ubuntu machine (for cross compiling) 
-   [Setup your IoT hub](../setup_iothub.md) 
-   [Provision your device and get its credentials](../manage_iot_hub.md)

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
-  Connect the Kiwi Mini using the ssh or telnet with putty.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

## Setup the development environment

This section shows you how to set up a development environment for the Azure IoT device SDK for C on Kiwi Mini.

Install dependencies under root/sudo. 

``` 
apt-get install curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip openjdk-7-jre libssl-dev libncurses-dev subversion gawk
```

-   Clone this repository ([azure-iot-sdk-c]( https://github.com/Azure/azure-iot-sdk-c.git)) to the machine you are using, being sure to do a recursive clone (git clone --recursive <repo address>).
-   New a package dir in the dir OpenWrt/package , copy the source of c sdk to dir src , and then following the rule of OpenWrt Package to new a makefile.


<a name="build"></a>
## Build the sample

-   Open the file **src/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c** in a text editor (for example nano)
-   Open the file **src/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c** in a text editor (for example nano)
-   Open the file **src/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c** in a text editor (for example nano)
-   Locate the following code in the file:
```
static const char* connectionString = "[device connection string]";
```
-   Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.
-   make package/azure-iot-sdk-c/{clean,compile,install} V=s

<a name="deploy"></a>
## Deploy the sample

-   Open a shell and navigate to the installed OpenWRT SDK folder. By default, it is **~/openwrt/sdk**.
-   Transfer the sample executable. (You also can use tftp to copy the executable to Kiwi Mini.  As the same way of http sample, please transfer iothub_client_sample_mqtt and iothub_client_sample_amqp to Kiwi Mini.)

OpenWRT Image:

```
scp ~/openwrt/sdk/build_dir/target-mips_r2_uClibc-0.9.33.2/azure-iot-sdk-c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http root@OpenWrt:/usr/sbin

```

Chmod Image:

```
chmod +x /usr/sbin/iothub_client_sample_*
```

***Note: The uClibc version might be different on your setup and you might need to adjust the path accordingly.***

## Make sure the certificates are installed

On the Kiwi Mini device, install the ca-certificates package like below(Also you can use curl tool to download the ipk.):

    wget https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base/ca-certificates_20161130_all.ipk --no-check-certificate
    opkg install ca-certificates_20161130_all.ipk

You might get an error message at this step(return code 127), but the certificates will be installed.

***Note: The certificate name mention above may change when newer version of certificate is released. If you get a 404 error while downloading the certificate file, please double check the CA certificate name under the base path [here](https://downloads.openwrt.org/snapshots/trunk/ar71xx/generic/packages/base) and update the certificate path accordingly.***

<a name="run"></a>
## Run the sample

-   Run the sample iothub_client_sample_http,iothub_client_sample_mqtt,iothub_client_sample_amqp
-   See [Manage IoT Hub][lnk-manage-iothub] for information about the tools you can use to monitor the messages your device sends to your IoT hub and to send commands to your devices.


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/devbox_setup.md
[setup-iothub]: ../setup_iothub.md
[lnk-manage-iothub]: ../manage_iot_hub.md


