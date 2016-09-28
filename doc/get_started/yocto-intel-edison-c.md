---
platform: yocto
device: intel-edison
language: c
---

# Run a simple C sample on Intel Edison

The following describes how to build and run the **simplesample_amqp** application on an Intel Edison.

## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Manage IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

## Preparing the Intel Edison board

- Ensure the latest OS image has been installed. To do that, follow these instructions on Intel's official documentation [portal][IntelEdisonGettingStarted].
- After following the standard Intel Edison setup instructions, you will need to establish a [serial connection][IntelEdisonSerialConnection] to your device from your machine.
- Once you have established a serial connection (command line) to your Intel Edison board, you can proceed to install Azure IoT SDK using the below instructions.
- Make sure you have run the "configure_edison —setup" command to set up your board
- Make sure your Intel Edison is online via your local Wi-Fi network (should occur during configure_edison setup)

## Installing nodejs on your Intel Edison

Intel Edison's build of [Yocto Linux][yocto] uses the "opkg" manager which doesn't, by default, include nodejs support. We must first add extended packages to Intel Edison which include node.js support.

- First, on your Intel Edison command line, use vi to add the following to your base-feeds:

	```
    $ vi /etc/opkg/base-feeds.conf
    ```

- Add the following lines to the base-feeds.conf file:

  ```
  src/gz all http://repo.opkg.net/edison/repo/all
  src/gz edison http://repo.opkg.net/edison/repo/edison/
  src/gz core2-32 http://repo.opkg.net/edison/repo/core2-32/
  ```

- Exit by hitting "esc", "SHIFT+:" and typing "wq" then hitting Enter. If you are unfamiliar with vi editing, read [this][vi].

- Next, update and upgrade your opkg base-feeds on your command line:

    ```
    $ opkg update
    $ opkg install nodejs
   
    ```

You should see the following:

![][img1]

## Downloading the Azure IoT SDK to your Intel Edison

The Azure IoT SDK relies on Git submodules, which is not built into the default Git package.  We recommend cloning the repository to your PC / Mac / Linux desktop and then transfer the files over the network to your Intel Edison using [FileZilla][filezilla], SCP, or rsync. 

We recommend using the default folder located in /home/root to keep the repository.

- First, run "wpa_cli status" on your Intel Edison to find your IP address.

  - For rsync, the Intel Edison needs rsync to be installed.
  
    ```
    opkg install rsync
    ```
    
    Then, on the desktop, cd to the folder containing the repository and run rsync:

    ```
    rsync -azP --exclude=.vscode --exclude=.git ./azure-iot-sdks/ root@<<edison ip>>:/home/root/azure-iot-sdks
    ```

  - For scp, cd to the folder containing the repository and run scp:

    ```
    scp -Cvr ./azure-iot-sdks/ root@<<edison ip>>:/home/root/azure-iot-sdks/
    ```

  - For FileZilla, run "wpa_cli status" on your Intel Edison to find your IP address, then use "sftp://your.ip.address", use password "root" and your Intel Edison password to establish an SFTP connection via FileZilla. Once you have done that, you can drag and drop files over the network directly.

	![][img2]

### Using Git to download Azure IoT SDK

The default installation of git does not support submodules, but if you install a submodule enabled version of git, or [build git from source][gitIntallFromSource], you may use Git and clone the Azure SDK repository directly to your Edison using the following commands. :

  ```
  $ git clone --recursive https://github.com/Azure/azure-iot-sdks.git
  ```

- You may be prompted to add an RSA key to your device, respond with "yes".


## Building the Azure IoT SDK and running a simple sample on Intel Edison

We will build a sample application which relies on the SDK.
We first need to update the credentials in the sample AMPQ app to match those of our Azure IoT Hub application. When we build the Azure IoT SDK, the sample C applications are automatically built by default, we need to include our credentials into the sample app while we build the SDK so that they are ready to function after we build.

- Edit "/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c" as given below using vi or your favorite text editor:
 - Replace the "IoT Hub connection" aka "connectionString" string placeholder with your info as below (static const char* ….) When you are finished the result should look like the below connection string with your own credentials instead of the placeholders in brackets.
  **IMPORTANT**: Replace items in brackets with your credentials or the sample will not function
 
            static const char* connectionString = "HostName=[YOUR-HOST-NAME];CredentialScope=Device;DeviceId=[YOUR-DEVICE-ID];SharedAccessKey=[YOUR-ACCESS-KEY";
  
- Azure IoT SDKs require cmake 3.0 or higher. Follow the steps below to install cmake 3.4
  
        $ wget https://cmake.org/files/v3.4/cmake-3.4.0-Linux-i386.sh  
        $ chmod u+x cmake-3.4.0-Linux-i386.sh  
        $ ./cmake-3.4.0-Linux-i386.sh --prefix=/usr
  
  **Note**:--prefix=/usr will replace your current cmake with the latest one.
   
- Use following steps to build Azure IoT SDK
   
    - Edit build.sh script to get rid of the command nproc
   
          cd azure-iot-sdks/c/build_all/linux
          vi ./build.sh
    
    - Change line "make --jobs=nproc" to "make --jobs=2"

    - Build the Azure IoT sdk
   
            cd ~/azure-iot-sdks/c/build_all/linux
            ./build.sh --no-mqtt 
    
- Navigate to: ~/azure-iot-sdks/c/cmake/iotsdk_linux/iothub_client/samples/iothub_client_sample_amqp/ and run the following commands
  
        ./iothub_client_sample_amqp
    
    The result should be the following:

    ```
    Starting the IoTHub client sample AMQP...
    Info: IoT Hub SDK for C, version 1.0.0
    IoTHubClient_SetMessageCallback...successful.
    IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.
    IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.
    IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.
    IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.
    IoTHubClient_SendEventAsync accepted data for transmission to IoT Hub.
    Press any key to exit the application.
    Confirmation[0] received for message tracking id = 0 with result = IOTHUB_CLIENT_CONFIRMATION_OK
    Confirmation[1] received for message tracking id = 1 with result = IOTHUB_CLIENT_CONFIRMATION_OK
    Confirmation[2] received for message tracking id = 2 with result = IOTHUB_CLIENT_CONFIRMATION_OK
    Confirmation[3] received for message tracking id = 3 with result = IOTHUB_CLIENT_CONFIRMATION_OK
    Confirmation[4] received for message tracking id = 4 with result = IOTHUB_CLIENT_CONFIRMATION_OK
    ```


[IntelEdisonGettingStarted]: https://software.intel.com/iot/library/edison-getting-started
[IntelEdisonSerialConnection]: https://software.intel.com/setting-up-serial-terminal-intel-edison-board
[yocto]: http://www.yoctoproject.org/docs/latest/adt-manual/adt-manual.html
[vi]: https://www.cs.colostate.edu/helpdocs/vi.html
[filezilla]: https://filezilla-project.org
[ldcconfig]: http://codeyarns.com/2014/01/14/how-to-add-library-directory-to-ldconfig-cache/

[img1]: ./media/edison01.png
[img2]: ./media/edison02.png

[setup-iothub]: ../setup_iothub.md
[provision-device]: ../manage_iot_hub.md
