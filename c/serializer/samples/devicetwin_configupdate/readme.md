---
platform: raspbian
device: raspberrypi3/raspberrypi2
language: c
---
How to Update an IoT Device Configuration with Azure IoT Device Twins
===
---

# Table of Contents
-   [Step 0: Introduction](#Introduction)
-   [Step 1: Prerequisites](#Step-1-Prerequisites)
-   [Step 2: Prepare Device](#Step-2-PrepareDevice)
-   [Step 3: Build and Run the Device Twins Sample](#Step-3-Build)
-   [Step 4: Monitor Device Twins](#Step-4-Monitor)

<a name="Introduction"></a>
# Introduction
## Device Twins
Device Twins is a feature of Azure IoT Hub that allows synchronization between a device's configuration and cloud representation of the device. Device Twins stores device 
specific metadata in the cloud and reports the current state such as available capabilities and conditions from your device's app. You can query Device Twins to get your metadata, configuration or state.
You can learn more about Device Twins by reading the following articles:

-   [Understand Device Twins][lnk-device-twin-intro]
-   [How to use Device Twins][lnk-device-twin-get-started]
-   [How to use Device Twins Desired Properties][lnk-device-twin-properties]

## Scenario Description
Your field device sends temperature and humidity telemetry data every 3 seconds. You are troubleshooting a temperature anomaly and want to receive the telemetry at a higher rate, 500ms instead of 3s, to detect if you are losing some data points. In order to do this, you will have to issue a remote cloud-to-device command using a Device Twins "Desired Property" to request a change in the telemetry send frequncy.

Here is the overall process you will follow to achieve this:
-   You will connect to the device and start the app that sends telemetry. The send frequency value of 3000 ms is the default configured value via a Device Twins Desired Property called sendFrequency. 
-   You will observe telemetry data via the Device Explorer's Data tab. 
-   You will set the Desired Property sendFrequency to 500 ms to change the telemetry send frequency. You will do this by using Device Explorer Twin -> Management tab -> Twin Props. 
-   Your device will receive an event notification of the updated Desired Property and will change the frequency rate for sending telemetry data. 
-   Your device will set the property sendFrequency to 500ms indicating the newly applied frequency. 
-   You will observe the data flowing every 500 ms into Device Explorer's Data tab 

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning this tutorial:
-   A computer with [Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git) client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   SSH client on your desktop computer, such as [PuTTY](http://www.putty.org/), so you can remotely access the command line on the Raspberry Pi.
-   Required hardware:
	-   [Raspberry Pi 3/2](https://www.adafruit.com/products/3055)
	-   8GB or larger MicroSD Card
	-   Wired USB keyboard 
	-   Wired USB mouse (optional; you can navigate NOOBS with a keyboard)
	-   Micro USB cable
    -   5V/2a Power Supply
	-   HDMI cable
	-   HDMI Capable TV or Monitor
	-   Ethernet cable or Wi-Fi dongle
	    You may wish to consider a Starter Kit such as [CanaKit](https://www.amazon.com/CanaKit-Raspberry-Complete-Starter-Kit/dp/B01C6Q2GSY/ref=sxts_k2p_hero2?pf_rd_m=ATVPDKIKX0DER&pf_rd_p=2668835642&pf_rd_r=ZNN3EEA1V2FNMVX4M6KH&pd_rd_wg=NKs7X&pf_rd_s=desktop-sx-top-slot&pf_rd_t=301&pd_rd_w=LjWyi&pf_rd_i=CanaKit+Raspberry+Pi+3&pd_rd_r=K2H6B8XEDVHTE4J35TN1&ie=UTF8&qid=1478205900&sr=2) that
	    includes some of these hardware requirements.
-   [Setup your IoT Hub][lnk-setup-iot-hub] Note: Make sure you create an IoT Hub which has Device Management (Check "Enable Device Management")
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   [Set up Device Explorer with Twin][lnk-device-explorer-twin] or download it from [ here ](https://cdnx.azureedge.net/files/Device%20Explorer.zip)

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

>Note: You can skip the Raspbian install if you already have it on your SD card.

-   Install the latest Raspbian operating system on your Raspberry Pi 3/2 by
following the instructions in the [NOOBS setup guide](http://www.raspberrypi.org/help/noobs-setup/). 
-   When the installation process is complete, the Raspberry Pi configuration menu
(raspi-config) loads. 
-   Under **Advanced
Options**, enable **ssh** so you can access the device remotely with
PuTTY or WinSCP. For more information, see <https://www.raspberrypi.org/documentation/remote-access/ssh/>.
-   Connect your Raspberry Pi to your network using an ethernet cable or a WiFi dongle.
-   You need to determine the IP address of your Raspberry Pi in order to connect to it over the network. Open Terminal and run the following command to find the IP address of your device.
    ```
    hostname -I
    ```
-   Once you see that your board is working, open an SSH terminal program such as [PuTTY](http://www.putty.org/) on your desktop machine.
-   Use the IP address from the previous step as the Host name, Port=22, and Connection type=SSH to complete the connection.
-   When prompted, log in with username **pi**, and password **raspberry**.

<a name="Step-3-Build"></a>
# Step 3:  Build and Run the Device Twins Sample

Run the following commands in the terminal window connected to your Raspberry Pi.
-   Install git by running
    ```
    sudo apt-get install git
    ```

-   Download the Azure IoT Device SDK to your Raspberry Pi:

    ```
    git clone --recursive https://github.com/azure/azure-iot-sdks -b mvp_summit 
    ```

-   Confirm that you now have a copy of the SDK under the directory ./azure-iot-sdks. Then cd to the directory:

    ```
    cd azure-iot-sdks
    ```

-   Prepare your environment by running. (Answer **y** when you are prompted to install the additional components needed to run the samples):
    ```
    sudo c/build_all/linux/setup.sh
    ```

-   Edit the file ./c/serializer/samples/devicetwin_configupdate/devicetwin_configupdate.c and replace the connection string placeholder with the device connection string you obtained when you [provisioned your device]([lnk-manage-iot-hub]).The device connection string should be in this format "`HostName=<iothub-name>.azure-devices.net;DeviceId=<device-name>;SharedAccessKey=<device-key>`".  
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
## Run the Device Twins Config Update Sample ##

-   Run the **devicetwin_configupdate** sample:

    ```
    c/cmake/iotsdk_linux/serializer/samples/devicetwin_configupdate/devicetwin_configupdate
    ```
You should now be sending temperature and humidity to your IoT Hub every 3000ms.

<a name="Step-4-Monitor"></a>
# Step 4:  Monitor Device Twins by changing the Desired Property from the Cloud

-   You could observe telemetry data and 3000 ms rate frequency in [ Device Explorer Twin ](https://cdnx.azureedge.net/files/Device%20Explorer.zip). Click the Data tab, then click the Monitor button to observe received events from your device.

  ![](./media/ObserveTelemetry.png)

-   Open the Management tab, Twin Props and observe the entire twin object, with desired and reported properties. Notice desired "sendFrequency" is 3000 ms and reported "sendFrequency" is 3000 ms.
-   Let's assume now the following use case: You are troubleshooting a temperature anomaly and want to receive the telemetry on a higher rate to detect if you are losing some data points, 
    for example every 500 ms. In order to do this, you will have to issue a clone command using Device Twins Desired Properties to request a change of the frequency rate of sending telemetry. 
-   Set a new value for Desired Property "sendFrequency" to be 500 ms in Device Twins window, then click "Send (use Json format)" button, like following
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
-   Stop the devicetwin_configupdate app, which is running on your device, to simulate a disconnected device. Set a new value for Desired Property "sendFrequency" (let's say 7000) while the app is not running (see above step). 
    Notice the desire property "sendFrequency" is changed, but reported property "sendFrequency" is not because the device is disconnected. 
-   Start the device app again on the RaspberryPi by running  c/cmake/iotsdk_linux/serializer/samples/devicetwin_configupdate/devicetwin_configupdate
-   Go to Device Twins and refresh the screen. You should notice reported property being changed to the same value as Desired Property without setting the Desired Property again. 
-   Here is one of the great values of the Device Twins: it stores the state of the device while the device is offline and when it wakes up, it will phone home to IoTHub to get the latest state.

Note: You can learn more about how to use Device Tins and how to implement methods by reading below tutorials:
-   You can read more about how to use Desired Properties to update configuration of the devices by reading [ how to configure devices using twins ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-how-to-configure/)
-   You can implement a reboot method by reading [ how to invoke direct methods ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-direct-methods/)
-   The application backend can keep track the results of the configuration operation across many devices, by querying twins. You can learn how to query twins by reading [ how to query twins ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-query-language/)
-   You can learn [ how to schedule jobs ](https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-jobs/) on multiple devices to update Desired Properties or tags or to invoke direct methods.

[lnk-setup-iot-hub]: ../../../../doc/setup_iothub.md
[lnk-manage-iot-hub]: ../../../../doc/manage_iot_hub.md
[devbox-setup]: ../../../doc/devbox_setup.md
[lnk-device-twin-intro]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-device-twins/
[lnk-device-twin-get-started]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-node-node-twin-getstarted/
[lnk-device-twin-properties]: https://azure.microsoft.com/en-us/documentation/articles/iot-hub-devguide-device-twins/
[lnk-device-explorer-twin]: https://github.com/Azure/azure-iot-sdks/tree/master/c/serializer/samples/devicetwin_simplesample#step-3-monitor-device-twin
