The Azure IoT Service SDK for Node.js helps you build applications that interact with your devices and manage their identities in your IoT hub. With the new device management (DM) capabilities, you can interact with your devices in new ways. 

## Installation

```
npm install azure-iothub@latest
```

## Features

### Send commands to your devices and receive command feedback from the device
* Send commands to your devices and get feedback when they're delivered [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/send_c2d_message.js)

### Manage your device registry
* Create/remove/update/list device identities in your IoT hub [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/registry_sample.js)
* Run complex queries to find specific devices based on property values, tags, etc. [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/registry_queryDevices.js)
* Import/Export devices in bulk from Azure Storage blobs [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/registry_bulk_sample.js) 

### Manage your devices
* Schedule reboots, firmware upgrades, factory resets, and other device management tasks on your devices [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/jobClient_scheduleJob.js)
* Read/Write device properties [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/jobClient_scheduleJob.js)
* Monitor device management jobs and query for jobs history [Code sample](https://github.com/Azure/azure-iot-sdks/blob/dmpreview/node/service/samples/jobClient_queryJobHistory.js) 