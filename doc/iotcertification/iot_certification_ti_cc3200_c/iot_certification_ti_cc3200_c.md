How to certify TI CC3200 device with Azure IoT SDK
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Sign Up To Azure IoT Hub](#Step_1)
-   [Step 2: Register Device](#Step_2)
-   [Step 3: Build and Validate the sample using C client libraries](#Step_3)
    -   [3.1 Install prerequisites](#Step_3_1)
    -   [3.2 Build Azure IoT SDK](#Step_3_2)
    -   [3.3 Run and Validate the Samples](#Step_3_3)
-   [Step 4: Package and Share](#Step_4)
    -   [4.1 Package build logs and sample test results](#Step_4_1)
    -   [4.2 Share with the Azure IoT Certification team](#Step_4_2)
    -   [4.3 Next steps](#Step_4_3)
-   [Step 5: Troubleshooting](#Step_5)

<a name="Introduction"></a>
# Introduction

**About this document**

This document provides step by step guidance to IoT hardware publishers on how to certify an IoT enabled hardware with Azure IoT C SDK. This multi-step process
includes: 
-   Configuring Azure IoT Hub 
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device
-   Packaging and sharing the logs

**Prepare**

Before executing any of the steps below, read through each process, step by step to ensure end to end understanding.

You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client, such as [PuTTY](http://www.putty.org/), so you can access the
    command line.
-   Required hardware [CC3200 Launchpad](http://www.ti.com/tool/cc3200-launchxl).

<a name="Step_1"></a>
# Step 1: Sign Up To Azure IoT Hub

Follow the instructions [here](https://account.windowsazure.com/signup?offer=ms-azr-0044p) on how to sign up to the Azure IoT Hub service.

As part of the sign up process, you will receive the connection string. 

-   **IoT Hub Connection String**: An example of IoT Hub Connection String is as below:

         HostName=[YourIoTHubName];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

<a name="Step_2"></a>
# Step 2: Register Device

In this section, you will register your device using DeviceExplorer. The DeviceExplorer is a Windows application that interfaces with Azure IoT Hub and can perform the following operations:

-   Device management
    -   Create new devices
    -   List existing devices and expose device properties stored on Device Hub
    -   Provides ability to update device keys
    -   Provides ability to delete a device
-   Monitoring events from your device
-   Sending messages to your device

To run DeviceExplorer tool, use following configuration string as described in [Step1](#Step_1):

-   IoT Hub Connection String
    

**Steps:**
1.  Click [here](<https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md>) to download and install DeviceExplorer.

2.  Add connection information under the Configuration tab and click the **Update** button.

3.  Create and register the device with your IoT Hub using instructions as below.

    a. Click the **Management** tab.

    b. Your registered devices will be visible in the list. In case your device is not there in the list, click **Refresh** button. If this is your first time, then you shouldn't retrieve anything.

    c. Click **Create** button to create a device ID and key.

    d. Once created successfully, device will be listed in DeviceExplorer.

    e. Right click the device and from context menu select "**Copy connection
    string for selected device**".

    f. Save this information in Notepad. You will need this information in
    later steps.

***Not running Windows on your PC?*** - Please send us an email on <iotcert@microsoft.com> and we will follow up with you with instructions.

<a name="Step_3"></a>
# Step 3: Build and Validate the sample using C client libraries

<a name="Step_3_1"></a>
## 3.1 Install prerequisites

While not strictly required, we recommend that you install the following tools from TI in the same directory and that you use directory names without any whitespace.

***Note:*** *This documentation assumes that you install everything in `C:\ti`.*

-   Install [Uniflash Tool](http://www.ti.com/tool/Uniflash) 3.2 or later for SimpleLink CC3200

-   Install [CC3200 SDK 1.1.0](http://www.ti.com/tool/cc3200sdk)

-   Install [TI-RTOS SDK for SimpleLink](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html) 2.14.01.20 or above

-   Install [NS package](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/ns/ns_1_10_00_00_eng.zip). Please note, the NS package is a pre-release version. It's content and the download location are subject to change.

-   Install [TI ARM Compiler](http://software-dl.ti.com/ccs/esd/test/ti_cgt_tms470_5.2.5_windows_installer.exe) 5.2.2 or above.

<a name="Step_3_2"></a>
## 3.2 Build Azure IoT SDK

1.  Edit file `products.mak` (*azure-iot-sdks\c\build_all\tirtos*). 
    Update the value of following variables:
    
    `XDCTOOLS_INSTALLATION_DIR`: install location of Uniflash Tool.
    `TIRTOS_INSTALLATION_DIR`: install location of TI-RTOS SDK.
    `CC3200SDK_INSTALLATION_DIR`: install location of CC3200 SDK.
    `NS_INSTALLATION_DIR`: install location of NS package.
    `ti.targets.arm.elf.M4`: install location of TI ARM compiler.

2. Open a Windows command prompt.

3. In the Windows command prompt, run the following commands (be sure to replace the paths with your installation paths).

  ```
  cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\build_all\tirtos
  C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
  C:\ti\xdctools_3_31_01_33_core\gmake.exe all
  ```

<a name="Step_3_3"></a>
## 3.3 Run and Validate the Samples

In this section you will run the Azure IoT client SDK samples to validate communication between your device and Azure IoT Hub. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. 

***Note:*** *Take screenshots of all the operations you will perform in this
section. These will be needed in [Step 4](#Step_4_2).*

<a name="Step_3_2_1"></a>
### 3.3.1 Build the sample simplesample_http application

1.  Open the `simplesample_http.c` file from the directory `azure-iot-sdks\c\serializer\samples\simplesample_http`.

2.  Find the following place holder for IoT connection string:
    
        static const char* connectionString = "[device connection string]";

3.  Replace the above placeholder with device connection string. You can get this from DeviceExplorer as explained in Step 2, that you copied into Notepad.

4.  Open the file `main.c` from the directory `azure-iot-sdks/c/serializer/samples/simplesample_http/tirtos/cc3200`. 

5.  Find the following comment for date-time:

        /* USER STEP: update to current date-time */

6.  Update the current date-time macros below the comment for current date time values.

    ```
    #define DAY     15
    #define MONTH   9
    #define YEAR    2015
    #define HOUR    6
    #define MINUTE  21
    #define SECOND  0
    ```

7. Open the file `wificonfig.h` from the directory `azure-iot-sdks/c/serializer/samples/simplesample_http/tirtos/cc3200`.

8.  Find the following comment for USER STEP:

        /* USER STEP: Update these macros */

9.  Update the wifi setting macros below the comment.

    ```
    #define SSID ""
    #define SECURITY_KEY ""
    ```

10. Download the [elf2cc32 executable for Windows](https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32.exe?raw=true) or [elf2cc32 executable for Linux](https://github.com/tisb-vikram/azure-iot-sdks/blob/7da24633b2c4af3bc779998e9950146f061a8a10/c/serializer/samples/simplesample_http/tirtos/cc3200/tools/elf2cc32?raw=true) to the folder `azure-iot-sdks\c\serializer\samples\simplesample_http\tirtos\cc3200\tools`. 
This tool is needed for converting the **simplesample_http.out** to **simplesample_http.bin** file.

11. Build the sample by executing following commands:

    ```
    cd <AZURE_INSTALL_DIR>\azure-iot-sdks\c\serializer\samples\simplesample_http\tirtos\cc3200
    C:\ti\xdctools_3_31_01_33_core\gmake.exe clean
    C:\ti\xdctools_3_31_01_33_core\gmake.exe all
    ```

<a name="Step_3_3_2"></a>
### Flash the sample simplesample_http and the root certificate

Flash the `simplesample_http.bin` file to your CC3200 Launchpad.

> Note: The root CA certificate - "Baltimore CyberTrust Root" has to be flashed to CC3200 Launchpad to the location `/cert/ms.der`. This location is referenced in `tirtos/cc3200/main.c` and is used by SimpleLink TLS stack.

The detailed information about the flash tool can be found in the [CC3200 UniFlash wiki](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash). The section [GUI Interface](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#GUI_Interface) walks through the steps for using the UniFlash tool.

Flash the application (.bin file) to the `/sys/mcuimg.bin` under System Files. For the certificate, [add a new file](http://processors.wiki.ti.com/index.php/CC31xx_%26_CC32xx_UniFlash#Adding_a_new_file_to_the_device) in the path `/cert/ms.der` and provide the path to the "Baltimore CyberTrust Root" certificate (.der format). The ms.der file is available at <AZURE_INSTALL_DIR>\azure-iot-sdks\c\certs\ms.der.

<a name="Step_3_3_3"></a>
### Running the sample simplesample_http
1.  After flashing the example, open a serial session to the appropriate COM port with the following settings:

    ```
    Baudrate:     9600
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
    ```

2.  Run the application by pressing the reset button on the CC3200 Launchpad. The application prints a message similar to the following:

    ```
    Starting the simplesample_http example
    CC3200 has connected to AP and acquired an IP address.
    IP Address: XXX.XXX.XX.XXX
    IoTHubClient accepted the message for delivery
    Message Id: 1 Received.
    Result Call Back Called! Result is: IOTHUB_CLIENT_CONFIRMATION_OK
    ```

3.  The DeviceExplorer can be used to monitor the data sent by the application. Under the **Data** tab in DeviceExplorer, **Monitor** option should be selected before running the application. Later when the application is run, a message similar to the following message is displayed on "Event Hub Data" window.

```
9/17/2015 7:28:28 PM> Data:[{"Temperature":67, "Humidity":42}]
```

<a name="Step_4"></a>
# Step 4: Package and Share

<a name="Step_4_1"></a>
## 4.1 Package build logs and sample test results

Package the following artifacts from your device:

1.  Build logs and test results from your platform.

2.  All the screenshots that belong to "**Running the sample simplesample_http**" section.

3.  Create a document that explains how to run the sample with your hardware (explicitly highlighting the new steps for customers). As a guideline on how the instructions should look please refer the examples published on GitHub repository [here](<https://github.com/Azure/azure-iot-sdks/tree/master/c/doc>).

<a name="Step_4_2"></a>
## 4.2 Share with the Azure IoT Certification team

Send the package in email to  <iotcert@microsoft.com>.

<a name="Step_4_3"></a>
## 4.3 Next steps

Once you shared the documents with us, we will contact you in the following 48 to 72 business hours with next steps.

<a name="Step_5"></a>
# Step 5: Troubleshooting

Please contact engineering support on <iotcert@microsoft.com> for help with troubleshooting.