# Getting started - running a C sample application on Windows Embedded Compact 2013 on a Toradex module

This "Getting Started" document shows you how to build and run the **simplesample_http** application on the Windows Embedded Compact 2013 platform.

## Requirements

These instructions assume that you have done the following:

- [Prepare your development environment][devbox-setup]

These instructions apply to [Colibri Modules][colibri-modules] running the [Windows Embedded Compact 2013 image provided by Toradex][toradex-images], if you are using a different operating system image or a different device running Windows Embedded Compact 2013, some of the steps described in this document may require different operations on your target device.

## Before you begin
Before you begin you will need to create and configure an IoT hub to connect to.
  1. [Set up your IoT Hub][setup-iothub].
  1. With your IoT hub configured and running in Azure, follow the instructions in [Connecting your device to an IoT hub][provision-device].
  1. Make note of the Connection String for your device from the previous step.

  > Note: You can skip this step if you just want to build the sample application without running it.

## Build and run the sample

1. Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake_ce8** folder in your home directory.

2. In Visual Studio, in **Solution Explorer**, navigate to **simplesample_http** project, open the **simplesample_http.c** file.

3. Locate the following code in the file:

    ```
   static const char* connectionString = "[device connection string]";
    ```

4. Replace "[device connection string]" with the device connection string you noted [earlier](#beforebegin) and save the changes:

  ```
  static const char* connectionString = "HostName=..."
  ```

5. The section "Send events" in the document [How to use Device Explorer](../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md) describes how to prepare the **DeviceExplorer** tool to receive device-to-cloud messages from the sample application.

6. To enable secure connection using HTTPS you need to import the security certificates used by Azure IoT hub into the local certificate store of the device. To do this you can connect to your hub using a browser. The hub url is in the format: https://<yourhubname>.azure-devices.net and you can find it in the hub's properties window on Azure portal. Your browser will show a blank page and a lock icon close to the address, by clicking on that icon you should be able to inspect and save the security certificates used to secure the connection to your hub. Save them in CER format and copy them to a thumbdrive or SD card.

7. Insert the USB thumbdrive or SD card in your Toradex board and boot your device.

8. Open control panel by clicking on the "Start" button and then selecting Settings\Control Panel.

9. Select the "Certificates" applet by double clicking on it.

10. Click on "Import..." and the on the "OK" button in the title bar of the dialog box that will appear.

11. Browse to the folder where your certificate file is stored (it can be "SD Card" or "USB HD", depending on the media you used to transfer it).

12. double click on the certificate file, it will be imported in your local certificates storage.

13. Save the registry by selecting "Start\Programs\ColibriTools\SaveReg". This will save the certificates permanently, so you don't need to repeat the operations described in points 7 to 13 every time you reboot your device.

14. Check that date and time on your device are set correctly. Using a date outside of the validity period of your certificates can prevent your device from getting connected to the hub. To update your date and time you can open a command prompt and type:
```
  services refresh NTP0:
```  
15. In **Solution Explorer**, right-click the **simplesample_http** project, click **Properties**, then select **Configuration Properties\Debugging** to build and then insert the ip addressof your target device in the **IP Address** field. Select **OK** to save your changes.

16. In **Solution Explorer**, right-click the **simplesample_http** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

17. Use the **DeviceExplorer** utility to observe the messages IoT Hub receives from the **simplesample_http** application.

18. See "Monitor device-to-cloud events" in the document [How to use Device Explorer][device-explorer] to learn how to use the **DeviceExplorer** utility to send cloud-to-device messages to the **simplesample_http** application.


[devbox-setup]: devbox_setup.md
[device-explorer]: ../../tools/DeviceExplorer/doc/how_to_use_device_explorer.md
[setup-iothub]: ../../doc/setup_iothub.md
[provision-device]: ./provision_device.md
[colibri-modules]:https://www.toradex.com/computer-on-modules/colibri-arm-family
[toradex-images]:http://developer.toradex.com/frequent-downloads#Windows_Embedded_Compact
