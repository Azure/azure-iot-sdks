---
platform: {enter the OS name running on device}
device: {enter your device name here}
language: java
---

Run a simple JAVA sample on {enter your device name here} device running {enter the OS name running on device}
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

# Instructions for using this template

-   Replace the text in {placeholders} with correct values.
-   Delete the lines {{enclosed}} after following the instructions enclosed between them.
-   It is advisable to use external links, wherever possible.
-   Remove this section from final document.

<a name="Introduction"/>
# Introduction

**About this document**

This document describes how to connect {enter your device name here} device running {enter the OS name running on device} with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][setup-devbox-linux]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   {enter your device name here} device.
-   {{Please specify if any other software(s) or hardware(s) are required.}}

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   {{Write down the instructions required to setup, configure and connect your device. Please use external links when possible pointing to your own page with device preparation steps.}}

<a name="Build"></a>
# Step 3: Build SDK and Run the sample

<a name="Step_3_1"/>
## 3.1 Install Azure IoT Device SDK and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages by issuing the following commands from the command line on the device.

<a name="Step_3_1_1"/>
### 3.1.1  Install Java JDK and set up environment variables
        
1.  {{Keep the command set based on your OS and remove the rest.}}
        
    {{**Debian**}}

        sudo apt-get update        
        sudo apt-get install openjdk-8-jdk      
   
    ***Note:*** *If openjdk-8-jdk package is not available, use following steps to add source in sources.list and rerun above commands again.*
    
    -   Edit /etc/apt/sources.list
    
    -   Add below line and save the changes.
        
        `deb http://ftp.debian.org/debian testing main`
   
    {{**Ubuntu**}}

        sudo apt-get update        
        sudo apt-get install openjdk-8-jdk 
   
    {{**Fedora**}}
   
        sudo dnf check-update -y
        sudo dnf install java-1.8.0-openjdk-devel
        
    {{**Any Other Linux OS**}}

        Use equivalent commands on the target OS
       
2.  Update the PATH environment variable to include the full path to the bin folder containing Java. To ensure the correct path of Java run below command:     
       
        which java
        
3.  Ensure that the directory shown by the `which java` command matches one of the directories shown in your $PATH variable. You can confirm this by running following command.

        echo $PATH

4.  If Java path is missing in PATH environment variable, run following command to set the same.    

        export PATH=[PathToJava]/bin:$PATH       

    ***NOTE:*** *Here **[PathToJava]** is output of `which java` command. For example, if `which java` output is /usr/bin/java, then export command will be* **export PATH=/usr/bin/java/bin:$PATH**

5.  Make sure that the JAVA_HOME environment variable includes the full path to the JDK. Use below command to get the JDK path.

        update-alternatives --config java

6.  Take note of the JDK location. `update-alternatives` output will show something similar to **/usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java**. The JDK directory would then be **/usr/lib/jvm/java-8-openjdk-amd64/**.

7.  Run the following command to set **JAVA_HOME** environment variable.

        export JAVA_HOME=[PathToJDK]

    ***Note***: *Here [PathToJDK] is JDK directory. For example if jdk directory is /usr/lib/jvm/java-8-openjdk-amd64/, export command will be* **export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/**

<a name="Step_3_1_2"/>
### 3.1.2  Install Maven and set up environment variables

1.  {{Keep the command set based on your OS and remove the rest.}}

    {{**Debian or Ubuntu**}}

        sudo apt-get install maven

    {{**Fedora**}}

        sudo dnf install maven
   
    {{**Any Other Linux OS**}}

        Use equivalent commands on the target OS

2.  Update the PATH environment variable to include the full path to the bin folder containing maven. To ensure the correct path of maven, run below command:     
       
        which mvn
         
3.  Ensure that the directory shown by the `which mvn` command matches one of the directories shown in your $PATH variable. You can confirm this by running following command.
 
        echo $PATH

4.  If maven path is missing in PATH environment variable, run following command to set the same.     

        export PATH=[PathToMvn]/bin:$PATH

    ***Note***: *Here [PathToMvn] is output of `which mvn`. For example if `which mvn` output is /usr/bin/mvn, export command will be* **export PATH=/usr/bin/mvn/bin:$PATH**
   
5.  You can verify that the environment variables necessary to run Maven 3 have been set correctly by running `mvn --version`.

<a name="Step_3_1_3"/>
### 3.1.3  Install GIT

1.  {{Keep the command set based on your OS and remove the rest.}}

    {{**Debian or Ubuntu**}}

        sudo apt-get install git

    {{**Fedora**}}

        sudo dnf install git   

    {{**Any Other Linux OS**}}

        Use equivalent commands on the target OS

<a name="Step_3_1_4"/>
### 3.1.4 Build the Azure IoT Device SDK for Java

1.  Download the SDK to the board by issuing the following command in PuTTY:

        git clone https://github.com/Azure/azure-iot-sdks.git

2.  Verify that you now have a copy of the source code under the directory **azure-iot-sdks**.

3.  Run the following commands on device in sequence to build Azure IoT SDK.

        cd azure-iot-sdks/java/device
        mvn install | tee JavaSDK_Build_Logs.txt

4.  Above command will generate the compiled JAR files with all dependencies. This bundle can be found at:

        azure-iot-sdks/java/device/iothub-java-client/target/iothub-java-client-{version}-with-deps.jar

<a name="Step_3_2"/>
## 3.2 Run and Validate the Samples

<a name="Step_3_2_1"/>
### 3.2.1 Send Device Events to IoT Hub:

-   Navigate to the folder containing the executable JAR file for send event sample.

        cd azure-iot-sdks/java/device/samples/send-event/target

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

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

<a name="Step_3_2_2"/>
### 3.2.2 Receive messages from IoT Hub

-   Navigate to the folder containing the executable JAR file for the receive message sample.

        cd azure-iot-sdks/java/device/samples/handle-messages/target
     
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

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.


[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/get_started/java-devbox-setup.md
[lnk-setup-iot-hub]: ../../setup_iothub.md
[lnk-manage-iot-hub]: ../../manage_iot_hub.md
