---
platform: digi embedded yocto 2.0
device: connectcore 6 ul starter kit
language: python
---

Run a simple PYTHON sample on ConnectCore 6 UL and ConnectCore 6 UL Starter board running Digi Embedded Yocto 2.0 (DEY2.0)
===
---

# Table of Contents

-   [Introduction](#Introduction)
-   [Step 1: Prerequisites](#Prerequisites)
-   [Step 2: Prepare your Device](#PrepareDevice)
-   [Step 3: Build and Run the Sample](#Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build ConnectCore 6 UL and Starter board running DEY2.0 with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-   [ConnectCore 6 UL Starter board and development environment][setup-starter-board]
-   [ConnectCore 6 UL Starter board hardware setup][setup-hardware]
-   [Prepare your development environment][setup-devbox-python]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]
-   Empty microSD card 8GB at least.

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device

You should have following steps to prepare the device.

## 2.1 Intall DEY2.0 into Linux Host PC

-   [Here][setup-dey2.0] is instruction how to install DEY2.0 and setup Linux Host PC.

## 2.2 Create projects and build images

-   [Here][create-dey2.0-image] is instruction to create projects and build images.

## 2.3 Customize root file system

-   DEY allows you to configure network default configuration for your customized image.
-   Dynamic IP or Static IP address can be configured. [Here][customize-network-setting] is instruction how to configure network setting.

-   Also DEY allows you to add features and packages to your target image.
-   Use the syntax below and add features to your **conf/local.conf** file.

        EXTRA_IMAGE_FEATURES = "debug-tweaks tools-debug tools-sdk dev-pkgs"
        IMAGE_INSTALL_append = " strace cmake boost"
        PACKAGECONFIG_pn_boost = " python"

-   Note that the space must be included before the first word in IMAGE_INSTALL_append and PACKAGECONFIG_pn_boost section. Then build new image by following command.

        bitake core-image-base

-   New image will be created in **tmp/deploy/images/ccimx6ulstarter** directory.
-   Specifically, following 3 files are symbolic link files. You can find actual files to trace link path.

        core-image-base-ccimx6ulstarter.rootfs.tar.bz2 -- root file system
        zImage-ccimx6ulstarter.bin
        zImage-imx6ul-ccimx6ulstarter-wb.dtb  -- device tree image


## 2.4 Format and partition microSD card

-   8GB size is appropriate for root file system area and swap srea.
-   Firstly, format microSD card in ext4 and make 3 partitions at least.
-   Following is example of partition in microSD card. (Block size is approximte.)

        Device          Blocks  Id  System
        /dev/mmcblk0p1  32KB    b   W95 FAT32
        /dev/mmcblk0p2  4GB     83  Linux
        /dev/mmcblk0p3  4GB     82  Linux Swap


## 2.5 Install root file system built in DEY2.0

-   Copy root file system into **/dev/mmcblk0p2** "Linux" parition.
-   Other 2 files are copied into **/dev/mmcblk0p1** "W32 FAT" partition.

-   Insert microSD card to socket in ConnectCore 6 UL Starter board.
-   USB cable is connected between Linux Host PC and the board.
-   Turn on the board then messages are shown through console over USB.
-   Hit any key immediately to stop autoboot. Then arrow prompt can be seen.

        Hit any key to stop autoboot:  0
        =>

## 2.6 Modify boot parameters to boot the OS from microSD card

-   Execute following command sets to modify boot parameters.

        setenv mmcdev 0
        setenv mmcpart 1
        setenv mmcroot /dev/mmcblk1p2
        setenv bootargs console=${console},${baudrare} root=${mmcroot}
        fatload mmc 0:1 ${fdt_addr} zImage-imx6ul-ccimx6ulstarter-wb.dtb
        fatload mmc 0:1 ${loadaddr} zImage-ccimx6ulstarter.bin
        bootz ${loadaddr] - ${fdt_addr}
        saveenv
        reset

-   Turn on the board and messages are shown through console over USB.
-   Open and configure minicom or serial console terminal then check messages.

        Digi Embedded Yocto 2.0-r3 ccimx6ulstarter /dev/ttymxc4

        ccimx6ulstarter login:

-   You can login by your username and passowrd if you configured.

-   Before building SDK, swap must be enabled.


## 2.6 Confirm software version

Check software version. **cmake** version is 3.x or higher. **gcc** version is 4.9 or higher. And **python** is 2.7.x.

        cmake --version
        gcc --version
        python --version


<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Install SDK

-   Remove microSD card from ConnectCore 6 UL Starter board then insert microSD card to Linux Host PC. Mount microSD card to be able to access.

        sudo mount /dev/mmcblk0p2 /media

-   Download Azure IoT Device SDK for Python by following commands.

        git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Archive and copy into microSD card by following commands.

        tar cvf ./azure-iot.sdks.tar ./azure-iot.sdks
        sudo cp ./azure-iot.sdks.tar /media/home/root/.

-   Unmount microSD card and remove from Linux Host PC.

        sudo umount /media

-   Insert microSD card into ConnectCore 6 UL Starter board.
-   Connect USB cable to the board then it turns on.
-   Login and move directory /home/root then extract `azure-iot-sdks.tar` file.

        cd /home/root
        tar xvf azure-iot-sdks.tar


## 3.1 Build SDK and sample

-   Run following commands to build the SDK:

        cd azure-iot.sdks/python/build_all/linux
        ./build.sh    

-   After a successful build, the `iothub_client.so` Python extension module is copied to the **python/device/samples** folder.

- Navigate to samples folder by executing following command:

        cd azure-iot-sdks/python/device/samples/

-   Edit the following file using `vi`text editor:

        vi iothub_client_sample.py

-   Find the following place holder for device connection string:

        connectionString = "[device connection string]"

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

## 3.2 Send Device Events to IoT Hub:

-   Run the sample application using the following command:

    **For AMQP protocol:**

        python iothub_client_sample.py -p amqp

    **For HTTP protocol:**

        python iothub_client_sample.py -p http

    **For MQTT protocol:**

        python iothub_client_sample.py -p mqtt

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the application.

## 3.3 Receive messages from IoT Hub

-   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to send cloud-to-device messages to the application.

[setup-starter-board]: http://www.digi.com/resources/documentation/digidocs/90001514/default.htm#reference/yocto/r_requirements_yocto.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CGet%2520started%7C_____1
[setup-hardware]: http://www.digi.com/resources/documentation/digidocs/90001514/default.htm#task/yocto/t_set_up_hardware_yocto.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CGet%2520started%7C_____2
[setup-dey2.0]: http://www.digi.com/resources/documentation/digidocs/90001514/default.htm#task/yocto/t_install_dey.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CSystem%2520development%7CFirst%2520steps%7C_____1
[create-dey2.0-image]: http://www.digi.com/resources/documentation/digidocs/90001514/default.htm#task/yocto/t_create_build_projects_yocto.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CSystem%2520development%7CFirst%2520steps%7C_____2
[customize-network-setting]: http://www.digi.com/resources/documentation/digidocs/90001514/default.htm#task/yocto/t_customize_root_file_system_yocto.htm%3FTocPath%3DDigi%2520Embedded%2520Yocto%7CSystem%2520development%7CFirst%2520steps%7C_____3
[setup-devbox-python]: https://github.com/Azure/azure-iot-sdk-python/blob/master/doc/python-devbox-setup.md
[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md
