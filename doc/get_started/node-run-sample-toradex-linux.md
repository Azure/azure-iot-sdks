# Getting started - running a Node.js sample application on Toradex modules with Linux

This "Getting Started" document shows you how to run the **simple_sample_http.js** Node.js sample application on a Toradex module with Linux.

## Prerequisites

Before you get started, you should:

- [Setup your IoT hub][lnk-setup-iot-hub]
- [Provision your device and get its credentials][lnk-manage-iot-hub]
- Ensure the module is flashed with [Toradex V2.5 Linux image or newer][toradex_image_update].

## Configure Node.js on Toradex module with Linux

1.  Insert the module into a compatible carrier board.  Power on the system and connect to the internet.

2.  Use the command line to install Node.js and necessary dependencies:

    ```
    opkg update
	opkg install nodejs nodejs-npm python python-simplejson
    ```

3.  Use the Node.js package manager (npm) to install the **azure-iot-device** library, the [sample Node.js device applications][node-sample-apps] and the sample applications' other dependencies:

    ```
    npm install azure-iot-device
	cd node_modules/azure-iot-device/samples
	npm install
	```

## Run Node.js Sample Application

1. On the host machine, start a new instance of the [Device Explorer][device-explorer], select or create a new device, obtain and note the connection string for the device, and begin monitoring under the Data tab.

2. Back on the device, at the Azure IoT Device samples location (node_modules/azure-iot-device/samples), edit the example javascript file(s) (e.g. simple_sample_http.js):

    ```
    vi simple_sample_http.js
    ```

   And locate the following line to change [IoT Device Connection String] to your connection string as provided by your IoT hub instance:

	```
	var connectionString = '[IoT Device Connection String]';
	```

3. Execute the sample application using Node.js:

    ```
    node simple_sample_http.js
    ```

   The sample application will send messages to your IoT hub and the Device Explorer utility will display the messages as your IoT hub receives them.

[device-explorer]: ../../tools/DeviceExplorer/readme.md
[toradex_image_update]: http://developer.toradex.com/knowledge-base/how-to-setup-environment-for-embedded-linux-application-development#Linux_Image_Update
[node-sample-apps]: https://github.com/Azure/azure-iot-sdks/tree/master/node/device/samples

[setup-iothub]: ../setup_iothub.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
