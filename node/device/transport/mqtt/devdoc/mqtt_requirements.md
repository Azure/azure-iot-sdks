# azure-iot-device-mqtt.Mqtt Requirements

## Overview
Mqtt provides a middle layer between the generic device Client and the specific MQTT transport implementation.

## Example usage
```js
'use strict';
var Mqtt = require('azure-iot-device-mqtt').Mqtt;
var Message = require('azure-iot-device-mqtt').Message;

var config = {
  gatewayHostName: /* ... */,
  deviceId: /* ... */,
  sharedAccessSignature: /* ... */,
  x509: {
    cert: /* ... */,
    key: /* ... */
  }
};

var mqtt = new Mqtt(config);
client.connect(function (err) {
  if (err) {
    console.error(err.message);
  } else {
    var message = new Message(data);
    mqtt.sendEvent(message, function () {
      if (err) {
        console.error(err.message);
      } else {
        console.log('sent message: ' + message.getData());
      }
    });

    mqtt.getReceiver(function (err, receiver) {
      receiver.on('message', function(msg) { 
        console.log('received: ' + msg.getData());
      });

      receiver.on('error', function(err) {
        console.error(err.message);
      });
    });
  }
});
```

## Public Interface
### Mqtt constructor
The `Mqtt` constructor initializes a new instance of the MQTT transport.

**SRS_NODE_DEVICE_MQTT_12_001: [** The `Mqtt` constructor shall accept the transport configuration structure.**]**

**SRS_NODE_DEVICE_MQTT_12_002: [** The `Mqtt` constructor shall store the configuration structure in a member variable.**]**

**SRS_NODE_DEVICE_MQTT_12_003: [** The `Mqtt` constructor shall create an MqttTransport object and store it in a member variable.**]**

### connect(done)
The `connect` method initializes a connection to an IoT hub.

**SRS_NODE_DEVICE_MQTT_12_004: [** The `connect` method shall call the `connect` method on `MqttTransport`. **]**

### disconnect(done)
The `disconnect` method should close the connection to the IoT Hub instance.

**SRS_NODE_DEVICE_MQTT_16_001: [** The `disconnect` method should call the `disconnect` method on `MqttTransport`. **]**

### sendEvent(message)
The `sendEvent` method sends an event to an IoT hub on behalf of the device indicated in the constructor argument.

**SRS_NODE_DEVICE_MQTT_12_005: [** The `sendEvent` method shall call the publish method on `MqttTransport`. **]**

### getReceiver(done)
The `getReceiver` method creates a receiver object and returns it, or returns the existing instance.

**SRS_NODE_DEVICE_MQTT_16_002: [** If a receiver for this endpoint has already been created, the `getReceiver` method should call the `done` callback with the existing instance as an argument. **]** 

**SRS_NODE_DEVICE_MQTT_16_003: [** If a receiver for this endpoint doesn’t exist, the `getReceiver` method should create a new `MqttReceiver` object and then call the `done` callback with the object that was just created as an argument. **]** 

### abandon(message, done)
The `abandon` method is there for compatibility purposes with other transports but will throw because the MQTT protocol doesn't support abandonning messages.

**SRS_NODE_DEVICE_MQTT_16_004: [** The `abandon` method shall throw because MQTT doesn’t support abandoning messages. **]** 

### complete(message, done)
The `complete` method is there for compatibility purposes with other transports but doesn't do anything because messages are automatically acknowledged.

**SRS_NODE_DEVICE_MQTT_16_005: [** The `complete` method shall call the `done` callback given as argument immediately since all messages are automatically completed. **]** 

### reject(message, done)
The `reject` method is there for compatibility purposes with other transports but will throw because the MQTT protocol doesn't support abandonning messages.

**SRS_NODE_DEVICE_MQTT_16_006: [** The `reject` method shall throw because MQTT doesn’t support rejecting messages. **]** 

### updateSharedAccessSignature(sharedAccessSignature, done)

**SRS_NODE_DEVICE_MQTT_16_007: [** The `updateSharedAccessSignature` method shall save the new shared access signature given as a parameter to its configuration. **]** 

**SRS_NODE_DEVICE_MQTT_16_008: [** The `updateSharedAccessSignature` method shall disconnect the current connection operating with the deprecated token, and re-initialize the transport object with the new connection parameters. **]** 

**SRS_NODE_DEVICE_MQTT_16_009: [** The `updateSharedAccessSignature` method shall call the `done` method with an `Error` object if updating the configuration or re-initializing the transport object. **]** 

**SRS_NODE_DEVICE_MQTT_16_010: [** The `updateSharedAccessSignature` method shall call the `done` callback with a `null` error object and a `SharedAccessSignatureUpdated` object as a result, indicating that the client needs to reestablish the transport connection when ready. **]** 

### setOptions(options, done)

**SRS_NODE_DEVICE_MQTT_16_011: [** The `setOptions` method shall throw a `ReferenceError` if the `options` argument is falsy **]**

**SRS_NODE_DEVICE_MQTT_16_015: [** The `setOptions` method shall throw an `ArgumentError` if the `cert` property is populated but the device uses symmetric key authentication. **]**

**SRS_NODE_DEVICE_MQTT_16_012: [** The `setOptions` method shall update the existing configuration of the MQTT transport with the content of the `options` object. **]**

**SRS_NODE_DEVICE_MQTT_16_013: [** If a `done` callback function is passed as a argument, the `setOptions` method shall call it when finished with no arguments. **]**

**SRS_NODE_DEVICE_MQTT_16_014: [** The `setOptions` method shall not throw if the `done` argument is not passed. **]**
