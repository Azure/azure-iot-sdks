# Getting started - running a Java sample application

This "Getting Started" document shows you how to build and run any of the samples contained within the samples folder. 

## Prerequisites

Before you get started, you should:
- [Prepare your development environment][devbox-setup]
- [Setup your IoT Hub][setup-iothub]
- [Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document: How to use Device Explorer][device-explorer]

## Build and run the samples

If you have successfully [prepared your development environment][devbox-setup] the IoT device SDK and all samples should be built and ready to use. 

1. Start a new instance of the [Device Explorer][device-explorer], select or create a new device, obtain and note the connection string for the device, and begin monitoring under the Data tab.

2. Open the {IoT device SDK root}/java/service in your favourite Java IDE.
 
3. Running the Device Manager Sample application:

	Navigate to the main sample file for Device Manager
	It can be found at: 
	{IoT device SDK root}\java\service\samples\device-manager-sample\src\main\java\samples\com\microsoft\azure\iot\service\sdk\DeviceManagerSample.java

	Locate the following code in the file:

	```
	private static final String connectionString = "[Connection string goes here]";
	private static final String deviceId = "[Device name goes here]";
	```
	Replace "[Connection string goes here]" with the connection information used in Device Explorer.
	Replace "[Device name goes here]" with the name of the device you want to create, read. update or delete.

	Locate the main function:
    public static void main(String[] args) throws IOException, URISyntaxException, Exception

	Notice there are function calls implemented for each CRUD operation and they called from the main function in order.

	Pick which operation you want to run, and comment out the others.

	Run the application.

	You can verify the result of your operation by using Device Explorer.


4. Running the Service Client Sample application:

	Navigate to the main sample file for Device Manager
	It can be found at: 
	{IoT device SDK root}\java\service\samples\service-client-sample\src\main\java\samples\com\microsoft\azure\iot\service\sdk\ServiceClientSample.java

	Locate the following code in the file:

	```
	private static final String connectionString = "[Connection string goes here]";
	private static final String deviceId = "[Device name goes here]";
	```
	Replace "[Connection string goes here]" with the connection information used in Device Explorer.
	Replace "[Device name goes here]" with the name of the device you want to create, read. update or delete.

	Locate the main function:
	public static void main(String[] args) throws IOException, URISyntaxException, Exception

	Update the value of the local variable "commandMessage" to contain the message you want tot send to the device.

	Run the Service Client Sample project. The application will send the message to the device and wait for feedback batch from the Azure Iot Hub.

## Documentation

The documentation can be found [here](https://azure.github.io/azure-iot-sdks/java/service/api_reference/index.html).

[devbox-setup]: devbox_setup.md
[setup-iothub]: ../../../doc/setup_iothub.md
[device-explorer]: ../../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
