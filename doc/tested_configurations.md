# OS Platforms and hardware compatibility

## OS Platforms

The Azure IoT libraries has been tested on the following OS platforms:


|OS Platform  |   Version|
|:---------------|:------------:|
|Debian Linux| 7.5|
|Fedora Linux|20|
|mbed OS| 2.0 |
|Raspbian Linux| 3.18 |
|Ubuntu Linux| 14.04 |
|Windows desktop| 7,8,10 |
|Windows IoT Core| 10 |
|Windows Server| 2012 R2|
|Yocto Linux|2.1 |



## C libraries

The [Microsoft Azure IoT Device SDK for C](c/device/readme.md) has been tested on the following configurations:

|OS Platform| Version|Protocols|
|:---------|:----------:|:----------:|
|Debian Linux| 7.5 | HTTPS, AMQP, MQTT |
|Fedora Linux| 20 | HTTPS, AMQP, MQTT |
|mbed OS| 2.0 | HTTPS, AMQP |
|Ubuntu Linux| 14.04 | HTTPS, AMQP, MQTT |
|Windows desktop| 7,8,10 | HTTPS, AMPQ, MQTT |
|Yocto Linux|2.1  | HTTPS, AMQP|



## Node.js libraries
The [Microsoft Azure IoT Device SDK for Node.js](node/device/readme.md) has been tested on the following configurations:


|Runtime| Version|Protocols|
|:---------|:----------:|:----:|
|Node.js| 4.1.0 | HTTPS|



## Java libraries
The [Microsoft Azure IoT Device SDK for Java](java/service/readme.md) has been tested on the following configurations:

|Runtime| Version|Protocols|
|:---------|:----------:|----|
|Java| 1.7 | HTTPS, AMQP |

The [Microsoft Azure IoT Service SDK for Java](java/service/readme.md) has been tested on the following configurations:

|Runtime| Version|Protocols|
|:---------|:----------:|:-----|
|Java| 1.8 | HTTPS, AMQP |


## CSharp
The [Microsoft Azure IoT Device SDK for .NET](csharp/readme.md) has been tested on the following configurations:

|OS Platform| Version|Protocols|
|:---------|:----------:|:----------:|
|Windows desktop| 7,8,10 | HTTPS, AMPQ|
|Windows IoT Core|10 | HTTPS|

Managed agent code requires Microsoft .NET Framework 4.5



# Microsoft Azure Certified for IoT



Microsoft Azure Certified for IoT is the partner program that connects the broader IoT ecosystem  with Microsoft Azure so that developers and architects understand the compatibility scenarios. Specifically, it provides a trusted list of OS/device combinations to help you getting started quickly with an IoT project – whether you’re in a proof of concept or pilot phase. With certified device and operating system combinations, your IoT project can get started quickly, with less work and customization required to make sure devices are compatible with  Azure IoT Suite and Azure IoT Hub.

## Certified for IoT Devices

Certified for IoT Devices have tested compatibility with the Azure IoT SDKs and are ready to be used in your IoT application. Specifically, we identify compatibility based on OS Platform and code language.

#### Devices list
 Each device has been certified to work with our SDK in the OS and language chosen by the device manufacturer. For example, Beaglebone Black works on Debian using our C, Javascript and Java language. This means that developers will be able to build applications in any of those languages and OS combinations on the specific devices.

 Learn more about developing solutions using Certified for IoT devices here (missing link).

|Device| Tested OS |Language|
|:---------|:----------|:----------|
|Raspberry Pi 2| Raspbian | C, Javascript, Java |
|Raspberry Pi 2| Windows 10 IoT Core|  C, Javascript, C#|
|Beaglebone Black| Debian |C, Javascript, Java|
|Beaglebone Green|Debian |C, Javascript, Java|
|TI CC3200 | TI RTOS |C|
|Intel Edison |Yocto |C, Javascript|
|Minnowboard Max |Windows 7,8, 10 |C#|
|Arrow Dragonboard |Windows 10 IoT Core | C#|
|Freescale FRDM K64 |mbedOS | C|
