# CC3200 Setup

This file contains the following information:
- How to build the Azure SDK and the *simplesample_http* sample for the TI CC3200 launchpad.
- How to run the sample.

## Table of Contents
- [Before you begin](#beforebegin)
- [Prerequisites](#Prerequisites)
- [Build the Azure SDK](#Build-SDK)
- [Build the sample simplesample_http application](#Build-SAMPLE)
- [Flash the sample simplesample_http and the root certificate](#Flash-SAMPLE)
- [Run the sample simplesample_http](#Run-SAMPLE)

<a name="beforebegin"/>
## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

<a name="Prerequisites"></a>
## Prerequisites
Required hardware: [CC3200 Launchpad](http://www.ti.com/tool/cc3200-launchxl).

This procedure assumes you have already installed the Azure IoT SDK on your development machine. These instructions refer to the folder that contains the Azure IoT SDK on your local machine as `<AZURE_INSTALL_DIR>`.

> **Note:** You can install the Azure IoT SDK using
`git clone https://github.com/Azure/azure-iot-sdks.git`

While not strictly required, we recommend that you install the following tools from TI in the same directory and that you use directory names without any whitespace. This documentation assumes that you install everything in `C:\ti`.

- Install [Uniflash Tool](http://www.ti.com/tool/Uniflash) 3.2 or later for SimpleLink CC3200

- Install [CC3200 SDK 1.1.0](http://www.ti.com/tool/cc3200sdk)

- Install [TI-RTOS SDK for SimpleLink](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html) 2.14.01.20 or above

- Install [NS package](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/ns/ns_1_10_00_00_eng.zip). Please note, the NS package is a pre-release version. It's content and the download location are subject to change.

- Install [TI ARM Compiler](http://software-dl.ti.com/ccs/esd/test/ti_cgt_tms470_5.2.5_windows_installer.exe) 5.2.2 or above

<a name="Build-SDK"></a>
## Build the Azure IoT SDK
1. Edit the `products.mak` file in `<AZURE_INSTALL_DIR>\azure-iot-sdks\c\build_all\tirtos` using your favorite text editor. The variables `XDCTOOLS_INSTALLATION_DIR, TIRTOS_INSTALLATION_DIR, CC3200SDK_INSTALLATION_DIR` and `NS_INSTALLATION_DIR` must point to the locations where you installed these products. The variable `ti.targets.arm.elf.M4` should point to the installation location of the TI ARM compiler. After modification, these variable definitions should look similar to the following. Note the use of "/" in the path.

  ```
  XDCTOOLS_INSTALLATION_DIR ?= C:/ti/xdctools_3_31_01_33_core
  TIRTOS_INSTALLATION_DIR ?= C:/ti/tirtos_simplelink_2_14_01_20
  NS_INSTALLATION_DIR     ?= C:/ti/ns_1_10_00_00_eng
  CC3200SDK_INSTALLATION_DIR ?= C:/ti/CC3200SDK_1.1.0/cc3200-sdk
  ti.targets.arm.elf.M4  ?= C:/ti/cgt_arm_5.2.5
  ```

2. Open a Windows command prompt.

3. In the Windows command prompt, run the following commands (be sure to replace the paths with your installation paths).

  ```
  cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\build_all\tirtos
  C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
  C:\ti\xdctools_3_31_01_33_core\gmake.exe all
  ```

<a name="Build-SAMPLE"></a>
## Build the sample simplesample_http application
Before building the application, complete the following steps:

1. Open the `simplesample_http.c` file from the directory `<AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http` in a text editor and replace the value of the "connectionString" variable with the device connection string you noted [earlier](#beforebegin).

2. Open the file `tirtos/cc3200/main.c`. Search for "USER STEP" and update the current date-time macros.

3. Open the file `tirtos/cc3200/wificonfig.h`. Search for "USER STEP" and update the WIFI SSID and SECURITY_KEY macros.

4. Download the [elf2cc32 executable for Windows](https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32.exe?raw=true) or [elf2cc32 executable for Linux](https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32?raw=true) to the folder <AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http\tirtos\cc3200\tools. This tool is needed for converting the simplesample_http.out to simplesample_http.bin file.

In the Windows command prompt, enter the following commands to build the application:

  ```
  cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http\tirtos\cc3200
  C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
  C:\ti\xdctools_3_31_01_33_core\gmake.exe all
  ```

<a name="Flash-SAMPLE"></a>
## Flash the sample simplesample_http and the root certificate

Flash the `simplesample_http.bin` file to your CC3200 Launchpad.

> Note: The root CA certificate - "Baltimore CyberTrust Root" has to be flashed to CC3200 Launchpad to the location `/cert/ms.der`. This location is referenced in `tirtos/cc3200/main.c` and is used by SimpleLink TLS stack.

Here's why you need the Baltimore root CA - it's the root CA for `*.azure-devices.net`, the IoT Hub endpoint and it's the only way for the tiny CC3200 to verify the chain of trust:

![image](https://cloud.githubusercontent.com/assets/6472374/11576321/71207be4-9a1e-11e5-9332-fa99fdbd31f9.png)

The detailed information about the flash tool can be found in the [CC3200 UniFlash wiki](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash). The section [GUI Interface](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#GUI_Interface) walks through the steps for using the UniFlash tool.

Flash the application (.bin file) to the /sys/mcuimg.bin under System Files. For the certificate, [add a new file](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#Adding_a_new_file_to_the_device) in the path /cert/ms.der and provide the path to the "Baltimore CyberTrust Root" certificate (.der format). The ms.der file is available at <AZURE_INSTALL_DIR>\azure-iot-sdks\c\certs\ms.der.

<a name="Run-SAMPLE"></a>
## Running the sample simplesample_http
After flashing the example, open a serial session to the appropriate COM port with the following settings:

```
Baudrate:     9600
Data bits:       8
Stop bits:       1
Parity:       None
Flow Control: None
```

Run the application by pressing the reset button on the CC3200 Launchpad. The application prints a message similar to the following:

```
Starting the simplesample_http example
CC3200 has connected to AP and acquired an IP address.
IP Address: XXX.XXX.XX.XXX
IoTHubClient accepted the message for delivery
Message Id: 1 Received.
Result Call Back Called! Result is: IOTHUB_CLIENT_CONFIRMATION_OK
```

The Device Explorer can be used to monitor the data sent by the application. Under the "Data" tab in Device Explorer, "Monitor" option should be selected before running the application. Later when the application is run, a message similar to the following message is displayed on "Event Hub Data" window.

```
9/17/2015 7:28:28 PM> Data:[{"Temperature":67, "Humidity":42}]
```

[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ../../tools/iothub-explorer/doc/provision_device.md
