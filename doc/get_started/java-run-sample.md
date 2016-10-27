---
platform: debian, fedora, Linux, opensuse, raspbian, Ubuntu, windows, yocto 
device: any
language: java
---

Run a simple Java sample on device
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Command Line Arguments](#command_line_arguments) 

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
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

-   Make sure desktop is ready as per instructions given on [Prepare your development environment][devbox-setup].

<a name="Build"></a>
# Step 3: Build and Run the sample

If you have successfully [prepared your development environment][devbox-setup] the IoT device SDK and all samples should be built and ready to use.

1. Start a new instance of the [Device Explorer][device-explorer], select or create a new device, obtain and note the connection string for the device, and begin monitoring under the Data tab.

2. Build the client library along with the **samples** :
	```
	{IoT device SDK root}/java/device/>mvn install -DskipTests
	```
On build completion, you will see summary of what got built.    

3. Navigate to the folder containing the executable JAR file for the sample that you wish to run and run the samples as follows:

	For example, the executable JAR file for sending and receving event, **send-receive-sample** can be found at:

	```
	{IoT device SDK root}/java/device/samples/send-receive-sample/target/send-event-{version}-with-deps.jar
	```

	Navigate to the directory with the sample. Run the sample using the following command:

	```
	java -jar ./send-receive-sample-{version}-with-deps.jar "{connection string}" "{number of requests to send}" "{https or amqps or mqtt or amqps_ws }"
	```

	Note that the double quotes around each argument are required, but the braces '{' and '}' should be removed.

<a name="command_line_arguments"></a>
## More details on command line arguments
Samples would use following command line arguments:

1. [Device connection string] - `HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>`

2. [Number of requests to send]: For example, **5** 

3. [`https | amqps | mqtt | amqps_ws`]: For example, amqps_ws (AMQP over WebSocket)

4. [Path to certificate to enable 1-way authentication]: For example, `azure-iot-sdks\c\certs\ms.der` **optional argument**

Path to certificate is an **optional** argument and would be needed in case you want to point it to the local copy of the Server side certificate. Please note that this option is used by client for validating Root CA sent by Azure IoT Hub Server as part of TLS handshake. It is for 1-way TLS authentication and is **not** for specifying client side certificate (2-way TLS authentication).

## Next Steps
To learn how to create a Java application that communicates with an IoT hub from scratch, see [Get started with Azure IoT Hub for Java][how-to-build-a-java-app-from-scratch].

## Documentation

The documentation can be found [here](https://azure.github.io/azure-iot-sdks/java/device/api_reference/index.html).

[devbox-setup]: java-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
[how-to-build-a-java-app-from-scratch]: https://azure.microsoft.com/documentation/articles/iot-hub-java-java-getstarted/
