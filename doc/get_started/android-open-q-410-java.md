---
platform: android
device: open-q 410
language: java
---

Run a simple JAVA sample on Intrinsyc’s Open-Q 410 development board running Android
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Open-Q 410 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Prepare your development environment:

    -   Download and install latest JDK from [here](<http://www.oracle.com/technetwork/java/javase/downloads/index.html>).
    -   Set environment variable JAVA_HOME with path to JDK.
    -   Download [Android Studio](<https://developer.android.com/studio/index.html>) on your Windows machine and follow the installation instructions.
    -   Install android sdk 22 5.1.1 Lollipop.
    -   Computer with Git client installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
    -   Download Maven from [here](<https://maven.apache.org/download.cgi>).
    -   Install Maven by following instructions available [here](<https://maven.apache.org/install.html>).
    -   Add path to Maven bin folder to environment variable PATH.
-   [Setup your IoT hub][lnk-setup-iot-hub]

-   [Provision your device and get its credentials][lnk-manage-iot-hub]


<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

###Enable USB debugging on your device 
 
-   Make sure usb cable connected to correct micro USB socket and nothing connected to  USB host.
-   Settings > About Phone > Build number > Tap it 7 times to become developer
-   Settings > Developer Options > USB Debugging
-   Connect the  board (correct USB 2.0 device micro‐ B) to Windows 10 development machine with USB cable 
 
###Turn on Wifi and configure the board to connect to internet 
 
-   Settings -> WiFi 
 
###If LCD Display is not provided  
 
-   There is an option to connect HDMI display and follow the board instructions to switch between adb and mouse/keyboard (they cannot be used simultaneously)

<a name="Build"></a>
# Step 3: Build and Run the sample

Please find Azure Android java device sample code [here][android-sample-code].

<a name="Step_3_1"></a>
## 3.1  Modify and Build the Samples

-   Start Android Studio and open Android project from here:
        `azure-iotsdks/java/device/samples/android-sample/`
-   Replace the [device connection string] placeholder in MainActivity.java with your device connection string.
-   Uncomment necessary protocol HTTP or MQTT.
-   Change targetSdkVersion in app/build.gradle to 22 (Android 5.1 Lollypop).
-   Build project: Build -> Make Project 


<a name="Step_3_2"></a>
## 3.2 Build the Sample

1.  Start a new instance of Android Studio and open Android project from here:

        azure-iot-sdks/java/device/samples/android-sample/

2.  Go to **MainActivity.java**, replace the **[device connection string]** placeholder with connection string of the device you have created in [Provision your device and get its credentials][lnk-manage-iot-hub] and save the file.  An example of IoT Hub Connection String is as below:

         HostName=[YourIoTHubName];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

3. Build your project by going to **Build** menu **> Make Project**.

<a name="Step_3_3"></a>
## 3.3 Run and Validate the Samples

In this section you will run the Azure IoT client SDK samples to validate
communication between your device and Azure IoT Hub. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. You will also monitor any messages sent from the Azure IoT Hub to client.

<a name="Step_3_3_1"></a>
### 3.3.1 Run the Sample:

-   Start Device Explorer, go to Data tab and click on  Monitor.
-   If you get Messaging Exception Error, make sure your firewall settings allow the app to connect to IoT Hub.
-   Make sure device is connected to Windows 10 machine and adb enabled.
-   Select one of your project's files and click Run  from the toolbar.
-   In the Choose Device window that appears, select the **Choose a running device** radio button, select your device, and click OK.
-   Android Studio will install the app on your connected device and starts it.

<a name="Step_3_3_2"></a>
### 3.3.2 Send Device Events to IoT Hub:

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.
-   As soon as you run the app on your device (or emulator), it will start sending messages to IoTHub.
-   Check the **Android Monitor** window  in Android Studio. Verify that the confirmation messages show an OK. If not, then you may have incorrectly copied the device hub connection information.

<a name="Step_3_3_3"></a>
### 3.3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.
-   Click the **Receive Messages** button from the sample App UI loaded on your device or in the emulator. If you modify the application code to receive message right after sending message, you could skip this step.
-   Check the **Android Monitor** window in Android Studio. You should be able to see the command received.


[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[android-sample-code]: https://github.com/Azure/azure-iot-sdks/tree/master/java/device/samples/android-sample
[mainactivity-source-code]: https://github.com/Azure/azure-iot-sdks/blob/master/java/device/samples/android-sample/app/src/main/java/com/iothub/azure/microsoft/com/androidsample/MainActivity.java
