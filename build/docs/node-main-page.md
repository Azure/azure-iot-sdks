The Microsoft Azure IoT device libraries for Node.js contain code that facilitates building devices and applications that connect to and are managed by Azure IoT Hub services. The libraries also include code to manage your IoT Hub service.

## Features

Device libraries
* Sends event data to Azure IoT based services.
* Maps server commands to device functions.
* Buffers data when network connection is down.
* Batches messages to improve communication efficiency.
* Supports pluggable transport protocols. HTTP and AMQP protocols are available now, with more coming soon.
* Supports pluggable serialization methods. JSON is available now, with more coming soon.

Service libraries
* A service client you can use to send cloud-to-device commands from IoT Hub to your devices.
* A registry manager that you can use to manage the contents of the IoT Hub device identity registry.
