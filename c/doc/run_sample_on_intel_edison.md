# Run a simple C sample on Intel Edison

The following describes how to build and run the **simplesample_amqp** application on an Intel Edison.

## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

## Preparing the Intel Edison board

- Ensure the latest OS image has been installed. To do that, follow these instructions on Intel’s official documentation [portal][IntelEdisonGettingStarted].
- After following the standard Intel Edison setup instructions, you will need to establish a [serial connection][IntelEdisonSerialConnection] to your device from your machine.
- Once you have established a serial connection (command line) to your Intel Edison board, you can proceed to install Azure IoT SDK using the below instructions.
- Make sure you have run the “configure_edison —setup” command to set up your board
- Make sure your Intel Edison is online via your local Wi-Fi network (should occur during configure_edison setup)

## Installing Git on your Intel Edison

Git is a widely used distributed version control tool, we will need to install Git on the Intel Edison in order to clone the Azure IoT SDK and build it locally. To do that, we must first add extended packages to Intel Edison which include Git. Intel Edison’s build of [Yocto Linux][yocto] uses the “opkg” manager which doesn’t, by default, include Git support.

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

- Exit by hitting “esc,” “SHIFT+:" and typing “wq” then hitting Enter. If you are unfamiliar with vi editing, read [this][vi].

- Next, update and upgrade your opkg base-feeds on your command line:

    ```
    $ opkg update
    ```

You should see the following:

![][img1]

## Downloading the Azure IoT SDK to your Intel Edison

- On your Intel Edison, use Git and clone the Azure SDK repository directly to your Edison using the following commands. We recommend using the default folder located in /home/root:

  ```
  $ opkg install git
  $ git clone github.com/Azure/azure-iot-suite-sdks.git
  ```

- You may be prompted to add an RSA key to your device, respond with “yes.”

- Alternate Deploy Method

  - If for any reason you are unable to clone the Azure IoT SDK directly to Edison, you can clone the repository to your PC / Mac / Linux desktop and then transfer the files over the network to your Intel  Edison using [FileZilla][filezilla] or SCP.
  - For FileZilla, run “wpa_cli status” on your Intel Edison to find your IP address, then use “sftp://your.ip.address”, use password “root” and your Intel Edison password to establish an SFTP connection via FileZilla. Once you have done that, you can drag and drop files over the network directly.

	![][img2]

## Building the Azure IoT SDK and running a simple sample on Intel Edison

We will build a sample application which relies on the SDK.
We first need to update the credentials in the sample AMPQ app to match those of our Azure IoT Hub application. When we build the Azure IoT SDK, the sample C applications are automatically built by default, we need to include our credentials into the sample app while we build the SDK so that they are ready to function after we build.

- Edit "/c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp.c" as given below using vi or your favorite text editor:
 - Replace the "IoT Hub connection” aka “connectionString” string placeholder with your info as below (static const char* ….) When you are finished the result should look like the below connection string with your own credentials instead of the placeholders in brackets.
  **IMPORTANT**: Replace items in brackets with your credentials or the sample will not function
  ```
  static const char* connectionString = “HostName=[YOUR-HOST-NAME];CredentialScope=Device;DeviceId=[YOUR-DEVICE-ID];SharedAccessKey=[YOUR-ACCESS-KEY";
  ```

- In the terminal, enter /c/build_all/linux and execute the following steps:
  ```
  $ opkg install util-linux-libuuid-dev
  $ ./build_proton.sh
  $ ./build.sh
  ```

- Update the ldconfig cache
  While building the Azure IoT SDK, we needed to first build a dependency called [Qpid Proton][qpidproton].
  However, we need to register the resulting library with [ldconfig][ldcconfig] before we can proceed to testing and building our C-language samples.
  To do that, we need to first locate where the Proton lib is and then copy it into the /lib folder in Yocto.

- Add libqpid-proton.so.2 to shared libs by running the following terminal commands:
  ```
  $ find -name 'libqpid-proton.so.2’
  ```
  - Copy the directory you are given by this command to your clipboard
    ```
    $ cp [directory_to_libqpid-proton.so.2] /lib
    ```
  - Replace [directory_to_libqpid-proton.so.2] with the result of the “find” command from the first step
    ```
    $ ldconfig
    ```
  - This will automatically update the cache, its a one line command
    ```
    $ ldconfig -v | grep "libqpid-p*”
    ```
If you completed the operation correctly, you will see "libqpid-proton.so.2” listed
Now that we have added Qpid Proton to our ldcache, we are able to build the sample C project which relies on Proton:

- Navigate back to: /c/iothub_client/samples/iothub_client_sample_amqp/iothub_client_sample_amqp/linux and run the following commands
  ```
  make -f makefile.linux
  ./iothub_client_sample_amqp
  ```

The result should be the following:

```
# ./iothub_client_sample_amqp
hub_client/samples/iothub_client_sample_amqp/linux#
Starting the IoTHub client sample AMQP...
IoTHubClient_SetNotificationCallback...successful.
IoTHubClient_SendTelemetryAsync accepted data for transmission to IoT Hub.
IoTHubClient_SendTelemetryAsync accepted data for transmission to IoT Hub.
IoTHubClient_SendTelemetryAsync accepted data for transmission to IoT Hub.
IoTHubClient_SendTelemetryAsync accepted data for transmission to IoT Hub.
IoTHubClient_SendTelemetryAsync accepted data for transmission to IoT Hub.
Press any key to exit the application.
Confirmation[0] received for message tracking id = 0 with result = IOTHUB_CLIENT_CONFIRMATION_OK Confirmation[1] received for message tracking id = 1 with result = IOTHUB_CLIENT_CONFIRMATION_OK Confirmation[2] received for message tracking id = 2 with result = IOTHUB_CLIENT_CONFIRMATION_OK Confirmation[3] received for message tracking id = 3 with result = IOTHUB_CLIENT_CONFIRMATION_OK Confirmation[4] received for message tracking id = 4 with result = IOTHUB_CLIENT_CONFIRMATION_OK
```


[IntelEdisonGettingStarted]: https://software.intel.com/en-us/iot/library/edison-getting-started
[IntelEdisonSerialConnection]: https://software.intel.com/en-us/setting-up-serial-terminal-intel-edison-board
[yocto]: http://www.yoctoproject.org/docs/latest/adt-manual/adt-manual.html
[vi]: https://www.cs.colostate.edu/helpdocs/vi.html
[filezilla]: https://filezilla-project.org
[qpidproton]: http://qpid.apache.org/releases/qpid-proton-0.5/
[ldcconfig]: http://codeyarns.com/2014/01/14/how-to-add-library-directory-to-ldconfig-cache/

[img1]: ./media/edison01.png
[img2]: ./media/edison02.png

[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
