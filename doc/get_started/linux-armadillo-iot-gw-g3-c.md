---
platform: debian
device: armadillo-iot g3
language: c
---

Run a simple C sample on Armadillo-IoT G3 device running Debian jessie
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)
-   [Tips](#tips)

# Instructions for using this template

-   Replace the text in {placeholders} with correct values.
-   Delete the lines {{enclosed}} after following the instructions enclosed between them.
-   It is advisable to use external links, wherever possible.
-   Remove this section from final document.

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to connect Armadillo-IoT G3 device running Debian jessie with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [Prepare your development environment](#Prepare_dev_env)
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Armadillo-IoT G3 device.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Follow the instruction described [here](http://manual.atmark-techno.com/armadillo-iot/armadillo-iotg-std_startup_guide_ja-2.3.4/) to setup the device.

-   Install packages which necessary to running the IoT SDK samples:

    -   first, make the default user to sudoer

        ```
        $ su
        # apt-get update
        # vigr
        ...add 'atmark' to sudoer group
        # exit
        $ exit
        ```

    -   Then, install other packages via sudo

            sudo apt-get install -y uuid cmake pkgconf openssh-server

    -   Create the home directory of the default user if it not exists:

        ```
        sudo mkdir /home/atmark
        sudo chown /home/atmark
        sudo chgrp /home/atmark
        ```

    -   Prepare the directory for [the next step](#Load):

        ```
        cd
        mkdir work
        cd work
        mkdir IoT_SDK
        cd IoT_SDK
        ```

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample with ATDE
***In this section, you build the SDK and sample with the ATDE on the host PC, not on the device*** (You can build them on the device, but it may need slightly long time.)

-   Install the prerequisite packages for the Microsoft Azure IoT Device SDK for C by issuing the commands from the command line on the ATDE which given [here](#Prepare_dev_env).
    
    Then, issuing the following commands:

    ```
    cd ~/work
    mkdir IoT_SDK
    cd IoT_SDK
    ```

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the ATDE:

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit/Create the following file using any text editor of your choice (this file is necessary to cross build the SDK):

		azure-iot-sdks/c/build_all/linux/toolchain-armadillo.cmake

-   Insert the following lines to the file:

    ```
    INCLUDE(CMakeForceCompiler)
    SET(CMAKE_SYSTEM_NAME Linux)
    SET(CMAKE_SYSTEM_VERSION 1)
    SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-4.9)
    SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-4.9)
    ```

-   Edit the following file using any text editor of your choice:

    **For AMQP protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    **For HTTPS protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

    **For MQTT protocol:**

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt.c

-   Find the following place holder for IoT connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Build the SDK using following command.
    
    ***Note:*** *(**This is cross building**, so after build is complete, all test cases will fail since it cannot execute the ARM binary on the x86 PC)*

        sudo azure-iot-sdks/c/build_all/linux/build.sh -cl -fsigned-char --toolchain-file azure-iot-sdks/c/build_all/linux/toolchain-armadillo.cmake --run-e2e-tests

    You should remember the following two options:
    -   -cl -fsigned-char: without this option, one of the test case in unittests will fail
    -   --toolchain-file: for the cross build

### 3.1.1 Copy the built image to the device:

-   At first, you should modify the build.sh to keep the already built binaries (edit the file using any text editor of your choice):

        azure-iot-sdks/c/build_all/linux/build.sh

    -   Comment out the line 95 and 96:

            #rm -r -f ~/cmake
            #mkdir ~/cmake

-   Then, archive the two directories (azure-iot-sdks/ and cmake/):

    ```
    $ tar cvf azure-iot-sdks.tar ./azure-iot-sdks
    $ su
    # cd
    # tar cvf ~atmark/work/IoT_SDK/cmake.tar ./cmake
    # exit
    ```

-   Remote copy above two archives to the device via scp:

        scp *.tar azure-iot-sdks.tar atmark@<IP addr of the device>:~/work/IoT_SDK/

-   When scp is done, perform following steps **on the device**.

-   Extract the archive and run the build.sh to execute SDK's test cases.

    ```
    $ cd ~/work/IoT_SDK
    $ tar xvf ./azure-iot-sdks.tar
    $ su
    # cd
    # tar xvf ~atmark/work/IoT_SDK/cmake.tar
    # exit
    ```

-   Set environment variables by populating **IOT_DEVICE_PARAMS.TXT**.

        nano azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

-   Set environment variables by running following command on your device:

        cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        chmod +x setiotdeviceparametersfore2etests.sh
        sudo ./setiotdeviceparametersfore2etests.sh

-   Restart the device.

-  	Run the build.sh to execute SDK's test cases.

    ```
    $ cd ~/work/IoT_SDK
    $ su
    # ./azure-iot-sdks/c/build_all/linux/build.sh -cl -fsigned-char --run-e2e-tests | tee LogFile.txt
    ```

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    **If using AMQP protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amq

    **If using HTTPS protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_httpttp

    **If using MQTT protocol:**

        azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_mqtt/iothub_client_sample_mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

<a name="tips"></a>
# Tips

-   If you just want to build the serializer samples, run the following commands:

    ```
    cd ./c/serializer/build/linux
    make -f makefile.linux all
    ```

[setup-devbox-linux]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md


<a name="Prepare_dev_env"></a>
## Prepare your development environment

-   Download the ATDE v5 (Atmark Techno Development Environment v5) from here: [http://armadillo.atmark-techno.com/atde](http://armadillo.atmark-techno.com/atde)

-   Open the .vmx image file in the downloaded archve with VMware.

-   When after ATDE 5 is booted, issue following commands in console (for install gcc 4.9 and cmake 3.8):

        sudo apt-get install -y uuid uuid-dev
        sudo cp /etc/apt/sources.list /etc/apt/sources.list.wheezy
        sudo sed -i 's/wheezy/jessie/g' /etc/apt/sources.list
        sudo apt-get update
        sudo apt-get install -y gcc cmake

-   Next, issue following commands in console (for install gcc 4.9 cross toolchain):

        mkdir work
        cd work
        mkdir pkgs
        cd pkgs
        wget http://launchpadlibrarian.net/191842424/libisl13_0.14-1_amd64.deb
        sudo dpkg -i libisl13_0.14-1_amd64.deb
        wget http://launchpadlibrarian.net/200825600/binutils-arm-linux-gnueabihf_2.25-5ubuntu3cross1.105_amd64.deb
        sudo dpkg -i binutils-arm-linux-gnueabihf_2.25-5ubuntu3cross1.105_amd64.deb
        wget http://launchpadlibrarian.net/200858209/gcc-4.9-arm-linux-gnueabihf-base_4.9.2-10ubuntu10cross0.7_amd64.deb
        sudo dpkg -i gcc-4.9-arm-linux-gnueabihf-base_4.9.2-10ubuntu10cross0.7_amd64.deb
        wget http://launchpadlibrarian.net/202006617/libc6-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        sudo dpkg -i libc6-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        wget http://launchpadlibrarian.net/200858223/libatomic1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libatomic1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858233/libgomp1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libgomp1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858221/libasan1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libasan1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858226/libgcc1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libgcc1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858263/libubsan0-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libubsan0-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858225/libgcc-4.9-dev-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libgcc-4.9-dev-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858206/cpp-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb
        sudo dpkg -i cpp-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb
        wget http://launchpadlibrarian.net/200858210/gcc-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb
        sudo dpkg -i gcc-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb
        wget http://launchpadlibrarian.net/202006617/libc6-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        sudo dpkg -i libc6-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        wget http://launchpadlibrarian.net/202006623/libc6-armel-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        sudo dpkg -i libc6-armel-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        wget http://launchpadlibrarian.net/200858244/libsfgcc1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libsfgcc1-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858256/libsfstdc++6-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libsfstdc++6-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858262/libstdc++6-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libstdc++6-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/202006619/libc6-dev-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        sudo dpkg -i libc6-dev-armhf-cross_2.21-0ubuntu4cross1.108_all.deb
        wget http://launchpadlibrarian.net/200858259/libstdc++-4.9-dev-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        sudo dpkg -i libstdc++-4.9-dev-armhf-cross_4.9.2-10ubuntu10cross0.7_all.deb
        wget http://launchpadlibrarian.net/200858207/g++-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb
        sudo dpkg -i g++-4.9-arm-linux-gnueabihf_4.9.2-10ubuntu10cross0.7_amd64.deb

-   Next, issue following commands in console (for install libcurl4-openssl-dev cross):

        sudo cp /etc/apt/sources.list.wheezy  /etc/apt/sources.list
        sudo apt-get update
        sudo apt-get download libcurl3:armhf
        sudo apt-get download libcurl4-openssl-dev:armhf
        sudo dpkg-cross --build --arch armhf -M libcurl3_7.26.0-1+wheezy13_armhf.deb
        sudo dpkg-cross --build --arch armhf -M libcurl4-openssl-dev_7.26.0-1+wheezy13_armhf.deb
        sudo dpkg -i libcurl3-armhf-cross_7.26.0-1+wheezy13_all.deb
        sudo dpkg -i --force-conflicts --force-overwrite libcurl4-openssl-dev-armhf-cross_7.26.0-1+wheezy13_all.deb
