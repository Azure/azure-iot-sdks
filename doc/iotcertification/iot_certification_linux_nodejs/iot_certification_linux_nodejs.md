How to certify IoT devices running Linux with Azure IoT SDK 
===
---


# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Configure Azure IoT Hub](#Step_1:_Configure)
-   [Step 2: Register Device](#Step_2:_Register)
-   [Step 3: Build and validate the sample using Node JS client libraries](#Step_3:_Build)
    -   [3.1 Load the Azure IoT bits and prerequisites on device](#Step_3_1:_Load)
    -   [3.2 Build the samples](#Step_3_2:_Build)
    -   [3.3 Run and Validate the Samples](#Step_3_3:_Run)
-   [Step 4: Package and Share](#Step_4:_Package_Share)
    -   [4.1 Package build logs and sample test results](#Step_4_1:_Package)
    -   [4.2 Share package with Engineering Support](#Step_4_2:_Share)
    -   [4.3 Next steps](#Step_4_3:_Next)
-   [Step 5: Troubleshooting](#Step_5:_Troubleshooting)

<a name="Introduction"/>
# Introduction 

**About this document**

This document provides step-by-step guidance to IoT hardware publishers on how to certify an IoT enabled hardware with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub 
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device
-   Packaging and sharing the logs   
    

**Prepare**

Before executing any of the steps below, read through each process, step by step to ensure end to end understanding. You should have the following items ready before beginning the process:

-   Computer with GitHub installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub private repository
-   SSH client, such as [PuTTY](http://www.putty.org/), so you can access the command line
-   Required hardware to certify

***Note:*** *If you haven’t contacted Microsoft about being an Azure Certified for IoT partner, please submit this [form](<https://iotcert.cloudapp.net/>) first to request it and then follow these instructions.*

<a name="Step_1:_Configure"/>
# Step 1: Sign Up To Azure IoT Hub

Follow the instructions [here](https://account.windowsazure.com/signup?offer=ms-azr-0044p) on how to sign up to the Azure IoT Hub service.

As part of the sign up process, you will receive the connection string. 

-   **IoT Hub Connection String**: An example of IoT Hub Connection String is as below:

          HostName=[YourIoTHubName];CredentialType=SharedAccessSignature;CredentialScope=[ContosoIotHub];SharedAccessKeyName=[YourAccessKeyName];SharedAccessKey=[YourAccessKey]

<a name="Step_2:_Register"/>
# Step 2: Register Device

In this section, you will register your device using Device Explorer. The Device Explorer is a Windows application that interfaces with Azure IoT Hub and can perform the following operations:

-   Device management
    -   Create new devices
    -   List existing devices and expose device properties stored on Device Hub
    -   Provides ability to update device keys
    -   Provides ability to delete a device
-   Monitoring events from your device.
-   Sending messages to your device.

To run Device Explorer tool, follow the configuration strings as described in  [Step1](#Step_1:_Configure):

-   IoT Hub Connection String

**Steps:**
1.   Click [here](<https://github.com/Azure/azure-iot-sdks/blob/develop/tools/DeviceExplorer/doc/how_to_use_device_explorer.md>) to download and install Device Explorer.

2.  Add connection information under the Configuration tab and click the **Update** button.

3.  Create and register the device with your IoT Hub using instructions as below.

    a. Click the **Management** tab.
    
    b. Click **List** button to call the device hub and retrieve a list of devices. If this is your first time, then you shouldn't retrieve anything.
    
    c. Click **Create** button to create a device ID and key.
    
    d. Once created successfully, device will be listed in device explorer.
    
    e. Right click the device and from context menu select "**Copy connection string for selected device**".
    
    f. Save this information in Notepad. You will need this information in later steps.

***Not running Windows on your PC?*** - Please send us an email on 
<azureiotcertified@microsoft.com> and we will follow up with you with
instructions.

<a name="Step_3:_Build"/>
# Step 3: Build and validate the sample using Node JS client libraries

This section walks you through building, deploying and validating the IoT Client SDK on your device running a Linux operating system. You will install necessary prerequisites on your device.  Once done,  you will build and deploy the IoT Client SDK and validate the sample tests required for IoT certification with the Azure IoT SDK.

<a name="Step_3_1:_Load"/>
## 3.1 Load the Azure IoT bits and prerequisites on device

-   Open a PuTTY session and connect to the device.

-   Choose your commands in next steps based on the OS running on your device.

-   Run following command to check if NodeJS is already installed

        node --version

    If version is **0.12.x**, then skip next step of installing prerequisite packages. Else uninstall it by issuing following command from command line on the device.

    **Debian or Ubuntu**
                        
        sudo apt-get remove nodejs

    **Fedora**            
                        
        sudo dnf remove nodejs
 
    **Any Other Linux OS**
                        
        Use equivalent commands on the target OS

-   Install the prerequisite packages by issuing the following commands from the command line on the device. Choose your commands based on the OS running on your device.

    **Debian or Ubuntu**
                        
        curl -sL https://deb.nodesource.com/setup_0.12 | sudo bash -

        sudo apt-get install -y nodejs

    **Fedora**            
                        
        wget http://nodejs.org/dist/v0.12.7/node-v0.12.7-linux-x64.tar.gz

        tar xvf node-v0.12.7-linux-x64.tar.gz 

        sudo mv node-v0.12.7-linux-x64 /opt

        echo "export PATH=\$PATH:/opt/node-v0.12.7-linux-x64/bin" >> ~/.bashrc

        source ~/.bashrc
 
    **Any Other Linux OS**
                        
        Use equivalent commands on the target OS
        
    **Note:** To test successful installation of Node JS, try to fetch its version information by running following command:

        node --version 

-   Download the SDK to the board by issuing the following command in
    PuTTY:

        git clone https://github.com/Azure/azure-iot-sdks.git

-   You will be prompted for your GitHub user name and password.

    ***Note:*** *If you have two-factor authentication enabled for your account, you'll need to generate/use a personal access token in place of your password. To generate token, navigate to <https://github.com/settings/tokens/new> and follow the on screen instructions.*

-   Verify that you now have a copy of the source code under the directory ~/azure-iot-suite-sdks.

<a name="Step_3_2:_Build"/>
## 3.2 Build the samples

-   To update samples run the following command on device.

    **For simple_sample_http.js:**
    
        nano azure-iot-sdks/node/samples/simple_sample_http.js

    **For send_batch_http.js:**
    
        nano azure-iot-sdks/node/samples/send_batch_http.js        
    
-   This launches a console-based text editor. Scroll down to the
    connection information.

-   Find the following place holder for IoT connection string:

        var connectionString = "[IoT Device Connection String]";

-   Replace the above placeholder with device connection string. You can get this from device explorer as explained in [Step 2](#Step_2:_Register), that you copied into Notepad.

-   Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER.

-   Press Ctrl+X to exit nano.

    **For registry_sample.js:**
    
        nano azure-iot-sdks/node/samples/registry_sample.js

-   This launches a console-based text editor. Scroll down to the
    connection information.

-   Find the following place holder for IoT connection string:

        var connectionString = "[IoT Connection String]";

-   Replace the above placeholder with "IoT Hub Connection String" which you used        in  [Step 2](#Step_2:_Register) for device registration.

-   Save your changes by pressing Ctrl+O and when nano prompts you to save it as the same file, just press ENTER.

-   Press Ctrl+X to exit nano.

-   Build the SDK code using the following command.

     	   ~/azure-iot-sdks/node/build.sh | tee LogFile.txt 

   ***Note:*** *LogFile.txt in above command should be replaced with a file name where build output will be written.*

<a name="Step_3_3:_Run"/>
## 3.3 Run and Validate the Samples

In this section you will run the Azure IoT client SDK samples to validate communication between your device and Azure IoT Hub service. You will send messages to the Azure IoT Hub service and validate that IoT Hub has successfully receive the data. You will also monitor any messages send from the Azure IoT Hub to client.

**Note:** Take screen shots of all operations, like sample screen shots, performed in below sections. These will be needed in [Step 4](#Step_4_2:_Share)

### 3.3.1 Send Device Events to IOT Hub:

1.  Launch the Device Explorer as explained in [Step 2](#Step_2:_Register) and navigate to **Data** tab. Select the device name you created from the drop-down list of device IDs, click **Monitor** button.

    ![DeviceExplorer_Monitor](images/3_3_1_01.png)

2.  Device Explorer is now monitoring data sent from the selected device to the IoT Hub.

3.  Run the sample by issuing following command:

        node ~/azure-iot-sdks/node/samples/simple_sample_http.js

4. Verify that the receive and send status messages show no error. If any, then you may have incorrectly copied the device hub connection information.

    **Note**: **204 No Content** is not an error.
 
   ![Simple_Sample_result_terminal](images/3_3_1_02.png)

5.  Device Explorer should show that IoT Hub has successfully received data sent by sample test.

    ![Simple_Sample_result_DeviceExplorer](images/3_3_1_03.png)

6.  Run the sample by issuing following command and then repeat step 4 to 5:

        node ~/azure-iot-sdks/node/samples/send_batch_http.js

    ![Send_Batch_result_terminal](images/3_3_1_04.png)
    ![Send_Batch_result_DeviceExplorer](images/3_3_1_05.png)
    
7.  Run the sample to register a device by issuing following command:

        node ~/azure-iot-sdks/node/samples/registry_sample.js

8.  Verify that you receive information for new device created in the messages.    
    
9.  In Device Explorer, go to Management tab and click List button. Your new device should show up in the list.

    ![Registry_Sample_result_DeviceExplorer](images/3_3_1_07.png)

### 3.3.2 Receive messages from IoT Hub

1.  To verify that you can send messages from the IoT Hub to your
    device, go to the **Notification** tab in Device Explorer.

2.  Select the device you created using Device ID drop down.

3.  Add some text to the Notification field, then click Send button.
    
    ![NotificationSend_DeviceExplorer](images/3_3_2_01.png)
    
4.  You should be able to see the command received in the console window
    of the client sample.

    ![NotificationSend_terminal](images/3_3_1_02.png)

<a name="Step_4:_Package_Share"/>
# Step 4: Package and Share

<a name="Step_4_1:_Package"/>
## 4.1 Package build logs and sample test results

Package following artifacts from your device:

1.  Build logs and E2E test results that were logged in the log file during build 
    run.

2.  All the screenshots that are shown above in "**Send Device Events to IoT Hub**" section.

3.  All the screenshots that are above in "**Receive messages from IoT Hub**" section.

4.  If you made any changes to above steps for your hardware, send us clear instructions of how to run this sample with your hardware (explicitly highlighting the new steps for customers). As a guideline on how the instructions should look please refer the examples published on GitHub repository [here](<https://github.com/Azure/azure-iot-sdks/tree/master/node/doc>)

<a name="Step_4_2:_Share"/>
## 4.2 Share package with Engineering Support

Share the package in email to <azureiotcertified@microsoft.com>.

<a name="Step_4_3:_Next"/>
## 4.3 Next steps

Once you shared the documents with us, we will contact you in the following 48 to 72 business hours with next steps.

<a name="Step_5:_Troubleshooting"/>
# Step 5: Troubleshooting

Please contact engineering support on <azureiotcertified@microsoft.com> for help with troubleshooting.
