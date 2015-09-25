# Getting started - running a C sample application on a BeagleBone Black device

This "Getting Started" document shows you how to build and run the **simplesample_amqp** application on a BeagleBone Black device. This sample application connects your BeagleBone to an IoT hub.

**Table of Contents**
- [Requirements](#Requirements)
- [Connect the board](#Connect-the-board)
- [Verify that you can connect to the device](#Verify-that-you-can-connect-to-the-device)
- [Install and build the Microsoft Azure IoT device SDK for C libraries](#Load-the-Azure-IoT-bits-and-prerequisites)
- [Build and run the sample client application](#Build-the-Azure-client)

<a name="Requirements"/>
## Requirements

You should have available:
- An SSH client, such as [PuTTY](http://www.putty.org/) on Windows, so you can access the command line on the BeagleBone.
- Either the Chrome or Firefox web browser, so you can browse to the embedded webserver on your your BeagleBone.
- A BeagleBone Black device You can find a list of distributors to buy a board from at [beagleboard.org](http://beagleboard.org/black) .
- A network router configured to allow the BeagleBone to connect to the internet over a wired Ethernet connection.

These instructions assume that you have done the following:
- [Setup your IoT Hub][setup-iothub]
- Configure your device in IoT Hub. <br/>See the section "Configure IoT Hub connection" in the document [How to use Device Explorer][device-explorer].

## Connect the board

Follow the [Getting Started](http://beagleboard.org/getting-started) instructions at [beagleboard.org](http://beagleboard.org/) to connect your BeagleBone to your computer.

> Note: Although it is not necessary in order to complete the following steps, you may want to consider [updating your board](http://beagleboard.org/getting-started#update) to the latest software revision.

<a name="Verify-that-you-can-connect-to-the-device"/>
## Verify that you can connect to the device

- Once you see that your board is alive, use Chrome or Firefox to browse to http://192.168.7.2. This is a web site hosted on your board.
- (Optional)  Go to the [Interactive Guide](http://192.168.7.2/Support/BoneScript/) page and go through the basic samples. For example, turn the LEDs on, turn LEDs off. Verify that your board behaves as expected.
- Open an SSH terminal program on your desktop machine (for example, [PuTTY](http://www.putty.org/) on Windows).
- Connect using the following parameters: Host name=192.168.7.2, Port=22, Connection type=SSH.
- When prompted, log in with username **root**, no password.
- Connect an Ethernet cable to your board. Ping a website such as bing.com to verify connectivity to the internet.
- If the ping command doesn't work, reboot your board, log in again using your SSH client, and run the command **ifconfig**. You should see the **eth0**, **lo**, and **usb0** interfaces. The **eth0** interface should have been assigned a valid IP address by a DHCP server on your network.

<a name="Load-the-Azure-IoT-bits-and-prerequisites"/>
## Install and build the Microsoft Azure IoT device SDK for C libraries

- Open an SSH session and connect to the board (as described in the previous section).
- Install the Git package by issuing the following commands in the SSH session:

  ```
  sudo apt-get update
  sudo apt-get install git
  ```

- Download a copy of this repository to the board using the `git clone` command.

- You now should have a complete copy of the Azure IoT Hub SDKs repository on your BeagleBone. You should always use the **master** branch which contains the latest release of the software.

- Navigate to the folder **c/build_all/linux** in your local copy of this repository.

3. Execute the `setup.sh` script to install the prerequisite packages and the dependent libraries.

4. Run the `build.sh` script to verify that the SDK libraries and samples build successfully on the BeagleBone board.

<a name="Build-the-Azure-client"/>
## Build and run the sample client application

1. In your SSH session, open the file **c/serializer/samples/simplesample_amqp/simplesample_amqp.c** in a text editor such as Nano.

2. Locate the following code in the file:
    ```
   static const char* connectionString = "[device connection string]";
    ```
3. Replace "[device connection string]" with your connection string (to learn how to obtain this device connection string using the device explorer, read [this](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md)).

4. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.


5. Save your changes and build the samples. To build your sample you can run the `build.sh` script in the **c/build_all/linux** directory. To skip running the unit tests, use:

  ```
  ./build.sh --skip-unit-tests
  ```

6. Run the **c/serializer/samples/simplesample_amqp/linux/simplesample_amqp** sample application.

7. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_amqp** application.

8. See "Send cloud-to-device messages" in the document [How to use Device Explorer for IoT Hub devices][device-explorer] for instructions on sending messages with the **DeviceExplorer** utility .


[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
