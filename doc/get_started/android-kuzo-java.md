---
platform: android
device: kuzo
language: java
---

Run a simple JAVA sample on Kuzo device running Android L (v5.1)
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

This document describes how to connect Intel Sofia with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   Prepare your development environment:

    -   Download and install latest JDK from [here](<http://www.oracle.com/technetwork/java/javase/downloads/index.html>).
    -   Download [Android Studio](<https://developer.android.com/studio/index.html>) on your Windows machine and follow the installation instructions.
    -   Computer with Git client installed and access to the [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.

-   [Setup your IoT hub][lnk-setup-iot-hub]

-   [Provision your device and get its credentials][lnk-manage-iot-hub]


<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Make sure desktop is ready as per instructions given in [Prepare your development environment](#Setup_DevEnv).

-   Plug in your device to your development machine with a USB cable. If you're developing on Windows, you might need to install the appropriate USB driver for your device. For help installing drivers, see the [OEM USB Drivers](<https://developer.android.com/studio/run/oem-usb.html>) document.

-   Enable USB debugging on your device. On Android 4.0 and newer, go to Settings > Developer options. Kuzo device USB debugging is on by default,so this step can be ignored.
    ***Note***: *On Android 4.2 and newer, Developer options is hidden by default. To make it available, go to Settings > About phone and tap Build number seven times. Return to the previous screen to find Developer options.*


<a name="Build"></a>
# Step 3: Build and Run the sample

Please find Azure Android java device sample code [here][android-sample-code].

<a name="Step_3_1"></a>
## 3.1 Modify the Samples for screenless device

The sample application will require user interaction to click a button on screen to receive messages from IoT hub. If the target device is a screenless device, you may modify the sample code to have the app to receive message right after sending messages without user interactions.

Following is the example of modification:

In [MainActivity.java][mainactivity-source-code]:

	```
	public class MainActivity extends AppCompatActivity {

    	String connString = "[device connection string]";
	    //IotHubClientProtocol curTestProtocol = IotHubClientProtocol.HTTPS;
	    //IotHubClientProtocol curTestProtocol = IotHubClientProtocol.MQTT;

	    // setup a variable to switch the usage of Azure connection protocol globally.
	    IotHubClientProtocol curTestProtocol = IotHubClientProtocol.HTTPS;

	    . . .
    	public void SendMessage() throws URISyntaxException, IOException {
    	    . . .
    	    // Comment/uncomment from lines below to use HTTPS or MQTT protocol
    	    //IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;
    	    //IotHubClientProtocol protocol = IotHubClientProtocol.MQTT;
    	    IotHubClientProtocol protocol = curTestProtocol;
    	    . . .
    	}
    	. . .

    	public void btnReceiveOnClick(View v) throws URISyntaxException, IOException {
      		. . .
	      	// Comment/uncomment from lines below to use HTTPS or MQTT protocol
	      	//IotHubClientProtocol protocol = IotHubClientProtocol.HTTPS;
	      	//IotHubClientProtocol protocol = IotHubClientProtocol.MQTT;
	      	IotHubClientProtocol protocol = curTestProtocol;
	      	. . .
    	}
    	. . .
	}

	protected void onCreate(Bundle savedInstanceState) {
	    . . .
	    try {
	        SendMessage();
	        btnReceiveOnClick(null); // call button click event handler
	    }
	    . . .
	}

	public void btnReceiveOnClick(View v) throws URISyntaxException, IOException {
	    . . .
	    try {
	        // Thread.sleep(2000);
	        int maxMsgCount = 1; // exit the while loop when this number of messages is received.
	        long maxTimeout = 10000; // in milliseconds
	        long elapsedTime = 0;
	        System.out.println("Start to receive messages from IoTHub...");
	        while (maxMsgCount > counter.get()) {
	            Thread.sleep(500);
	            elapsedTime += 500;
	            if (elapsedTime >= maxTimeout) {
	                System.out.println("Timeout!");
	                break;
	            }
	        }
	        System.out.println("Stop receiving messages from IoTHub!");
	    }
	    . . .
	}
	```

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
