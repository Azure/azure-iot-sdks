# C Device SDK
- 

# Java SDK
- 

# C# Service SDK
- 

# Node Device Client
- 

# Python SDK
- 

# General
- 

# Bug fixes

- 


Release for May 6, 2016

# C Device SDK
- Cloud-to-Device message application properties exposed by AMQP transport to IoT client layer.
- Parse for a device SAS token input part of the device connection string.
- Added support for AMQP on Arduino Yun builds

# Java SDK
- Device SDK is now Java 7 compatible. It can be used to target Android projects 4.0.3 or higher with HTTPS and MQTT protocols.

# C# Service SDK
- Support for PurgeMessageQueueAsync
- Revised service API version with added support for custom blob names for Import/ExportDevicesAsync + quality of life changes for bulk operations

# Node Device Client
- Added a node-RED module for Azure IoT Hub

# Python SDK
- Bug fixes for messageTimeout option and message properties in the Python binding
- Build improvements

# General
- Documentation fixes

# Bug fixes

- GitHub issue #454: message-id and correlation-id properties now are exposed when using AMQP transport on IoT Hub C Device Client;
- Java Device SDK - fixed a bug which was preventing message properties being sent over AMQP protocol.


