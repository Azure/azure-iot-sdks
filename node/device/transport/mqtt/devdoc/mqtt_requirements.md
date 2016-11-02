# azure-iot-device-mqtt.Mqtt/MqttWs Requirements

## Overview
`Mqtt` and `MqttWs` provide a standard transport interface between the generic device Client and the specific MQTT transport implementation. 
`MqttWs` will connect over secure websockets whereas `Mqtt` connects over secure TCP sockets.

## Example usage
```js
'use strict';
var Mqtt = require('azure-iot-device-mqtt').Mqtt; // Or require('azure-iot-device-mqtt').MqttWs
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
The `Mqtt` and `MqttWs` constructors initialize a new instance of the MQTT transport.

**SRS_NODE_DEVICE_MQTT_12_001: [** The constructor shall accept the transport configuration structure.**]**

**SRS_NODE_DEVICE_MQTT_12_002: [** The constructor shall store the configuration structure in a member variable.**]**

**SRS_NODE_DEVICE_MQTT_12_003: [** The constructor shall create an MqttTransport object and store it in a member variable.**]**

**SRS_NODE_DEVICE_MQTT_16_016: [** The `Mqtt` constructor shall initialize the `uri` property of the `config` object to `mqtts://<host>`. **]**

**SRS_NODE_DEVICE_MQTT_16_017: [** The `MqttWs` constructor shall initialize the `uri` property of the `config` object to `wss://<host>:443/$iothub/websocket`. **]**

**SRS_NODE_DEVICE_MQTT_18_025: [** If the `Mqtt` constructor receives a second parameter, it shall be used as a provider in place of mqtt.js **]**  

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

### sendMethodResponse(response, done)
The `sendMethodResponse` method sends the given response to the method's topic on an IoT Hub on behalf of the device indicated in the constructor argument. The `response` argument is an object that has the following shape:

```
interface StringMap {
  [key: string]: string;
}

interface DeviceMethodResponse {
  requestId: string;
  properties: StringMap;
  status: number;
  bodyParts: Buffer[];
}
```

**SRS_NODE_DEVICE_MQTT_13_001: [** `sendMethodResponse` shall throw an `Error` if `response` is falsy or does not conform to the shape defined by `DeviceMethodResponse`. **]**

**SRS_NODE_DEVICE_MQTT_13_002: [** `sendMethodResponse` shall build an MQTT topic name in the format: `$iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES>` where `<STATUS>` is `response.status`. **]**

**SRS_NODE_DEVICE_MQTT_13_003: [** `sendMethodResponse` shall build an MQTT topic name in the format: `$iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES>` where `<REQUEST ID>` is `response.requestId`. **]**

**SRS_NODE_DEVICE_MQTT_13_004: [** `sendMethodResponse` shall build an MQTT topic name in the format: `$iothub/methods/res/<STATUS>/?$rid=<REQUEST ID>&<PROPERTIES>` where `<PROPERTIES>` is URL encoded. **]**

**SRS_NODE_DEVICE_MQTT_13_005: [** `sendMethodResponse` shall concatenate `response.bodyParts` into a single `Buffer` and publish the message to the MQTT topic name. **]**

**SRS_NODE_DEVICE_MQTT_13_006: [** If the MQTT publish fails then an error shall be returned via the `done` callback's first parameter. **]**

**SRS_NODE_DEVICE_MQTT_13_007: [** If the MQTT publish is successful then the `done` callback shall be invoked passing `null` for the first parameter. **]**

### sendTwinRequest(method, resource, properties, body, done)
The `sendTwinRequest` method sends the given body to the given endpoint on an IoT hub on behalf of the device indicated in the constructor argument.


**SRS_NODE_DEVICE_MQTT_18_001: [** The `sendTwinRequest` method shall call the publish method on `MqttTransport`. **]**

**SRS_NODE_DEVICE_MQTT_18_008: [** The `sendTwinRequest` method shall not throw if the `done` callback is falsy. **]**

**SRS_NODE_DEVICE_MQTT_18_009: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `method` argument is falsy. **]**

**SRS_NODE_DEVICE_MQTT_18_010: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `method` argument is not a string. **]**

**SRS_NODE_DEVICE_MQTT_18_019: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `resource` argument is falsy. **]**

**SRS_NODE_DEVICE_MQTT_18_020: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `resource` argument is not a string. **]**

**SRS_NODE_DEVICE_MQTT_18_011: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `properties` argument is falsy. **]**

**SRS_NODE_DEVICE_MQTT_18_012: [** The `sendTwinRequest` method shall throw an `ArgumentError` if the `properties` argument is not a an object. **]**

**SRS_NODE_DEVICE_MQTT_18_018: [** The `sendTwinRequest` method shall throw an `ArgumentError` if any members of the `properties` object fails to serialize to a string **]**

**SRS_NODE_DEVICE_MQTT_18_013: [** The `sendTwinRequest` method shall throw an `ReferenceError` if the `body` argument is falsy. **]**

**SRS_NODE_DEVICE_MQTT_18_022: [** The `propertyQuery` string shall be construced from the `properties` object. **]**  

**SRS_NODE_DEVICE_MQTT_18_023: [** Each member of the `properties` object shall add another 'name=value&' pair to the `propertyQuery` string. **]**  

**SRS_NODE_DEVICE_MQTT_18_004: [** If a `done` callback is passed as an argument, The `sendTwinRequest` method shall call `done` after the body has been published. **]**

**SRS_NODE_DEVICE_MQTT_18_021: [** The topic name passed to the publish method shall be $iothub/twin/`method`/`resource`/?`propertyQuery` **]**

**SRS_NODE_DEVICE_MQTT_18_015: [** The `sendTwinRequest` shall publish the request with QOS=0, DUP=0, and Retain=0 **]**

**SRS_NODE_DEVICE_MQTT_18_016: [** If an error occurs in the `sendTwinRequest` method, the `done` callback shall be called with the error as the first parameter. **]**

**SRS_NODE_DEVICE_MQTT_18_024: [** If an error occurs, the `sendTwinRequest` shall use the MQTT `translateError` module to convert the mqtt-specific error to a transport agnostic error before passing it into the `done` callback. **]**

**SRS_NODE_DEVICE_MQTT_18_017: [** If the `sendTwinRequest` method is successful, the first parameter to the `done` callback shall be null and the second parameter shall be a MessageEnqueued object. **]**

### getTwinReceiver(done) 
The `getTwinReceiver` method creates a `MqttTwinReceiver` object for the twin response endpoint and returns it, or returns the existing instance.

**SRS_NODE_DEVICE_MQTT_18_014: [** The `getTwinReceiver` method shall throw an `ReferenceError` if done is falsy **]** 

**SRS_NODE_DEVICE_MQTT_18_003: [** If a twin receiver for this endpoint doesn’t exist, the `getTwinReceiver` method should create a new `MqttTwinReceiver` object. **]**

**SRS_NODE_DEVICE_MQTT_18_002: [** If a twin receiver for this endpoint has already been created, the `getTwinReceiver` method should not create a new `MqttTwinReceiver` object. **]**

**SRS_NODE_DEVICE_MQTT_18_005: [** The `getTwinReceiver` method shall call the `done` method after it completes **]**

**SRS_NODE_DEVICE_MQTT_18_006: [** If a twin receiver for this endpoint did not previously exist, the `getTwinReceiver` method should return the a new `MqttTwinReceiver` object as the second parameter of the `done` function with null as the first parameter. **]**

**SRS_NODE_DEVICE_MQTT_18_007: [** If a twin receiver for this endpoint previously existed, the `getTwinReceiver` method should return the preexisting `MqttTwinReceiver` object as the second parameter of the `done` function with null as the first parameter. **]**


