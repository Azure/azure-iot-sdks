---
platform: windows 7 professional
device: idm-iwp-hw-2
language: java
---

Run a simple JAVA sample on IDM-IWP-HW-2 device running Windows 7 Professional
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)


<a name="Introduction"/>
# Introduction

**About this document**

This document describes how to connect IDM-IWP-HW-2 device running Windows 7 Professional with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-windows]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   IDM-IWP-HW-2 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   You can setup your device as a common PC.
-   Just connect a Monitor, Mouse and Keyboard.
-   And then turn on the device and configure the network.

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

<a name="Step_3_1"/>
## 3.1 Install Azure IoT Device SDK and prerequisites on device

-   To run the SDK you will need Java SE 1.8.

-   Install the prerequisite packages using command line on the device.

<a name="Step_3_1_1"/>
### 3.1.1  Install Java JDK 1.8 and set up environment variables
        
1.  For downloads and installation instructions go here: <http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html>
       
2.  Please make sure that the `PATH` environment variable includes the full path to the jdk1.8.x\bin directory. (Example: c:\Program Files\Java\jdk1.8.0_65)
        
3.  Please make sure that the `JAVA_HOME` environment variable includes the full path to the jdk1.8.x directory. (Example: JAVA_HOME=c:\Program Files\Java\jdk1.8.0_65)

4.  You can test whether your PATH variable is set correctly by restarting your console and running `java -version`.

<a name="Step_3_1_2"/>
### 3.1.2  Install Maven and set up environment variables
Using Maven 3 is the recommended way to install Azure IoT device SDK for Java.

1.  For downloads and installation instructions for maven 3 go here: <https://maven.apache.org/download.cgi>

2.  Please make sure that the PATH environment variable includes the full path to the apache-maven-3.x.x\bin directory. (Example: F:\Setups\apache-maven-3.3.3\bin). The apache-maven-3.x.x directory is where Maven 3 is installed.

2.  You can verify that the environment variables necessary to run Maven 3 have been set correctly by restarting your console and running `mvn --version.`
  
<a name="Step_3_1_3"/>
### 3.1.3  Install GIT

-   For downloads and installation instructions go here:
<http://git-scm.com/book/en/v2/Getting-Started-Installing-Git>


<a name="Step_3_1_4"/>
### 3.1.4 Build the Azure IoT Device SDK for Java

1.  Download the SDK to the board by issuing the following command:

        git clone https://github.com/Azure/azure-iot-sdks.git

2.  Verify that you now have a copy of the source code under the directory **azure-iot-sdks**.

3.  Run the following commands on device in sequence to build Azure IoT SDK.

        cd azure-iot-sdks/java/device
        mvn install

4.  Above command will generate the compiled JAR files with all dependencies. This bundle can be found at:

        azure-iot-sdks/java/device/iothub-java-client/target/iothub-java-client-{version}-with-deps.jar

<a name="Step_3_2"/>
## 3.2 Run and Validate the Samples

<a name="Step_3_2_1"/>
### 3.2.1 Send Device Events to IoT Hub:

-   Navigate to the folder containing the executable JAR file for send event sample.

        cd /azure-iot-sdks/java/device/samples/send-event/target

-   Run the sample by issuing following command.
{{Keep the command set based on your protocol(s) and remove the rest.}}

    {{**If using AMQPS protocol:**}}

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "amqps"
    
    {{**If using HTTPS protocol:**}}

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "https"

    {{**If using MQTT protocol:**}}

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "mqtt"
        
    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

<a name="Step_3_2_2"/>
### 3.2.2 Receive messages from IoT Hub

-   Navigate to the folder containing the executable JAR file for the receive message sample.

        cd /azure-iot-sdks/java/device/samples/handle-messages/target
     
-   Run the sample by issuing following command.

    {{**If using AMQPS protocol:**}}
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "amqps"
    
    {{**If using HTTPS protocol:**}}
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "https"
        
     {{**If using MQTT protocol:**}}
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "mqtt"

    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

[setup-devbox-windows]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/java-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
