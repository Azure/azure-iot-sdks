How to certify IoT devices running Linux with Azure IoT SDK
===========================================================

--------------------------------------------------------------------------------

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Configure Azure IoT Hub](#Step_1:_Configure)
-   [Step 2: Register Device](#Step_2:_Register)
-   [Step 3: Build and Validate the sample using C client
    libraries](#Step_3:_Build)
    -   [3.1 Load the Azure IoT bits and prerequisites on
        device](#Step_3_1:_Load)
    -   [3.2 Build the samples](#Step_3_2:_Build)
    -   [3.3 Execute End to End Test Cases](#Step_3_3:_Run_E2E)
    -   [3.4 Run and Validate the Samples](#Step_3_4:_Run)
-   [Step 4: Package and Share](#Step_4:_Package_Share)
    -   [4.1 Package build logs and sample test results](#Step_4_1:_Package)
    -   [4.2 Share package with Engineering Support](#Step_4_2:_Share)
    -   [4.3 Next steps](#Step_4_3:_Next)
-   [Step 5: Troubleshooting](#Step_5:_Troubleshooting)

<a name="Introduction"/>

# Introduction

**About this document**

This document provides step-by-step guidance to IoT hardware publishers on how
to certify an IoT enabled hardware with Azure IoT SDK. This multi-step process
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
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    private repository.
-   SSH client, such as [PuTTY](http://www.putty.org/), so you can access the
    command line.
-   Required hardware to certify.

***Note:*** *If you haven’t contacted Microsoft about being an Azure Certified for IoT partner, please submit this [form](<https://iotcert.cloudapp.net/>) first to request it and then follow these instructions.*
    
<a name="Step_1:_Configure"/>

# Step 1: Sign Up To Azure IoT Hub

Follow the instructions [here](https://account.windowsazure.com/signup?offer=ms-azr-0044p) on how to sign up to the Azure IoT Hub service. As part of the sign up process, you will receive the connection string. 

-   **IoT Hub Connection String**: An example of IoT Hub Connection String is as below:

         HostName=[YourIoTHubName];CredentialType=SharedAccessSignature;CredentialScope=[ContosoIotHub];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

<a name="Step_2:_Register"/>

# Step 2: Register Device

In this section, you will register your device using Device Explorer. The Device
Explorer is a Windows application that interfaces with Azure IoT Hub and can
perform the following operations:

-   Device management
    -   Create new devices
    -   List existing devices and expose device properties stored on Device Hub
    -   Provides ability to update device keys
    -   Provides ability to delete a device
-   Monitoring events from your device
-   Sending messages to your device

To run Device Explorer tool, use following configuration string as described in
[Step1](#Step_1:_Configure):

-   IoT Hub Connection String
    

**Steps:**
1.  Click [here](<https://github.com/Azure/azure-iot-sdks/blob/develop/tools/DeviceExplorer/doc/how_to_use_device_explorer.md>) to download and install Device Explorer.

2.  Add connection information under the Configuration tab and click the **Update** button.

3.  Create and register the device with your IoT Hub using instructions as below.

    a. Click the **Management** tab.

    b. Click **List** button to call the device hub and retrieve a list
    of devices. If this is your first time, then you shouldn't
    retrieve anything.

    c. Click **Create** button to create a device ID and key.

    d. Once created successfully, device will be listed in device explorer.

    e. Right click the device and from context menu select "**Copy connection
    string for selected device**".

    f. Save this information in Notepad. You will need this information in
    later steps.

***Not running Windows on your PC?*** - Please send us an email on
<azureiotcertified@microsoft.com> and we will follow up with you with instructions.

<a name="Step_3:_Build"/>

# Step 3: Build and Validate the sample using C client libraries

This section walks you through building, deploying and validating the IoT Client
SDK on your device running a Linux operating system. You will install necessary
prerequisites on your device. Once done, you will build and deploy the IoT
Client SDK and validate the sample tests required for IoT certification with the
Azure IoT SDK.

<a name="Step_3_1:_Load"/>

## 3.1 Load the Azure IoT bits and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Install the prerequisite packages by issuing the following commands from the
    command line on the device. Choose your commands based on the OS running on
    your device.

    **Debian or Ubuntu**

        sudo apt-get update 

        sudo apt-get install -y curl libcurl4-openssl-dev uuid-dev uuid g++ make cmake git 

        sudo apt-get install -y uuid-dev uuid 

    **Fedora**

        sudo dnf check-update -y 

        sudo dnf install libcurl-devel openssl-devel libuuid-devel uuid-devel gcc-c++ make cmake git      

    **Any Other Linux OS**

        Use equivalent commands on the target OS

-   Download the SDK to the board by issuing the following command in PuTTY:

        git clone https://github.com/Azure/azure-iot-sdks.git

-   You will be prompted for your GitHub user name and password.

    ***Note:*** *If you have two-factor authentication enabled for your account,
    you'll need to generate/use a personal access token in place of
    your password. To generate token, navigate to
    <https://github.com/settings/tokens/new> and follow the on
    screen instructions.*

-   Verify that you now have a copy of the source code under the
    directory ~/azure-iot-suite-sdks.

<a name="Step_3_2:_Build"/>

## 3.2 Build the samples

-   There are two different samples available, one supporting AMQP protocol and
    other supporting HTTP protocol. You can use either of these protocols to
    validate a sample on your device. Based on your choice of protocol, run the
    following command on device.

    **For AMQP protocol:**

        nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTP protocol:**

        nano azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   This launches a console-based text editor. Scroll down to the
    connection information.

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string. You can get
    this from device explorer as explained in [Step 2](#Step_2:_Register), that
    you copied into Notepad.

-   Save your changes by pressing Ctrl+O and when nano prompts you to save it as
    the same file, just press ENTER.

-   Press Ctrl+X to exit nano.

-   Azure IoT Hub SDK depends on Apache Qpid Proton AMQP/HTTP to integrate with
    the IoT Hub. Run the following command to build/install Apache Proton.

        sudo ./azure-iot-sdks/c/build_all/linux/build_proton.sh --install /usr

-   Assuming everything went OK with the build\_proton.sh, you can now build the
    SDK code using the following command. If you are facing any issues during
    build, follow troubleshooting [Step 5](#Step_5:_Troubleshooting).

        chmod +x ./azure-iot-sdks/c/build_all/linux/build_paho.sh
        ./azure-iot-sdks/c/build_all/linux/build_paho.sh
        ./azure-iot-sdks/c/build_all/linux/build.sh | tee LogFile.txt

    ***Note:*** *LogFile.txt in above command should be replaced with a file
    name where build output will be written.*

<a name="Step_3_3:_Run_E2E"/>

## 3.3 Execute End to End Test Cases

In this section you will run the end to end test cases for Azure IoT client SDK to verify that you have build the SDK correctly.

### 3.3.1 Set the Environment Variables

-   Open **IOT_DEVICE_PARAMS.TXT** to edit.

        nano azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

-   Set the values for all the variables listed in the file as explained below.

    -   **IOTHUB_CONNECTION_STRING:** Connection string to your IoT Hub
    -   **IOTHUB_DEVICE_ID:** Id of the device you have registered in [Step 2](#Step_2:_Register)
    -   **IOTHUB_DEVICE_KEY:** Primary key of the device registered in [Step 2](#Step_2:_Register)
    -   **IOTHUB_EVENTHUB_ACCESS_KEY:** Shared access key of your IoT Hub
    -   **IOTHUB_EVENTHUB_CONNECTION_STRING:** Connection string to your Event Hub
    -   **IOTHUB_EVENTHUB_CONSUMER_GROUP:** Set value as **$Default**
    -   **IOTHUB_EVENTHUB_LISTEN_NAME:** Name of your Event Hub
    -   **IOTHUB_NAME:** Name of your IoT Hub
    -   **IOTHUB_SHARED_ACCESS_SIGNATURE:** this value can be generated from Device Explorer
    
        Go to **Configuration** tab &minus;&gt; Click **Generate SAS** button
    -   **IOTHUB_SUFFIX:** Suffix of your IoT Hub hostname in connection string
    -   **IOTHUB_PARTITION_COUNT:** Set value as **16**
    -   **IOTHUB_POLICY_NAME:** ToDo 
    -   **IOTHUB_POLICY_KEY:** ToDo
    

-   Set environment variables by running following command on your device:

        cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        chmod +x setiotdeviceparametersfore2etests.sh
        sudo setiotdeviceparametersfore2etests.sh 

### 3.3.2 Run End to End test cases

-   Run end to end test cases by executing following command on your device:

        ./azure-iot-sdks/c/iothub_client/build/linux/iothubclient_e2etests | tee E2ELogFile.txt

    ***Note:*** *E2ELogFile.txt in above command should be replaced with a file name where ene to end test results will be written.*


<a name="Step_3_4:_Run"/>
## 3.4 Run and Validate the Samples

In this section you will run the Azure IoT client SDK samples to validate
communication between your device and Azure IoT Hub. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. You will also monitor any messages send from the Azure IoT Hub to client.

**Note:** Take screenshots of all the operations you will perform in this
section. These will be needed in [Step 4](#Step_4_2:_Share)

### 3.4.1 Send Device Events to IOT Hub:

1.  Launch the Device Explorer as explained in [Step 2](#Step_2:_Register) and
    navigate to **Data** tab. Select the device name you created from the
    drop-down list of device IDs and click **Monitor** button.

    ![DeviceExplorer\_Monitor](images/3_3_1_01.png)

2.  Device Explorer is now monitoring data sent from the selected device to the
    IoT Hub.

3.  Run the sample by issuing following command.

    **If using AMQP protocol:** Run sample *iothub\_client\_sample\_amqp*

        ./azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/linux/iothub_client_sample_amqp

    **If using HTTP protocol:** Run sample *iothub\_client\_sample\_http*

        ./azure-iot-sdks/c/iothub\_client/samples/iothub_client_sample_http/linux/iothub_client_sample_http

4.  Verify that the confirmation messages show an OK. If not, then you may have
    incorrectly copied the device hub connection information.

    **If using AMQP protocol:**  
    ![SampleAMQP\_result\_terminal](images/3_3_1_02.png)

    **If using HTTP protocol:**  
    ![SampleHTTP\_result\_terminal](images/3_3_1_03.png)

5.  Device Explorer should show that IoT Hub has successfully received data sent
    by sample test.

    **If using AMQP protocol:**  
    ![SampleAMQP\_result\_DeviceExplorer](images/3_3_1_04.png)

    **If using HTTP protocol:**  
    ![SampleHTTP\_result\_DeviceExplorer](images/3_3_1_05.png)

### 3.4.2 Receive messages from IoT Hub

1.  To verify that you can send messages from the IoT Hub to your device,
    go to the **Notification** tab in Device Explorer.

2.  Select the device you created using Device ID drop down.

3.  Add some text to the Notification field, then click Send.

    ![NotificationSend\_DeviceExplorer](images/3_3_1_06.png)

4.  You should be able to see the command received in the console window for the
    client sample.

    **If using AMQP protocol:**  
    ![NotificationSend\_terminal](images/3_3_1_07.png)

    **If using HTTP protocol:**  
    ![NotificationSend\_terminal](images/3_3_1_08.png)

<a name="Step_4:_Package_Share"/>

# Step 4: Package and Share

<a name="Step_4_1:_Package"/>

## 4.1 Package build logs and sample test results

Package following artifacts from your device:

1.  Build logs and E2E test results that were logged in the log files during
    build run.

2.  All the screenshots that are shown above in "**Send Device Events to IoT Hub**" section.

3.  All the screenshots that are above in "**Receive messages from IoT Hub**" section.

4.  Send us clear instructions of how to run this sample with your hardware
    (explicitly highlighting the new steps for customers). As a guideline on how
    the instructions should look please refer the examples published on
    GitHub repository [here](<https://github.com/Azure/azure-iot-sdks/tree/master/c/doc>)

<a name="Step_4_2:_Share"/>

## 4.2 Share package with Microsoft Azure IoT team

Share the package in email to  <azureiotcertified@microsoft.com>.

<a name="Step_4_3:_Next"/>

## 4.3 Next steps

Once you shared the documents with us, we will contact you in the following 48
to 72 business hours with next steps.

<a name="Step_5:_Troubleshooting"/>

# Step 5: Troubleshooting

Please contact engineering support on <azureiotcertified@microsoft.com> for help with
troubleshooting.
