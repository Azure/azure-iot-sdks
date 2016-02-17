# Getting started - running a Java sample application

This "Getting Started" document shows you how to build and run any of the samples contained within the samples folder.

## Prerequisites

Before you get started, you should:
- [Prepare your development environment][devbox-setup]
- [Setup your IoT hub][lnk-setup-iot-hub]
- [Provision your device and get its credentials][lnk-manage-iot-hub]

## Build and run the samples

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
To learn how to create a Java application that communicates with an IoT hub from scratch, see [Get started with Azure IoT Hub for Java][create-from-scratch].

## Documentation

The documentation can be found [here](https://azure.github.io/azure-iot-sdks/java/device/api_reference/index.html).

[create-from-scratch]: https://azure.microsoft.com/documentation/articles/iot-hub-java-java-getstarted/
[devbox-setup]: devbox_setup.md
[device-explorer]: ../../../tools/DeviceExplorer/devdoc/how_to_use_device_explorer.md

[setup-iothub]: ../../../doc/setup_iothub.md
[lnk-manage-iot-hub]: ../../../doc/manage_iot_hub.md

