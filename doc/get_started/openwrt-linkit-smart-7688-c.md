---
platform: linux
device: LinkIt Smart 7688
language: c
---

Run a simple C sample on LinkIt Smart 7688 device
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

The following document describes the process of connecting a LinkIt Smart 7688 device to Azure IoT Hub.

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
- Setup an Ubuntu x86 machine (for cross compiling)
- [Setup your IoT hub](../setup_iothub.md) 
- [Register the device in IoT hub](../manage_iot_hub.md)

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device
-  Connect the LinkIt Smart device using the mini-USB cable.
-  To setup your LinkIt Smart device, please refer the getting started instructions [here](<<http://labs.mediatek.com/site/global/developer_tools/mediatek_linkit_smart_7688/get_started/7688_duo/get_hardware_and_software/index.gsp>>) .

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

## Setup the development environment

This section shows how to set up the development environment on the host machine for the Azure IoT device SDK.

Install dependencies under root/sudo. 

``` 
apt-get install curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git unzip libssl-dev libncurses-dev gawk
```

- Clone this repository ([azure-iot-sdks](https://github.com/Azure/azure-iot-sdks)) to the machine you are using, being sure to do a recursive clone (git clone --recursive <repo address>).
- Navigate to the folder **c/build_all/arduino** in your local copy of the repository.
- Update the sdk_repo references to point to the ones for LinkIt Smart as shown below:
```
sdk_repo="http://download.labs.mediatek.com/MediaTek_LinkIt_Smart_7688_Openwrt_sdk_Linux"
sdk_repo_x86="http://download.labs.mediatek.com/MediaTek_LinkIt_Smart_7688_Openwrt_sdk_Linux"
sdk_repo_x64="http://download.labs.mediatek.com/MediaTek_LinkIt_Smart_7688_Openwrt_sdk_Linux"
```
- In the Makefile.iot, update the following references:
```
CC="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-gcc" \
GCC="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-gcc" \
CXX="$(TOOLCHAIN_DIR)/bin/mipsel-openwrt-linux-g++" \
```
- Run the `./setup.sh` script to install the OpenWRT SDK and prerequisites. By default, the SDK will be installed at **~/openwrt/sdk**
- Enter 'Y' to build the Azure IoT SDK.

 <a name="build"/>
## Build the sample

- Open the file **c/serializer/samples/simplesample_http/simplesample_http.c** in a text editor (for example nano)
- Locate the following code in the file:
```
static const char* connectionString = "[device connection string]";
```
- Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin). Save the changes.
- Run the `./build.sh` script in the **c/build_all/arduino** directory.   

<a name="deploy"/>
## Deploy the sample

- Open a shell and navigate to the installed OpenWRT SDK folder. By default, it is **~/openwrt/sdk**.
- Transfer the sample executable.

OpenWRT LinkIt Image:

```
scp ~openwrt/sdk/build_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2/azure-iot-sdks-1/serializer/samples/simplesample_http/simplesample_http root@mylinkit.local:/tmp
```

***Note: The uClibc version might be different on your setup and you might need to adjust the path accordingly.***

<a name="run"/>
## Run the sample

- Run the sample **/tmp/simplesample_http**
- See [Manage IoT Hub][lnk-manage-iothub] for information about the tools you can use to monitor the messages your device sends to your IoT hub and to send commands to your devices.

***Note: To send a command to the device from iothub-explorer or DeviceExplorer, the command should be like {"Name":"TurnFanOff","Parameters":{}}***

[setup-devbox-linux]: ../../c/doc/devbox_setup.md
[setup-iothub]: ../setup_iothub.md
[lnk-manage-iothub]: ../manage_iot_hub.md
