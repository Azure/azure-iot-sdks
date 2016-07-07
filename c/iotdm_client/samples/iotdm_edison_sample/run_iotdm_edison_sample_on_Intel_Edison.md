# Run the `iotdm_edison_sample` on Intel Edison

This document describes how to build and run the `iotdm_edison_sample` application on an Intel Edison device.

## Prerequisites

You should also have the following items ready before beginning the process:

-   [Setup your IoT hub]
-   [Provision your device and get its credentials]


## Building and running iotdm\_simple\_sample on Intel Edison

1.  Flash an image to your Edison

  - Follow the [instructions][1] provided by Intel
  - Environment modifications (*necessary for `edison-image-20160606` image*)
  
    > Edit `/etc/opkg/iotkit.conf` to fix `opkg` ([*related information*][2])
    >  - update: `src iotkit http://iotdk.intel.com/repos/3.5/intelgalactic/opkg/i586`
    >    - to: `src iotkit http://iotdk.intel.com/repos/3.0/intelgalactic/opkg/i586`
  - Update the system
  
    ```
    $ opkg update
    $ opkg upgrade
    ```
  - Connect via ssh
  
    ```
    $ ssh root@<ip_address>
    ```

2.  Clone the latest version of the azure-iot-sdks repository to your Edison

    ```
    $ git clone https://github.com/Azure/azure-iot-sdks.git ~/azure-iot-sdks --branch dmpreview
    ```

    Note: **git** on Edison does not support the **submodule** command. You need to process the submodules manually. To do that follow the steps below:

    ```
    $ cd ~/azure-iot-sdks
    $ git checkout 3292dcc0081a40ca14a30912f33df1f02b444fd3
    $ pushd c/azure-c-shared-utility
    $ git clone https://github.com/Azure/azure-c-shared-utility.git .
    $ git checkout 0df665aaf2b12f36fb9807ff6c1c8f35a8a676dc
    $ popd
    $ pushd c/parson
    $ git clone https://github.com/kgabis/parson.git .
    $ git checkout 21c03640485c95e168dbad26bfed3f87b3804041
    $ popd
    $ pushd c/wakaama
    $ git clone https://github.com/mhshami01/wakaama.git .
    $ git checkout 8ef361e8e72c45d87028809c9f114e1a9dd406c8
    $ popd
    ```

3.  Configure the IoTHub connection in the source (*optional*)

    The `iotdm_edison_sample` application needs a valid **device connection string**.

    Replace the connection string between the double quotes in the code with your device's connection string:
    ```
    $ vi ~/azure-iot-sdks/c/iotdm_client/samples/iotdm_edison_sample/iotdm_edison_sample.c
    ```
    Edit line 34:
    ```
    static const char* connectionString = NULL;
    ```

4.  Now you are ready to build using the following command:

    ```
    $ ~/azure-iot-sdks/c/build_all/linux/build.sh --skip-unittests --no-amqp --no-mqtt
    ```

5.  Run the sample

    The `iotdm_edison_sample` application needs a valid device connection string. If you have configured it in source code, then you are good to go. Otherwise you need to add it as parameter to the `iotdm_edison_sample` command line call as shown below:

    ```
    $ ~/azure-iot-sdks/c/cmake/iotsdk_linux/iotdm_client/samples/iotdm_edison_sample/iotdm_edison_sample ["<device connection string>"]
    ```

#### You're all set!
When the `iotdm_simple_sample` runs, the device is ready to receive and process requests from the IoT Hub service. Notice that when the device connects to IoT Hub, the service will automatically start to observe resources on the device. The device libraries will invoke the device callbacks to retrieve the latest values from the device and provide them to the service.


## Viewing results using the node.js cli tool `iothub-explorer`

1. Install `iothub-explorer`

    ```
    $ sudo npm install -g iothub-explorer@dmpreview
    ```
2. Test your connection with a deep read

    ```
    $ iothub-explorer "<connection-string>" read <device-id> SerialNumber
    ```
    > *NOTE: A deep read will make a request of your device via Azure, thus confirming an end-to-end connection*

> **WARNING: Not all commands available from `iothub-explorer` will work, due the build switches used in the previous steps (.i.e `--no-amqp` and `--no-mqtt`)**

## Running a firmware update job on the Intel Edison

1.  Create an image using the instructions in [How to create a firmware image for Intel Edison](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/c/iotdm_client/samples/iotdm_edison_sample/how_to_create_an_image_for_Intel_Edison.md).

2.  Transfer your new image `edison.zip` to your Edison device. Place it in the `/home/root/` (a.k.a. `~`) folder.
  
    ```
    $ scp edison.zip root@<ip_address>:~/edison.zip
    ```
3.  In the `/home/root/` (a.k.a. `~`) folder create a file called `.cs` and add your device connection string to it.
  
    ```
    $ echo '<device-connection-string>' >> ~/.cs
    ```
4.  Run the `iotdm_edison_sample` application you created earlier:

    ```
    $ ~/azure-iot-sdks/c/cmake/iotsdk_linux/iotdm_client/samples/iotdm_edison_sample/iotdm_edison_sample ["<device connection string>"]
    ```    
5. Apply the new image on your Edison device using `iothub-explorer`:

    ```
    $ iothub-explorer <connection-string> firmware-update <device-id> file:///home/root/edison.zip
    ```
    > *NOTE: The 3 forward-slashes are intentional*


[Setup your IoT hub]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md
[Provision your device and get its credentials]: https://github.com/Azure/azure-iot-sdks/blob/master/doc/manage_iot_hub.md
[1]: https://software.intel.com/en-us/iot/hardware/edison
[2]: https://communities.intel.com/thread/103298
