# Run the iotdm\_edison\_sample on Intel Edison

This document describes how to build and run the iotdm\_edison\_sample application on an Intel Edison device.

## Prerequisites

You should also have the following items ready before beginning the process:

-   [Setup your IoT hub]
-   [Provision your device and get its credentials]

This setup process requires **cmake** version 2.8.11 or higher and **gcc** version 4.9 or higher:

-   You can verify the current version of **cmake** installed in your environment using the `cmake --version` command.
-   You can verify the current version of **gcc** installed in your environment using the `gcc --version` command
-   If you run into issues installing **git** on an Edison device refer to the following article <https://github.com/w4ilun/edison-guides/wiki/Installing-Git-on-Intel-Edison>

## Building and running iotdm\_simple\_sample on Intel Edison

1. Setup your Edison board and connect it to your wireless network.

  To set up your Edison board, you need to connect it to a host computer. Intel has a set of getting started guides for the following operating systems:

 -   [Get Started with the Intel Edison Development Board on Windows 64-bit.](https://software.intel.com/get-started-edison-windows)
 -   [Get Started with the Intel Edison Development Board on Windows 32-bit.](https://software.intel.com/get-started-edison-windows-32)
 -   [Getting Started with the Intel® Edison Board on Linux.](https://software.intel.com/get-started-edison-linux)

  To set up your Edison board and familiarize yourself with it, you should complete all the steps in these "Get started" articles except for:

 -  Flashing the latest firmware. You update the firmware as part of this tutorial so you don't need to complete this step at this time.
 - The last step, "Choose IDE", which is unnecessary for the current tutorial.

   When you have set up the Edison board and installed the necessary drivers on your host machine, you should make sure you can connect to the Edison board using a serial terminal. The [Setting up a serial terminal](https://software.intel.com/setting-up-serial-terminal-intel-edison-board) page on the Intel website has links to set up instructions for host operating systems such as Windows and Linux.


2. Update packages on the Edison

    Add these package repositories to the list of feeds (`vi /etc/opkg/base-feeds.conf`)

    ```
    src/gz all http://repo.opkg.net/edison/repo/all
    src/gz edison http://repo.opkg.net/edison/repo/edison/
    src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32/
    src/gz i586 http://iotdk.intel.com/repos/1.5/iotdk/i586/
    ```

    Then type:
    ```
    opkg update
    opkg install git
    opkg install util-linux-dev
    opkg install nodejs
    opkg install cmake
    ```

2.  Clone the latest version of this repository to your Edison

    ```
    git clone --recursive --branch dmpreview https://github.com/Azure/azure-iot-sdks.git
    ```

    Note: **git** on Edison does not support the **submodule** command. You need to process the submodules manually. To do that follow the steps below:

    ```
    cd ~/azure-iot-sdks
    mkdir -p c/azure-c-shared-utility
    pushd c/azure-c-shared-utility
    git clone https://github.com/Azure/azure-c-shared-utility.git .
    git checkout d22667f741684b4e79f471d2057103aa796a52e8
    popd
    mkdir -p c/wakaama
    pushd c/wakaama
    git clone https://github.com/mhshami01/wakaama.git .
    git checkout edb4a4a50f5f8184b309a79ed55c1294943ce587
    popd
    mkdir -p c/wakaama/tests/utils/tinydtls
    pushd c/wakaama/tests/utils/tinydtls
    git clone https://git.eclipse.org/r/tinydtls/org.eclipse.tinydtls .
    git checkout f623c23ba4b9589147a19ef25a31b4cd0a7514e3
    popd
    ```

3.  Configure the IoTHub connection in the source (optional)

    The iotdm\_edison\_sample application needs a valid device connection string. The connection string for iotdm\_edison\_sample is defined in the file **azure-iot-sdks/c/iotdm\_client/samples/iotdm\_edison\_sample/iotdm\_edison\_sample.c**.

    Replace the connection string between the double quotes in the code with your device's connection string:

    ```
    static const char* connectionString = "[device connection string]";
    ```

4.  Now you are ready to build. Type the following commands:

    ```
    cd ~/azure-iot-sdks/c/build_all/linux
    ./build.sh  --skip-unittests --no-mqtt --no-amqp 
    ```

5.  Run the sample

    The iotdm\_edison\_sample application needs a valid device connection string. If you have configured it in source code, then you are good to go. Otherwise you need to pass the device connection string as a command line parameter to the iotdm\_edison\_sample application as shown below:

    ```
    cd ~/cmake/iotdm_client/samples/iotdm_edison_sample/
    ./iotdm_edison_sample "[device connection string]"
    ```

    Note the double quotes around the connection string are required. If you don't use them the iotdm\_edison\_sample application will not start properly.

    To rebuild an individual component, you can run *make* in ~/cmake/path/to/component you want to rebuild. To rebuild iotdm_simple_sample after changing the connection string simply type:

    ```
    make
    ```    
    When the iotdm\_simple\_sample runs, the device is ready to receive and process requests from the IoT Hub service. Notice that when the device connects to IoT Hub, the service will automatically start to observe resources on the device. The device libraries will invoke the device callbacks to retrieve the latest values from the device and provide them to the service.


## Running a firmware update job on the Intel Edison

1.  Create an image using the instructions in [How to create a firmware image for Intel Edison](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/c/iotdm_client/samples/iotdm_edison_sample/how_to_create_an_image_for_Intel_Edison.md).

2.  Transfer your new image “edison.zip” to your Edison device. Place it in the folder /home/root/.

3.  In your /home/root/ folder create a file called "**.cs**" and add your device connection string to it. Save the file.

4.  Run the iotdm\_edison\_sample application you created earlier:

    ```
    cd ~/cmake/iotdm_client/samples/iotdm_edison_sample/
    ./iotdm_edison_sample
    ```    

5. To submit the firmware update job and monitor its progress, use the Node.js [device management sample UI](https://acom-sandbox.azurewebsites.net/en-us/documentation/articles/iot-hub-device-management-ui-sample/). You can run this sample on either Windows or Linux and it requires Node.js 6.1.0 or greater. As the packageURI parameter use "file:////home/root/edison.zip" (note the 4 backslashes is intentional)

    After the firmware has been applied and the machine reboots, the iotdm\_edison\_sample application will automatically run as a service and reconnect to Azure IoT Hub. To verify that the iotdm\_edison\_sample is running in the background type the following command:

    ```
    ps | grep iotdm
    ```

[Setup your IoT hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[Provision your device and get its credentials]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
