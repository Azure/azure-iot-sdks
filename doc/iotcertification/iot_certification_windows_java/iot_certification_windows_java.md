How to certify IoT devices running Windows with Azure IoT SDK
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Sign Up To Azure IoT Hub](#Step_1)
-   [Step 2: Register Device](#Step_2)
-   [Step 3: Build and Validate the sample using Java client libraries](#Step_3)
    -   [3.1 Install Azure IoT Device SDK and prerequisites on device](#Step_3_1)
    -   [3.2 Run and Validate the Samples](#Step_3_2)
-   [Step 4: Package and Share](#Step_4)
    -   [4.1 Package build logs and sample test results](#Step_4_1)
    -   [4.2 Share with the Azure IoT Certification team](#Step_4_2)
    -   [4.3 Next steps](#Step_4_3)
-   [Step 5: Troubleshooting](#Step_5)

<a name="Introduction"/>
# Introduction

**About this document**

This document provides step by step guidance to IoT hardware publishers on how
to certify an IoT enabled hardware with Azure IoT Java SDK. This multi-step process
includes: 
-   Configuring Azure IoT Hub 
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device
-   Packaging and sharing the logs

**Prepare**

Before executing any of the steps below, read through each process, step by step
to ensure end to end understanding.

You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client, such as [PuTTY](http://www.putty.org/), so you can access the
    command line.
-   Required hardware to certify.

<a name="Step_1"/>
# Step 1: Sign Up To Azure IoT Hub

Follow the instructions [here](https://account.windowsazure.com/signup?offer=ms-azr-0044p) on how to sign up to the Azure IoT Hub service.

As part of the sign up process, you will receive the connection string. 

-   **IoT Hub Connection String**: An example of IoT Hub Connection String is as below:

         HostName=[YourIoTHubName];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

<a name="Step_2"/>
# Step 2: Register Device

In this section, you will register your device using DeviceExplorer. The DeviceExplorer is a Windows application that interfaces with Azure IoT Hub and can perform the following operations:

-   Device management
    -   Create new devices
    -   List existing devices and expose device properties stored on Device Hub
    -   Provides ability to update device keys
    -   Provides ability to delete a device
-   Monitoring events from your device
-   Sending messages to your device

To run DeviceExplorer tool, use following configuration string as described in
[Step1](#Step_1):

-   IoT Hub Connection String
    

**Steps:**
1.  Click [here](<https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md>) to download and install DeviceExplorer.

2.  Add connection information under the Configuration tab and click the **Update** button.

3.  Create and register the device with your IoT Hub using instructions as below.

    a. Click the **Management** tab.

    b. Your registered devices will be displayed in the list. In case your device is not there in the list, click **Refresh** button. If this is your first time, then you shouldn't retrieve anything.

    c. Click **Create** button to create a device ID and key.

    d. Once created successfully, device will be listed in DeviceExplorer.

    e. Right click the device and from context menu select "**Copy connection
    string for selected device**".

    f. Save this information in Notepad. You will need this information in
    later steps.

***Not running Windows on your PC?*** - Please send us an email on
<iotcert@microsoft.com> and we will follow up with you with instructions.

<a name="Step_3"/>
# Step 3: Build and Validate the sample using Java client libraries

This section walks you through building, deploying and validating the IoT Client SDK on your device running a Windows operating system. You will install the necessary prerequisites on your device. Once done, you will build and deploy the IoT Client SDK, and validate the sample tests required for IoT certification with the Azure IoT SDK.

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
### 3.1.4  Build Qpid JMS

1.  Clone the repository for Qpid JMS to your local directory.
    
        git clone https://github.com/avranju/qpid-jms.git

2.  Install Qpid JMS by executing following commands in sequence:

        cd qpid-jms
        mvn install
        cd ..

    ***Note:*** *We have noticed that certain unit tests can fail when running  `mvn install` as given above with the latest version of JDK 8 (1.8.0_65 at the time this document was written). It works fine with older versions however. If this occurs please skip running unit tests using following command:*
    
        mvn install -DskipTests

<a name="Step_3_1_5"/>
### 3.1.5 Build the Azure IoT Device SDK for Java

1.  Download the SDK to the board by issuing the following command in PuTTY:

        git clone https://github.com/Azure/azure-iot-sdks.git

2.  Verify that you now have a copy of the source code under the directory **azure-iot-sdks**.

3.  Run the following commands on device in sequence to build Azure IoT SDK.

        cd azure-iot-sdks/java/device
        mvn install

4.  Above command will generate the compiled JAR files with all dependencies. This bundle can be found at:

        azure-iot-sdks/java/device/iothub-java-client/target/iothub-java-client-{version}-with-deps.jar

<a name="Step_3_2"/>
## 3.2 Run and Validate the Samples

In this section you will run the Azure IoT client SDK samples to validate
communication between your device and Azure IoT Hub. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. You will also monitor any messages sent from the Azure IoT Hub to client.

***Note:*** *Take screenshots of all the operations you will perform in this
section. These will be needed in [Step 4](#Step_4_2).*

<a name="Step_3_2_1"/>
### 3.2.1 Send Device Events to IoT Hub:

1.  Launch the DeviceExplorer as explained in [Step 2](#Step_2) and navigate to **Data** tab. Select the device name you created from the drop-down list of device IDs and click **Monitor** button.

    ![DeviceExplorer\_Monitor](images/3_3_1_01.png)

2.  DeviceExplorer is now monitoring data sent from the selected device to the IoT Hub.

3.  Navigate to the folder containing the executable JAR file for send event sample.

        cd /azure-iot-sdks/java/device/samples/send-event/target

4.  Run the sample by issuing following command.

    **If using AMQP protocol:**

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "amqps"
    
    **If using HTTP protocol:**

        java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "https"

    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

5.  Verify that the confirmation messages show an OK. If not, then you may have incorrectly copied the device hub connection information.

    **If using AMQP protocol:**  
    ![Terminal\_AMQP\_send\_event](images/terminal_amqp_send_event.png)

    **If using HTTP protocol:**  
    ![Terminal\_HTTP\_send\_event](images/terminal_http_send_event.png)

6.  DeviceExplorer should show that IoT Hub has successfully received data sent by sample test.

    **If using AMQP protocol:**  
    ![DeviceExplorer\_AMQP\_message\_received](images/device_explorer_amqp_message_received.png)

    **If using HTTP protocol:**  
    ![DeviceExplorer\_HTTP\_message\_received](images/device_explorer_http_message_received.png)

<a name="Step_3_2_2"/>
### 3.2.2 Receive messages from IoT Hub

1.  To verify that you can send messages from the IoT Hub to your device, go to the **Messages To Device** tab in DeviceExplorer.

2.  Select the device you created using Device ID drop down.

3.  Add some text to the Message field, then click Send.

    ![DeviceExplorer\_message\_send](images/device_explorer_message_send.png)

4.  Navigate to the folder containing the executable JAR file for the receive message sample.

        cd /azure-iot-sdks/java/device/samples/handle-messages/target
     
5.  Run the sample by issuing following command.

    **If using AMQP protocol:**
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "amqps"
    
    **If using HTTP protocol:**
   
        java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "https"

    Replace the following in above command:
    
    -   `{version}`: Version of binaries you have build
    -   `{connection string}`: Your device connection string
    -   `{number of requests to send}`: Number of messages you want to send to IoT Hub

6.  You should be able to see the command received in the console window for the client sample.

    **If using AMQP protocol:**  
    ![Terminal\_AMQP\_message\_received](images/terminal_amqp_message_received.png)

    **If using HTTP protocol:**  
    ![Terminal\_HTTP\_message\_received](images/terminal_http_message_received.png)

<a name="Step_4"/>
# Step 4: Package and Share

<a name="Step_4_1"/>
## 4.1 Package build logs and sample test results

Package the following artifacts from your device:

1.  Build logs and test results that were logged in the log files during steps 3.1.4 and 3.1.5.

2.  All the screenshots that are shown above in "**Send Device Events to IoT Hub**" section.

3.  All the screenshots that are above in "**Receive messages from IoT Hub**" section.

4.  Send us clear instructions of how to run this sample with your hardware (explicitly highlighting the new steps for customers). As a guideline on how the instructions should look please refer the examples published on GitHub repository at <https://github.com/Azure/azure-iot-sdks/tree/master/java/device/doc>.

<a name="Step_4_2"/>
## 4.2 Share with the Azure IoT Certification team

Send the package in email to  <iotcert@microsoft.com>.

<a name="Step_4_3"/>
## 4.3 Next steps

Once you shared the documents with us, we will contact you in the following 48 to 72 business hours with next steps.

<a name="Step_5"/>
# Step 5: Troubleshooting

Please contact engineering support on <iotcert@microsoft.com> for help with troubleshooting.