# Run the iotdm\_edison\_sample on Intel Edison

This document describes how to prepare your Linux development environment to use with the *Microsoft Azure IoT Hub device management library for C* and run the iotdm\_edison\_sample application on Intel Edison.

## Prerequisites

You should also have the following items ready before beginning the process:

-   [Setup your IoT hub]
-   [Provision your device and get its credentials]

This setup process requires **cmake** version 2.8.11 or higher and **gcc** version 4.9 or higher:

-   You can verify the current version of **cmake** installed in your environment using the `cmake --version` command.
-   You can verify the current version of **gcc** installed in your environment using the `gcc --version` command
-   If you run into issues installing **git** on an Edison device refer to the following article <https://github.com/w4ilun/edison-guides/wiki/Installing-Git-on-Intel-Edison>

## Building and running iotdm\_simple\_sample on Intel Edison

1.  Update packages on the Edison

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

    Note: **git** on Edison may not support the **submodule** command. In this case you see an error message for the above command. As a workaround you need to process the different submodules in a recursive way manually. In the **azure-iot-sdks** folder, execute the following command:

    ```
    find . –name .gitmodules
    ```

    Navigate to each directory with a **.gitmodules** file and **cat** the content of the file. For each section in the file, execute the following command:

    ```
    git clone <url> <path>
    ```

    Where &lt;url&gt; and &lt;path&gt; come from the actual section of the .gitmodule you are processing. When you have processed a section, check if there are more submodules to process by executing:

    ```
    find . –name .gitmodules
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
    ./build.sh
    ```

5.  Run the sample

    The iotdm\_edison\_sample application needs a valid device connection string. If you have configured it in source code, then you are good to go. Otherwise you need to add it as parameter to the iotdm\_edison\_sample command line call as shown below:

    ```
    cd ~/cmake/iotdm_client/samples/iotdm_edison_sample/
    ./iotdm\_edison\_sample "[device connection string]"
    ```

    Note to rebuild an individual component, you can run make in ~/cmake/path/to/component you want to rebuild. To rebuild iotdm_simple_sample after changing the connection string simply type:

    ```
    make
    ```    
    When the iotdm\_simple\_sample runs, the device is ready to receive and process requests from the IoT Hub service. Notice that when the device connects to IoT Hub, the service will automatically start to observe resources on the device. The device libraries will invoke the device callbacks to retrieve the latest values from the device and provide them to the service.

[Setup your IoT hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[Provision your device and get its credentials]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
