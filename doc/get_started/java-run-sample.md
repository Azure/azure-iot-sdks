Run a simple Java sample on device
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

This document describes how to build and run the Java sample application. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment][devbox-setup]
-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
-   [Prepare your development environment](https://github.com/Azure/azure-iot-sdks/blob/master/csharp/device/doc/devbox_setup.md).
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).

#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Make sure desktop is ready as per instructions given on [Prepare your development environment][devbox-setup].

<a name="Build"></a>
# Step 3: Build and Run the sample

If you have successfully [prepared your development environment][devbox-setup] the IoT device SDK and all samples should be built and ready to use.

1. Start a new instance of the [Device Explorer][device-explorer], select or create a new device, obtain and note the connection string for the device, and begin moitoring under the Data tab.

2. Navigate to the folder containing the executable JAR file for the sample that you wish to run and run the samples as follows:

	The executable JAR file for sending event
	can be found at:

	```
	{IoT device SDK root}/java/device/samples/send-event/target/send-event-{version}-with-deps.jar
	```

	Navigate to the directory with the event sample. Run the sample using the following command:

	```
	java -jar ./send-event-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "{https or amqps}"
	```

	The executable JAR file for receiving messages can be found at:

	```
	{IoT device SDK root}/java/device/samples/handle-messages/target/handle-messages-{version}-with-deps.jar
	```

	Navigate to the directory with the messages sample. Run the sample using the following command:

	```
	java -jar ./handle-messages-{version}-with-deps.jar "{connection string}" "{https or amqps}"
	```

	Note that the double quotes around each argument are required, but the braces '{' and '}' should be removed.

## Next Steps
To learn how to create a Java application that communicates with an IoT hub from scratch, see [How to write an Azure IoT client application from scratch with Java on Windows][create-from-scratch].

## Documentation

The documentation can be found [here](https://azure.github.io/azure-iot-sdks/java/device/api_reference/index.html).

[create-from-scratch]: how_to_build_a_java_app_from_scratch.md
[devbox-setup]: java-devbox-setup.md
[device-explorer]: <https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md>
