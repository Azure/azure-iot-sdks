---
platform: raspbian
device: raspberrypi3/raspberrypi2
language: c
---

Run a configuration update sample to demonstrate the use of Device Twins on a RaspberryPi 3 (or 2) device. 
===
---

# Table of Contents

-   [Step 0: Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare your Device](#Step-2-PrepareDevice)
-   [Step 3: Build and run the device twin sample](#Step-3-Build)
-   [Step 4: Monitor Device Twin by changing desired property](#Step-4-Monitor)


<a name="Introduction"></a>
# Introduction

**Description of the scenario**: Your device on the field sends temperature and humidity telemetry data every 3 seconds. You are troubleshooting a temperature anomaly and want to receive the telemetry on a higher rate to detect if you are losing some data points, for example every 500 ms. In order to do this, you will have to issue a remote cloud to device command using device twin desired properties to request a change in the frequency rate of sending telemetry.

Here is the multi-step process that helps you achieve this:
-   Device sends telemetry data every 3000 ms. You have to connect to the device to start the app which sends telemetry. The 3000 ms is the default configured value using a Device Twin desired property called sendFrequency. 
-   Telemetry data and frequency could be observed in Device Explorer Twin on Data tab 
-   User sets the desired property sendFrequency to 500 ms to change the frequency of sending telemetry. You could do this by using Device Explorer Twin -> Management tab -> Twin Props. 
-   Device receives event notification of new desired property, changes the frequency rate for sending telemetry data. 
-   Device sets reported property sendFrequency to 500ms indicating the newly applied frequency. 
-   User is noticing the data flowing every 500 ms into Device Explorer -> Data tab 

Device Twins is a feature of Azure IoT Hub that allows synchronization between a device's configuration and cloud representation of the device, named Device Twin. Device Twin stores device 
specific metadata in the cloud and reports the current state such as available capabilities and conditions from your device app. You can query device twin to get your metadata, configuration or state.
You can learn more about Device Twin reading the following articles:

-   [Understand Device Twins][lnk-device-twin-intro]
-   [How to use the device twin][lnk-device-twin-get-started]
-   [How to use twin properties][lnk-device-twin-properties]

**About this document**
This document describes the process of setting up a [Raspberry Pi 3/2](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/) device, building and running the sample to connect to an Azure IoT hub with Device Management 
and how to monitor device twin to change the desire state on the device.

This multi-step process includes:
-   Prerequisites 
-   Prepare your device 
-   Build and run the device twin sample 
-   Monitor Device Twin by changing desired property 

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
-   Required hardware:
	-   [Raspberry Pi 3/2](https://www.adafruit.com/products/3055)
	-   8GB or larger MicroSD Card
	-   USB keyboard
	-   USB mouse (optional; you can navigate NOOBS with a keyboard)
	-   USB Mini cable
	-   HDMI cable
	-   TV/ Monitor that supports HDMI
	-   Ethernet cable or Wi-Fi dongle
	    You may wish to consider a Starter Kit such as [CanaKit](https://www.amazon.com/CanaKit-Raspberry-Complete-Starter-Kit/dp/B01C6Q2GSY/ref=sxts_k2p_hero2?pf_rd_m=ATVPDKIKX0DER&pf_rd_p=2668835642&pf_rd_r=ZNN3EEA1V2FNMVX4M6KH&pd_rd_wg=NKs7X&pf_rd_s=desktop-sx-top-slot&pf_rd_t=301&pd_rd_w=LjWyi&pf_rd_i=CanaKit+Raspberry+Pi+3&pd_rd_r=K2H6B8XEDVHTE4J35TN1&ie=UTF8&qid=1478205900&sr=2) that
	    includes some of these hardware requirements.
-   [Setup your IoT hub][lnk-setup-iot-hub] Note: Make sure you create an IoT Hub which has enabled Device Management (Check "Enable Device Management")
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [Set up Device Explorer with Twin][lnk-device-explorer-twin] or download it from [ here ](https://cdnx.azureedge.net/files/Device%20Explorer.zip)

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

Note: Skip next two steps if you already have an SD card with Raspbian OS image
-   Install the latest Raspbian operating system on your Raspberry Pi 3/2 by
following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/).
-   When the installation process is complete, the Raspberry Pi configuration menu
(raspi-config) loads. Here you are able to set the time and date for your region
and enable a Raspberry Pi camera board, or even create users. Under **Advanced
Options**, enable **ssh** so you can access the device remotely with
PuTTY or WinSCP. For more information, see <https://www.raspberrypi.org/documentation/remote-access/ssh/>.
-   Connect your Raspberry Pi to your network using an ethernet cable or by using
a WiFi dongle on the device.
-   You need to determine the IP address of your Raspberry Pi in order to connect over the network. Run following command to find the IP address of your device.
    ```
    hostname -I
    ```
-   Once you see that your board is working, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.
-   Use the IP address from step 4 as the Host name, Port=22, and Connection type=SSH to complete the connection.
-   When prompted, log in with username **pi**, and password **raspberry**.

<a name="Step-3-Build"></a>
# Step 3:  Build and run the device twin sample

Run the following commands in the terminal window connected to your Raspberry Pi.
-   Install git by running
    ```
    sudo apt-get install git
    ```

-   Download the Azure IoT device SDK to your Raspberry Pi:

    ```
    git clone --recursive https://github.com/azure/azure-iot-sdks -b mvp_summit 
    ```

-   Confirm that you now have a copy of the SDK under the directory ./azure-iot-sdks. Then cd to the directory:

    ```
    cd azure-iot-sdks
    ```

-   Prepare your environment by running. Answer **y** when you are prompted to install the additional components needed to run the samples:
    ```
    sudo c/build_all/linux/setup.sh
    ```

-   Edit the file ./c/serializer/samples/devicetwin_configupdate/devicetwin_configupdate.c and replace connection string placeholder with the device connection string you obtained when you [provisioned your device]([lnk-manage-iot-hub]).The device connection string should be in this format "`HostName=<iothub-name>.azure-devices.net;DeviceId=<device-name>;SharedAccessKey=<device-key>`".  
    You can use the console-based text editor **nano** to edit the file 

    ```
    nano ./c/serializer/samples/devicetwin_configupdate/devicetwin_configupdate.c
    ```
    This is the line that you need to change in devicetwin_configupdate.c
    ```
    static const char* connectionString = "[device connection string]";
    ```
-   Finally, build the sample applications:

    ```
    ./c/build_all/linux/build.sh --skip-unittests
    ```

<a name="rundevicetwinsample"/></a>
## Run the device twin config update sample ##

-   Run the **devicetwin_configupdate** sample:

    ```
    c/cmake/iotsdk_linux/serializer/samples/devicetwin_configupdate/devicetwin_configupdate
    ```
This device twin sample application sends temperature and humidity to your IoT Hub every 3000ms.

<a name="Step-4-Monitor"></a>
# Step 4:  Monitor Device Twin by changing desired property from the cloud

-   You could observe telemetry data and 3000 ms rate frequency in [ Device Explorer Twin ](https://cdnx.azureedge.net/files/Device%20Explorer.zip). Click on your device, then on Data tab, then push Monitor button to observe received events from raspberrypi3

  ![](./media/ObserveTelemetry.png)

-   Open now Management tab, Twin Props. and observe the entire twin object, with desired and reported properties. Notice desired "sendFrequency" is 3000 ms and reported "sendFrequency" is 3000 ms.
-   Let's assume now the following use case: You are troubleshooting a temperature anomaly and want to receive the telemetry on a higher rate to detect if you are losing some data points, 
    for example every 500 ms. In order to do this, you will have to issue a clone command using device twin desired properties to request a change of the frequency rate of sending telemetry. 
-   Set a new value for desired property "sendFrequency" to be 500 ms in Device Twin window, then click "Send (use Json format)" button, like following
    ```
        "telemetryConfig": {
            "sendFrequency": 500
        }
     ```     
    ![](./media/DeviceTwinProperties.png)  
-   The device app is notified of the change immediately if connected, or at the first reconnect. The device app then reports the updated configuration on reported properties, like following
    ```
    "reported": {
        "telemetryConfig": {
            "sendFrequency": 500
        }
    }
    ``` 
    ![](./media/DeviceTwinPropertiesChanged.png)  
-   Notice the frequency of telemetry data has changed to 500 ms in Device Explorer -> Data -> Monitor like in following image 

    ![](./media/ObserveTelemetryChanged.png)
-   Stop the devicetwin_configupdate app which is running on your device to simulate a disconnected device. Set a new value for desired property "sendFrequency" (let's say 7000) while the app is not running (see above step). 
    Notice the desire property "sendFrequency" is changed, but reported property "sendFrequency" is not because the device is disconnected. 
-   Start the device app again on the RaspberryPi by running  c/cmake/iotsdk_linux/serializer/samples/devicetwin_configupdate/devicetwin_configupdate
-   Go to Device Twin and just refresh the screen; you should notice reported property being changed to the same value as desired property without setting the desired property again. 
-   Here is one of the great value of the device twin: it stores the state of the device while the device is offline and when it wakes up, it's phoning home to IoTHub to get the latest desired state and execute it.

Note: You can learn more about how to use twins and how to implement methods by reading below tutorials:
-   You can read more about how to use desired properties to update configuration of the devices by reading [ how to configure devices using twins ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-how-to-configure/)
-   You can implement a reboot method by reading [ how to invoke direct methods ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-direct-methods/)
-   The application backend can keep track the results of the configuration operation across many devices, by querying twins. You can learn how to query twins by reading [ how to query twins ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-query-language/)
-   You can learn [ how to schedule jobs ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-jobs/) on multiple devices to update desired properties or tags or to invoke direct methods.

[lnk-setup-iot-hub]: ../../../../doc/setup_iothub.md
[lnk-manage-iot-hub]: ../../../../doc/manage_iot_hub.md
[devbox-setup]: ../../../doc/devbox_setup.md
[lnk-device-twin-intro]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-device-twins/
[lnk-device-twin-get-started]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-getstarted/
[lnk-device-twin-properties]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-device-twins/
[lnk-device-explorer-twin]: https://github.com/Azure/azure-iot-sdks/tree/master/c/serializer/samples/devicetwin_simplesample#step-3-monitor-device-twin
