# Getting started - running a Node.js sample application

This "Getting Started" document shows you how to build and run the **simple_sample_http.js** Node.js sample application.

## Prerequisites

Before you get started, you should:

- [Prepare your development environment][setup-devbox].
- [Set up your IoT hub][setup-iothub].
- Use the **iothub-explorer** utility to [connect your device to an IoT hub][provision-device]. You'll use **iothub-explorer** again in a moment to monitor the messages you send from the sample application; leave your shell (Linux) or Node.js command prompt (Windows) open.
- Make note of the connection string for your device from the previous step.

## Build and run the sample

- Get the following sample files from https://github.com/Azure/azure-iot-sdks/node/device/samples
    - **package.json**
    - **simple_sample_http.js**

- Place the files in the folder of your choice on the target machine/device

- Open the file **simple_sample_http.js** in a text editor.

- Locate the following code in the file:

    ```
    var connectionString = '[IoT Device Connection String]';
    ```

- Replace `[IoT Device Connection String]` with the connection string for your device. Save the changes.

- From the shell or command prompt you used earlier to run the **iothub-explorer** utility, use the following command to receive device-to-cloud messages from the sample application (replace <device-id> with the ID you assigned your device earlier):

    ```
    node iothub-explorer.js "<iothub-connection-string>" monitor-events <device-id>
    ```

- Open a new shell or Node.js command prompt and navigate to the folder where you placed the sample files. Run the sample application using the following commands:

    ```
    npm install
    node .
    ```

- The sample application will send messages to your IoT hub, and the **iothub-explorer** utility will display the messages as your IoT hub receives them.

[setup-devbox]: devbox_setup.md
[setup-iothub]: ../../../doc/setup_iothub.md
[provision-device]: ../../../tools/iothub-explorer/doc/provision_device.md
