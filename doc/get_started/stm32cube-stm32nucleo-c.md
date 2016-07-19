---
platform: STM32 Cube 

device: STM32 Nucleo

language: c

---

Run a simple C sample on STM32 Nucleo  
===
---

# Table of Contents
-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)


<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect STM32 Nucleo based sensor node to the Microsoft Azure IoT Hub, by leveraging on Azure IoT Device SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering STM32 Nucleo to Azure IoT Hub
-   Build and deploy Azure IoT SDK on STM32 Nucleo

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process.

## 1.1 Development environment
- One among these three IDEs installed in your PC: [Keil MDK-ARM][lnk-ide-keil], [IAR Embedded Workbench][lnk-ide-iar], [AC6 System Workbench for STM32][lnk-ide-sw4stm32]
- A serial terminal installed in your PC (e.g. [TeraTerm][lnk-teraterm]) 
- [Setup your IoT hub][lnk-setup-iot-hub]
- [Provision your device and get its credentials][lnk-manage-iot-hub]

#### NOTE
[SystemWorkbench for STM32][lnk-ide-sw4stm32] is the free integrated development environment for STM32, and it is used as reference in this guide.

## 1.2 Hardware components
 - STM32 Nucleo development board ([NUCLEO-F401RE][lnk-nucleo-f4])
 - Wi-Fi expansion board for STM32 Nucleo ([X-NUCLEO-IDW01M1][lnk-nucleo-wifi])
 - Motion MEMS and environmental sensor expansion board for STM32 Nucleo ([X-NUCLEO-IKS01A1][lnk-nucleo-sensors])
 - Dynamic NFC tag expansion board for STM32 Nucleo ([X-NUCLEO-NFC01A1][lnk-nucleo-nfc]) 
 - NFC enabled Android phone to set SSID and Password of WiFi access point through a mobile application.
 

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
Combine STM32 Nucleo with its expansion boards as shown in the figure below:

![][1]

Connect the STM32 Nucleo based sensor node to your PC using a mini USB cable. 

<a name="Build"></a>
# Step 3: Build and Run the sample 

<a name="Load"></a>
## 3.1 Build SDK and sample code

1. Download [FP-CLD-AZURE1 Function Pack][lnk-nucleo-fp]. The Function Pack contains all the required drivers to use the STM32 Nucleo board with Wi-Fi, Sensors and NFC expansion boards, together with pre-integrated Microsoft Azure IoT SDK. 
2. Unzip the package and open one of the pre-configured project files available in ```FP-CLD-AZURE1/Projects/Multi/Applications/HTTPS_AzureSDK_S2C```, according to the IDE installed. In [SystemWorkbench for STM32][lnk-ide-sw4stm32] select the project from menu ```File -> Import -> Existing Projects into Workspace```; browse folders and select as root directory ```FP-CLD-AZURE1/Projects/Multi/Applications/HTTPS_AzureSDK_S2C/SW4STM32/STM32F401RE-Nucleo```
3. Open  file AzureIOTSDKConfig.h and update AZUREIOTHUBCONNECTIONSTRING with the credentials retrieved when registering the device in IoT Hub as described in [Step 1.1][lnk-setup-iot-hub].
4. Recompile the project, according to the chosen IDE, and flash the binary to the Nucleo board. In [SystemWorkbench for STM32][lnk-ide-sw4stm32] this can be done simply by clicking on Run button, as shown in picture below. 

![][2]


## 3.2 Connect STM32 Nucleo board to a WiFi access point 
In order to set or to overwrite WiFi access point parameters (SSID and Password), keep pressed USER button in STM32 Nucleo after RESET. If SSID and Password were previously written to NFC, these will be read and used to connect with the Access Point. In order to pass parameters to NFC board, an Android mobile phone equipped with an NFC WriteTag application need to be used (i.e. you can install and use [ST25 application][lnk-android-st25] ). Alternatively WiFi parameters can be configured via Serial Terminal, or directly set in source code by replacing X_CLOUD_SSID, X_CLOUD_KEY in file AzureIOTSDKConfig.h.
After first setting of WiFi parameters, these are permanently written to FLASH memory and used each time the board is RESET, unless USER button is kept pressed. 

![][3]


## 3.3 Send Device Events to IoT Hub:

After connection with a WiFi Access Point, the application contacts Azure IoT Hub using HTTPS protocol and it starts to transmit sample data read from sensor board (motion MEMS, temperature, pressure, humidity). 

See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the simplesample_http application running in STM32 Nucleo.

Messages sent to Azure IoT are also printed in Serial Terminal interface. 


## 3.3 Receive messages from IoT Hub

See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.
Messages received by STM32 Nucleo are printed over Serial Terminal interface once received. 
Green LED2 on Nucleo alternatively turn on/turn off every time a message from Azure IoT Hub is received. 

![][4]




[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[lnk-nucleo-f4]:http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-nucleo/nucleo-f401re.html
[lnk-nucleo-wifi]:http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idw01m1.html
[lnk-nucleo-sensors]:http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-sense-hw/x-nucleo-iks01a1.html
[lnk-nucleo-nfc]:http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-nfc01a1.html
[lnk-nucleo-fp]:http://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32-ode-function-pack-sw/fp-cld-azure1.html
[lnk-ide-keil]:http://www.keil.com/
[lnk-ide-iar]:http://www.iar.com/
[lnk-ide-sw4stm32]:http://www.openstm32.org/System+Workbench+for+STM32
[lnk-teraterm]:https://ttssh2.osdn.jp
[lnk-android-st25]:https://play.google.com/store/apps/details?id=com.st.demo


[1]: ./media/nucleo1.png
[2]: ./media/nucleo2.png
[3]: ./media/nucleo3.png
[4]: ./media/nucleo4.png

