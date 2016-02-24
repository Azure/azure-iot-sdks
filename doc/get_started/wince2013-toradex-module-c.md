---
platform: wince2013
device: toradex-module
language: c
---

Run a simple C sample on Windows Embedded Compact 2013 on a Toradex module
===
---

# Table of Contents

- [Introduction](#Introduction)
- [Step 1: Prerequisites](#Step-1-Prerequisites)
- [Step 2: Prepare your Device](#Step-2-PrepareDevice)
- [Step 3: Build and Run the Sample](#Step-3-Build)

<a name="Introduction"></a>
# Introduction

**About this document**

This document describes how to build and run the **simplesample_http** application on the Windows Embedded Compact 2013 platform. This multi-step process includes:
- Configuring Azure IoT Hub
- Registering your IoT device
- Build and deploy Azure IoT SDK on device

<a name="Step-1-Prerequisites"></a>
# Step 1: Prerequisites

You should have the following items ready before beginning the process:
- Computer with Git client installed and access to the
  [azure-iot-sdks](https://github.com/Azure/azure-iot-sdks) GitHub public repository.
- [Colibri Modules][colibri-modules] running the [Windows Embedded Compact 2013 image provided by Toradex][toradex-images]
-   [Setup your IoT hub][lnk-setup-iot-hub]
-   [Provision your device and get its credentials][lnk-manage-iot-hub]

<a name="Step-2-PrepareDevice"></a>
# Step 2: Prepare your Device

These instructions assume that you have done the following:

- [Prepare your development environment][devbox-setup]

These instructions apply to [Colibri Modules][colibri-modules] running the [Windows Embedded Compact 2013 image provided by Toradex][toradex-images], if you are using a different operating system image or a different device running Windows Embedded Compact 2013, some of the steps described in this document may require different operations on your target device.

<a name="Step-3-Build"></a>
# Step 3: Build and Run the sample

1. Start a new instance of Visual Studio 2015. Open the **azure_iot_sdks.sln** solution in the **cmake_ce8** folder in your home directory.

2. In Visual Studio, in **Solution Explorer**, navigate to **simplesample_http** project, open the **simplesample_http.c** file.

3. Locate the following code in the file:

    ```
   static const char* connectionString = "[device connection string]";
    ```

4. Replace "[device connection string]" with the device connection string you noted [earlier](#Step-1-Prerequisites) and save the changes:

  ```
  static const char* connectionString = "HostName=..."
  ```

6. To enable secure connection using HTTPS you need to import the security certificates used by Azure IoT hub into the local certificate store of the device. To do this you can connect to your hub using a browser. The hub url is in the format: https://<yourhubname>.azure-devices.net and you can find it in the hub's properties window on Azure portal. Your browser will show a blank page and a lock icon close to the address, by clicking on that icon you should be able to inspect and save the security certificates used to secure the connection to your hub. Save them in CER format and copy them to a thumbdrive or SD card.

7. Insert the USB thumb-drive or SD card in your Toradex board and boot your device.

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
15. In **Solution Explorer**, right-click the **simplesample_http** project, click **Properties**, then select **Configuration Properties\Debugging** to build and then insert the ip address of your target device in the **IP Address** field. Select **OK** to save your changes.

16. In **Solution Explorer**, right-click the **simplesample_http** project, click **Debug**, and then click **Start new instance** to build and run the sample. The console displays messages as the application sends device-to-cloud messages to IoT Hub.

17.   See [Manage IoT Hub][lnk-manage-iot-hub] to learn how to observe the messages IoT Hub receives from the **simplesample_http** application and how to send cloud-to-device messages to the **simplesample_http** application.

[devbox-setup]: https://github.com/Azure/azure-iot-sdks/blob/master/c/doc/devbox_setup.md
[device-explorer]: https://github.com/Azure/azure-iot-sdks/blob/master/tools/DeviceExplorer/readme.md
[colibri-modules]:https://www.toradex.com/computer-on-modules/colibri-arm-family
[toradex-images]:http://developer.toradex.com/frequent-downloads#Windows_Embedded_Compact

[lnk-setup-iot-hub]: ../setup_iothub.md
[lnk-manage-iot-hub]: ../manage_iot_hub.md