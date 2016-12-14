---
platform: ubuntu 14.04lts
device: cps-mc341-adsc2
language: c
---

Run a simple C sample on CPS-MC341-ADSC2 device running Ubuntu 14.04LTS
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

This document describes how to connect CPS-MC341-ADSC2 device running  ubuntu14.04LTS with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

-  Required items for development.
    - CONPROSYS(CPS-MC341-ADSC2)
    - SDHC card (4Gbyte or larger, non-SDXC-compliant)
    - Cable for serial monitor (Recommended cable: TTL-232R-3V3 AJ by FTDI)
    - LAN Cable
    - PC

-  Creating a booting SD card for CONPROSYS.
    - Preparation for the SD card image file.
    Download the SD image file specific to the CONPROSYS model from the [CONTEC website](<http://www.contec.co.jp/product/m2m/software.html>)
    - Write the image file into the SD card.
    Write the image file that downloaded into the SD card.
    The downloaded image file is a zip file. Decompress it into img file before start writing it in the SD card.

-  By connecting a serial cable from host PC to CONPROSYS through the serial port (Diameter 3.5 MINI-JACK), you can log in from the console to the product.
See the setting details below.
    - Baud rate:115200 bps
    - Data bit:8bit
    - Parity:none
    - Stop bit:1bit
    - Hardware flow:none

- We recommend the following serial cable for connecting the host PC to CONPROSYS.Download appropriate driver software to match OS of PC serial monitoring.
    - TTL-232R-3V3 AJ by FTDI (included when purchasing the package version)
    - URL for driver can be found [here](<http://www.ftdichip.com/Drivers/VCP.htm>)
    -  Default log-in and password are
        - Log-in:conprosys
        - Password:contec

- Create a new partition in the blank area of the memory.
    1) Start the CONPROSYS.
    2) Open the SD card device by fdisk to add a new Linux partition.
    
        sudo fdisk /dev/mmcblk0
    
    i) Add a new Linux partition by n command.
    Parameter is set as follows:
        <Parameter>
        Command (m for help): <n>
        Partition type: <p>
        Partition number : <3>
        First sector: <default value>
        Last sector: <default value>

    ii) Write the partition information by w command.
    Command (m for help): w

    3) Reboot CONPROSYS with the command below.

        sudo reboot

    4) Confirm the device of /dev/mmcblkk0p3 is available. Format the new created partition.

        sudo mkfs -t ext3 /dev/mmcblk0p3

    5) Create the destination for the new partition and mount it.
    An example: mounting directory /mnt/ext_mmc
    
        sudo mkdir /mnt/ext_mmc

        sudo mount /dev/mmcblk0p3 /mnt/ext_mmc

    6) Add the setting in /etc/fstab to mount automatically upon booting for the next time succeeding
     An example: mounting directory /mnt/ext_mmc

         /dev/mmcblk0p3 /mnt/ext_mmc ext3 defaults 0 0

- Expand the rootfs partition (ext3) of Linux to the maximum memory size in the SD card.
    1) Start the CONPROSYS.
    2) Open the SD card device by fdisk to change a Linux partition.

        sudo fdisk /dev/mmcblk0

    i) Display the present partition information by p command.
    Take a note of the information displayed.
    *Especially the Start / End address of /dev/mmcblk0p2.
    [A display example: a SD card with 4GByte is run by Ubuntu14.04]
        Command (m for help): p
        Disk /dev/mmcblk0: 7746 MB, 7746879488 bytes
        255 heads, 63 sectors/track, 941 cylinders, total 15130624 sectors
        Units = sectors of 1 * 512 = 512 bytes
        Sector size (logical/physical): 512 bytes / 512 bytes
        I/O size (minimum/optimal): 512 bytes / 512 bytes
        Disk identifier: 0x00000000
        Device Boot Start End Blocks Id System
        /dev/mmcblk0p1 * 63 144584 72261 c W95 FAT32 (LBA)
        /dev/mmcblk0p2 144585 7855784 3847567+ 83 Linux

    ii) Delete Linux partition (Partition number: 2) by d command.
        Command (m for help): d
        Partition number (1-4): 2

    iii) Add the Linux partition by n command.
    For this, set the parameter as described below.
    <Parameter>
        Command (m for help): n
        Partition type: p
        Partition number : 2
        First sector: <default value> (the Start address of /dev/mmcblk0p2 which you took a note by p command)
        Last sector: <default value > (the End address of /dev/mmcblk0p2 which you took a note by p command)

    iv) Display the changed partition information by p command.
    Check whether the End address and Blocks of /dev/mmcblk0p2 are changed.
    [A display example: a SD card with 4GByte is run by Ubuntu14.04]
        Command (m for help): p
        Disk /dev/mmcblk0: 7746 MB, 7746879488 bytes
        255 heads, 63 sectors/track, 941 cylinders, total 15130624 sectors
        Units = sectors of 1 * 512 = 512 bytes
        Sector size (logical/physical): 512 bytes / 512 bytes
        I/O size (minimum/optimal): 512 bytes / 512 bytes
        Disk identifier: 0x00000000
        Device Boot Start End Blocks Id System
        /dev/mmcblk0p1 * 63 144584 72261 c W95 FAT32 (LBA)
        /dev/mmcblk0p2 144585 15130623 7484987 83 Linux

    v) Write the partition information by w command.

        Command (m for help): w

    3) Reboot CONPROSYS with the command below.

        sudo reboot

    4) Resize the partition that was changed by resize2fs command.

        sudo resize2fs /dev/mmcblk0p2

    *It may take quite a long time to complete.
    (several to several dozen minutes depending on the memory size)
    After resize2fs command completes, confirm whether the memory size is expanded by df command.

        df

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   CPS-MC341-ADSC2 device.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Default network setting is listed below:
      -   LAN A (eth0):  10.1.1.101
      -   LAN B (eth1):  192.168.1.101    (for 2 LAN Type setting only)

- If you would like to change the network setting, edit the file on target at location **etc/network/interfaces**

        Example)  Set "eth0" to "dhcp".
        auto eth0
        iface eth0 inet dhcp

-   On the board, system reboot.

-   Set the time synchronization using the ntpdate command.

-   Update the apt-get

	sudo apt-get update

-   Install the uuid-dev

        sudo apt-get install uuid-dev

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

	git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using vi editor:

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for device connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Edit the following file using vi editor:

        azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

        IOTHUB_CONNECTION_STRING=<HostName=...>
        IOTHUB_DEVICE_ID=
        IOTHUB_DEVICE_KEY=
        IOTHUB_EVENTHUB_CONNECTION_STRING=<Endpoint=...>
        IOTHUB_EVENTHUB_CONSUMER_GROUP=$Default
        IOTHUB_EVENTHUB_LISTEN_NAME=
        IOTHUB_SHARED_ACCESS_SIGNATURE=<SharedAccessSignature sr=...>
        IOTHUB_PARTITION_COUNT=16
        IOTHUB_PROTOCOL_GATEWAY_HOSTNAME=

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   On the board, run the following command to change params:

        cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        sudo chmod +x setiotdeviceparametersfore2etests.sh
        sudo ./setiotdeviceparametersfore2etests.sh

-   On the board, system reboot.

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Prerequisites) and save the changes.

-   Set the time synchronization using the ntpdate command.

-   Edit the following file using vi editor:

        azure-iot-sdks/c/build_all/linux/build.sh

-   Find the following place holder:

        CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)

-   Edit as follows, and then save the changes:

        CORES=1

-   Edit the following file using vi editor:

        azure-iot-sdks/c/azure-c-shared-utility/tests/condition_unittests/CMakeLists.txt

-   Add, and then save the changes:

        Set(CMAKE_CXX_FLAGS "-lrt")

-   Swap memory setting
    CONPROSYS memory may not be enough for building large source code. You can expand the memory by creating a swap file on the disk.
An example: Creating 1Gbyte of SWAP memory
    1) Create a SWAP file.

        dd if=/dev/zero of=/home/swapfile bs=1024 count=1024000

        mkswap /home/swapfile

    2) Set the SWAP file.

        sudo swapon /home/swapfile

    Once you create the SWAP file, you only need to set the file after next time.

-   Build the SDK samples using the following command:

        ./azure-iot-sdks/c/build_all/linux/build.sh 

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

	~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http


-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

## 3.3 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.



The following is described for past compatibility.
Please use Ubuntu 14.04LTS for new use.
===
---
Run a simple C sample on CPS-MC341-ADSC2 device running Ubuntu 10.04LTS
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

This document describes how to connect CPS-MC341-ADSC2 device running  ubuntu10.04LTS with Azure IoT SDK. This multi-step process includes:
-   Configuring Azure IoT Hub
-   Registering your IoT device
-   Build and deploy Azure IoT SDK on device

<a name="Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:

  -  By connecting a serial cable from host PC to CONPROSYS through the serial port (Diameter 3.5 MINI-JACK), you can log in from the console to the product.
See the setting details below.
     - Baud rate:115200 bps
     - Data bit:8bit
     - Parity:none
     - Stop bit:1bit
     - Hardware flow:none

- We recommend the following serial cable for connecting the host PC to CONPROSYS.Download appropriate driver software to match OS of PC serial monitoring.
    - TTL-232R-3V3 AJ by FTDI (included when purchasing the package version)
    - URL for driver can be found [here](<http://www.ftdichip.com/Drivers/VCP.htm>)
    -  Default log-in and password are
        - Log-in:conprosys
        - Password:contec

-   Computer with Git client installed and access to the
    [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub
    public repository.
-   CPS-MC341-ADSC2 device.
-   Download and install [DeviceExplorer](https://github.com/Azure/azure-iot-sdks/releases/download/2015-11-13/SetupDeviceExplorer.msi).
-   [Set up your IoT hub](https://github.com/Azure/azure-iot-sdks/blob/master/doc/setup_iothub.md).
#### Create a device on IoT Hub
-   With your IoT hub configured and running in Azure, follow the instructions in **"Create Device"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).
#### Write down device credentials
-   Make note of the Connection String for your device by following the instructions in **"Get device connection string or configuration data"** section of [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md).

<a name="PrepareDevice"></a>
# Step 2: Prepare your Device
-   Default network setting is listed below:
      -   LAN A (eth0):  10.1.1.101
      -   LAN B (eth1):  192.168.1.101    (for 2 LAN Type setting only)

- If you would like to change the network setting, edit the file on target at location **etc/network/interfaces**

        Example)  Set "eth0" to "dhcp".
        auto eth0
        iface eth0 inet dhcp

-   On the board, system reboot.

-   Set the time synchronization using the ntpdate command.

-   Update the apt-get

	sudo apt-get update

-   Install the uuid-dev

        sudo apt-get install uuid-dev

<a name="Build"></a>
# Step 3: Build and Run the sample

<a name="Load"></a>
## 3.1 Build SDK and sample

-   Download the Microsoft Azure IoT Device SDK for C to the board by issuing the following command on the board::

	git clone --recursive https://github.com/Azure/azure-iot-sdks.git

-   Edit the following file using vi editor:

    {{**For AMQP protocol:**}}

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c

    {{**For HTTPS protocol:**}}

        azure-iot-sdks/c/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http.c

-   Find the following place holder for device connection string:

        static const char* connectionString = "[device connection string]";

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Edit the following file using vi editor:

        azure-iot-sdks/tools/iot_hub_e2e_tests_params/iot_device_params.txt

        IOTHUB_CONNECTION_STRING=<HostName=...>
        IOTHUB_DEVICE_ID=
        IOTHUB_DEVICE_KEY=
        IOTHUB_EVENTHUB_CONNECTION_STRING=<Endpoint=...>
        IOTHUB_EVENTHUB_CONSUMER_GROUP=$Default
        IOTHUB_EVENTHUB_LISTEN_NAME=
        IOTHUB_SHARED_ACCESS_SIGNATURE=<SharedAccessSignature sr=...>
        IOTHUB_PARTITION_COUNT=16
        IOTHUB_PROTOCOL_GATEWAY_HOSTNAME=

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   On the board, run the following command to change params:

        cd ./azure-iot-sdks/tools/iot_hub_e2e_tests_params/
        sudo chmod +x setiotdeviceparametersfore2etests.sh
        sudo ./setiotdeviceparametersfore2etests.sh

-   On the board, system reboot.

-   Replace the above placeholder with device connection string you obtained in [Step 1](#Step-1:-Prerequisites) and save the changes.

-   Set the time synchronization using the ntpdate command.

-   Edit the following file using vi editor:

        azure-iot-sdks/c/build_all/linux/build.sh

-   Find the following place holder:

        CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)

-   Edit as follows, and then save the changes:

        CORES=1

-   Edit the following file using vi editor:

        azure-iot-sdks/c/azure-c-shared-utility/tests/condition_unittests/CMakeLists.txt

-   Add, and then save the changes:

        Set(CMAKE_CXX_FLAGS "-lrt")

-   Build the SDK samples using the following command:

        sudo -E ./azure-iot-sdks/c/build_all/linux/build.sh 

## 3.2 Send Device Events to IoT Hub:

-   Run the sample by issuing following command:

    {{**If using AMQP protocol:**}}

	~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp

    {{**If using HTTPS protocol:**}}

	~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_http/iothub_client_sample_http


-   On Windows, refer "Monitor device-to-cloud events" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) to see the data your device is sending.

## 3.3 Receive messages from IoT Hub

-   On Windows, refer "Send cloud-to-device messages" in [DeviceExplorer Usage document](https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/doc/how_to_use_device_explorer.md) for instructions on sending messages to device.

