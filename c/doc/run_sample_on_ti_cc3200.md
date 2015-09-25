# CC3200 Setup

This file contains the following information:
- How to build the Azure SDK and the simplesample_http sample for the TI CC3200 launchpad
- How to run the sample

## Table of Contents
- [Requirements](#Requirements)
- [Prerequisites](#Prerequisites)
        - [Install Uniflash](#Install-Uniflash)
        - [Install CC3200 SDK](#Install-CC3200SDK)
        - [Install TI-RTOS](#Install-TIRTOS)
        - [Install NS package](#Install-NS)
        - [Install TI ARM compiler](#Install-ARMCGT)
- [Build the Azure SDK](#Build-SDK)
- [Build the sample simplesample_http](#Build-SAMPLE)
- [Flash the sample simplesample_http and the root certificate](#Flash-SAMPLE)
- [Run the sample simplesample_http](#Run-SAMPLE)

<a name="Requirements"></a>
## Requirements
Required hardware: [CC3200 Launchpad](http://www.ti.com/tool/cc3200-launchxl).

<a name="Prerequisites"></a>
## Prerequisites
This procedure assumes you have the AZURE SDK already installed in the directory `<AZURE_INSTALL_DIR>`.
While not strictly required, it is suggested to install the following tools from TI under the same directory and we would recommend using directory names without white-spaces. For this documentation we would assume you have installed everything under `C:\ti`.

<a name="Install-Uniflash"></a>
### Install Uniflash tool 3.2 or later for SimpleLink CC3200
[Uniflash Tool](http://www.ti.com/tool/Uniflash)

<a name="Install-CC3200SDK"></a>
### Install CC3200 SDK 1.1.0
[CC3200 SDK 1.1.0](http://www.ti.com/tool/cc3200sdk)

<a name="Install-TIRTOS"></a>
### Install TI-RTOS for SimpleLink 2.14.01.20 or above
[TI-RTOS SDK for SimpleLink](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html)

<a name="Install-NS"></a>
### Install the NS package
(link TBD)

<a name="Install-ARMCGT"></a>
### Install TI ARM compiler 5.2.2 or above
[TI ARM Compiler](http://software-dl.ti.com/ccs/esd/test/ti_cgt_tms470_5.2.5_windows_installer.exe)

<a name="Build-SDK"></a>
## Build the Azure IoT SDK
1. Update `products.mak` in `<AZURE_INSTALL_DIR>\azure-iot-sdks\c\build_all\tirtos`. Edit the file in your favorite text editor. The variables `XDCTOOLS_INSTALLATION_DIR, TIRTOS_INSTALLATION_DIR, CC3200SDK_INSTALLATION_DIR` and `NS_INSTALLATION_DIR` should point to the locations where you have these products installed. The variable `ti.targets.arm.elf.M4` should point to the installation location of the TI ARM compiler. After modification, they should look roughly as follow. Please note the use of "/" in the path.

        XDCTOOLS_INSTALLATION_DIR ?= C:/ti/xdctools_3_31_01_33_core
        TIRTOS_INSTALLATION_DIR ?= C:/ti/tirtos_simplelink_2_14_01_20
        NS_INSTALLATION_DIR     ?= C:/ti/ns_1_10_00_00_eng
        CC3200SDK_INSTALLATION_DIR ?= C:/ti/CC3200SDK_1.1.0/cc3200-sdk
        ti.targets.arm.elf.M4  ?= C:/ti/cgt_arm_5.2.5

2. Open the Windows command prompt
In Windows, run 'cmd' from the search box under the Start Menu

3. In the Windows command prompt, run the following commands (replace the paths with your actual installation paths)

        > cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\build_all\tirtos
        > C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
        > C:\ti\xdctools_3_31_01_33_core\gmake.exe all

<a name="Build-SAMPLE"></a>
##Build the sample simplesample_http
Before building the application, do the following steps:

- Open `simplesample_http.c` from the directory `<AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http`
  and update the "connectionString". This string can be obtained from the Device Explorer.

- Open `tirtos/cc3200/main.c`. Search for "USER STEP" and update the current
  date-time macros.

- Open `tirtos/cc3200/wificonfig.h`. Search for "USER STEP" and update the WIFI
  SSID and SECURITY_KEY macros.

In the Windows Command Prompt, enter the following to build the application:

    > cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http\tirtos\cc3200
    > C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
    > C:\ti\xdctools_3_31_01_33_core\gmake.exe all

Flash the `simplesample_http.bin` to your CC3200 Launchpad.

Note: The root CA certificate - "Baltimore CyberTrust Root" has to be flashed to CC3200 Launchpad to the location `/cert/ms.der`. This location is referenced in `tirtos/cc3200/main.c` and is used by SimpleLink TLS stack.

<a name="Flash-SAMPLE"></a>
## Flash the sample simplesample_http and the root certificate

The detailed information about the flash tool can be found in the [CC3200 UniFlash wiki](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash). The section [GUI Interface](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#GUI_Interface) walks through the steps for using the UniFlash tool.

Flash the application (.bin file) to the /sys/mcuimg.bin under System Files. For the certificate, [add a new file](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#Adding_a_new_file_to_the_device) in the path /cert/ms.der and provide the path to the "Baltimore CyberTrust Root" certificate (.der format).

<a name="Run-SAMPLE"></a>
## Running the sample simplesample_http
After flashing the example, open a serial session to the appropriate COM port with the following settings:

    Baudrate:     9600
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None

Run the application by pressing the reset button on the CC3200 Launchpad. The application prints a message similar to the following message:

    Starting the simplesample_http example
    CC3200 has connected to AP and acquired an IP address.
    IP Address: XXX.XXX.XX.XXX
    IoTHubClient accepted the message for delivery
    Message Id: 1 Received.
    Result Call Back Called! Result is: IOTHUB_CLIENT_CONFIRMATION_OK

The Device Explorer can be used to monitor the data sent by the application. Under the "Data" tab in Device Explorer, "Monitor" option should be selected before running the application. Later when the application is run, a message similar to the following message is displayed on "Event Hub Data" window.

    9/17/2015 7:28:28 PM> Data:[{"Temperature":67, "Humidity":42}]
