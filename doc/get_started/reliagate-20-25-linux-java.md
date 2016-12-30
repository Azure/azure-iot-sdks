---
platform: linux yocto
device: eurotech reliagate 20-25
language: java
---

# Table of Contents

-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)


**About this document**

This document describes how to connect EUROTECH ReliaGATE 20-25 device running Linux Yocto 1.8 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK and Samples on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [An EUROTECH ReliaGATE 20-25 IoT Development Kit device][lnk-eth-reliagate-20-25]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Unbox your Eurotech ReliaGATE 20-25 IoT Development Kit
-   Connect your Eurotech ReliaGATE 20-25 IoT Gateway to your LAN router through the ETH1 Ethernet port
-   Supply power to your IoT Gateway and let it turn on
-   Wait for the IoT Gateway to boot - this process can take up to minute or two
-   From a host PC running Bonjour or Avahi services, SSH connect to Eurotech ReliaGATE 20-25 IoT Gateway with the following command:

    % ssh root@reliagate-20-25_<serial_number>.local

-   From a host PC not running Bonjour or Avahi services, connect to the Eurotech ReliaGATE 20-25 through its serial debug port.
-   Connect the serial console cable provided in the Dev Kit to the serial debug port accessible in gateway service panel
-   Use the following connection parameters to open a serial console: baud rate: 115200, 8N1
-   Login in into the gateway with the root user account
-   Discover the IP address of the gateway ETH1 interface through the following command:

    % ifconfig

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

<a name="Step_3_1"></a>
## 3.1 Prepare Azure IoT Device SDK and Samples on Host system

-   On a Host system, Windows, Mac OS X or Linux, open a command line terminal
-   Install the prerequisite packages by issuing the following commands from the command line.

<a name="Step_3_1_1"></a>
### 3.1.1  Install Java JDK and set up environment variables

Make sure a Java 8 Virtual Machine is installed in your host OS and it is included in your path.

If Java is not available, use the appropriate instructions to install Java on your host operating system and add it into your system PATH.

To verify Java is properly installed, run the following from your command line:

    % java -version
    java version "1.8.0_66"
    Java(TM) SE Runtime Environment (build 1.8.0_66-b17)
    Java HotSpot(TM) 64-Bit Server VM (build 25.66-b17, mixed mode)

<a name="Step_3_1_2"></a>
### 3.1.2  Install Maven and set up environment variables

Make sure a Maven 3 is installed in your host OS and it is included in your path.

If Maven 3 is not available, use the appropriate instructions to install Maven 3 on your host operating system and add it into your system PATH.

To verify Maven 3 is properly installed, run the following command from your command line:

    % mvn -version
    Apache Maven 3.3.9 (bb52d8502b132ec0a5a3f4c09453c07478323dc5; 2015-11-10T17:41:47+01:00)
    Maven home: /Packages/apache-maven-3.3.9
    Java version: 1.8.0_66, vendor: Oracle Corporation
    Java home: /Library/Java/JavaVirtualMachines/jdk1.8.0_66.jdk/Contents/Home/jre
    Default locale: en_US, platform encoding: UTF-8
    OS name: "mac os x", version: "10.12.1", arch: "x86_64", family: "mac"

<a name="Step_3_1_3"></a>
### 3.1.3  Install GIT

Make sure a git is installed in your host OS and it is included in your path.

If git is not available, use the appropriate instructions to install git on your host operating system and add it into your system PATH.

To verify git is properly installed, run the following command from your command line:

    % git --version
    git version 2.3.2 (Apple Git-55)

<a name="Step_3_1_4"></a>
### 3.1.4 Build the Azure IoT Device SDK for Java

1.  Download the SDK to the board by issuing the following command:

        % git clone https://github.com/azure/azure-iot-sdk-java

2.  Verify that you now have a copy of the source code under the directory **azure-iot-sdk-java**.

3.  Run the following commands in sequence to build Azure IoT SDK.

        % cd azure-iot-sdk-java/
        % cd service/
        % mvn install | tee JavaSDK_Service_Build_Logs.txt
        % cd ../device/
        % mvn install | tee JavaSDK_Device_Build_Logs.txt

4.  Above command will generate the compiled JAR files with all dependencies. This bundle can be found at:

        azure-iot-sdk-java/device/iothub-java-client/target/iothub-java-client-1.0.16-with-deps.jar

<a name="Step_3_1_5"></a>
### 3.1.5 Install Azure IoT Device SDK on the EUROTECH ReliaGATE 20-25

All EUROTECH IoT Gateways come with a Java 8 Virtual Machine pre-installed so they are ready to run the Azure IoT Device SDK out of the box with no addition software installation.

Transfer the Azure IoT Device SDK and Samples to the target EUROTECH ReliaGATE 20-25.

Run the following commands from your host system to copy the Azure IoT Device SDK and Samples jars to the target device under the home directory of the root account.

    % cd azure-iot-sdk-java/
    % scp device/iothub-java-client/target/iothub-java-client-1.0.16-with-deps.jar root@reliagate-20-25_<serial_number>.local:~
    % scp device/samples/send-event/target/send-event-1.0.16-with-deps.jar root@reliagate-20-25_<serial_number>.local:~
    % scp device/samples/handle-messages/target/handle-messages-1.0.16-with-deps.jar  root@reliagate-20-25_<serial_number>.local:~

<a name="Step_3_2"></a>
## 3.2 Run and Validate the Samples

Open a SSH terminal on the EUROTECH ReliaGATE 20-25. From your host system use Open Putty or other terminals to open the SSH session.

    % ssh root@reliagate-20-25_<serial_number>.local

<a name="Step_3_2_1"></a>
### 3.2.1 Send Device Events to IoT Hub:

-   Navigate to the folder containing the executable JAR file for send event sample.

        reliagate-20-25% cd ~

-   Run the sample by issuing following command.

    **If using AMQPS protocol:**

        reliagate-20-25% java -jar send-event-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "10" "amqps"

    **If using HTTPS protocol:**

        reliagate-20-25% java -jar send-event-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "10" "https"

    **If using MQTT protocol:**

        reliagate-20-25% java -jar send-event-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "10" "mqtt"


<a name="Step_3_2_2"></a>
### 3.2.2 Receive messages from IoT Hub

-   Navigate to the folder containing the executable JAR file for the receive message sample.

        reliagate-20-25% cd ~

-   Run the sample by issuing following command.

    **If using AMQPS protocol:**

        reliagate-20-25% java -jar handle-messages-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "amqps"

    **If using HTTPS protocol:**

        reliagate-20-25% java -jar handle-messages-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "https"

    **If using MQTT protocol:**

        reliagate-20-25% java -jar handle-messages-1.0.16-with-deps.jar "HostName=eth.azure-devices.net;DeviceId=eth-reliagate-20-25;SharedAccessKey=FKuEjy7DHsEJeRH23PD0+ahtkyf+6DUZBQW1vXUBlkQ=" "mqtt"



[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/java-devbox-setup.md
[lnk-setup-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[lnk-manage-iot-hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[lnk-eth-reliagate-20-25]: https://www.eurotech.com/en/products/iot/IoT+Development+Kits
