#azure-iothub.AmqpWs requirements

## Overview
`AmqpWs` provides transport functionality for applications that want to communicate with an Azure IoT Hub using the AMQP protocol **over websockets**. 
It provides an additional level of abstraction on top of the `AmqpTransport` class which is not specific to the device or service side (and is located in the `azure-iot-common` package).
Based on the configuration parameters given to the constructor, the Amqp object will build the SASL-Plain URL used to communicate with the IoT Hub instance, as well as the sending and receiving endpoints, and will instantiate an `AmqpTransport` object to use with these parameters.

## Usage
```js
'use strict';
var Amqp = require('./lib/amqp_ws.js').Amqp;
var Message = require('azure-iot-common').Message;

function print(err, res) {
  if (err) console.log(err.toString());
  if (res) console.log(res.constructor.name);
}

var serviceConfig = {
  host: 'hostname',
  keyName: 'keyname' ,
  key: 'key'
};

var serviceAmqp = new Amqp(serviceConfig);
serviceAmqp.send(deviceId, new Message('hello world'), print);
serviceAmqp.getReceiver(function (receiver) {
  receiver.on('message', function (msg) {
    receiver.complete(msg, print); 
  });
  receiver.on('errorReceived', function (err) {
    print(err); 
  });
});

```
## Public API
### constructor

**SRS_NODE_IOTHUB_SERVICE_AMQP_WS_16_001: [** The `AmqpWs` constructor shall accept a config object with those four properties:
- `host` – (string) the fully-qualified DNS hostname of an IoT Hub
- `hubName` - (string) the name of the IoT Hub instance (without suffix such as .azure-devices.net).
- `keyName` – (string) the name of a key that can be used to communicate with the IoT Hub instance
- `sharedAccessSignature–` (string) the key associated with the key name. **]**

**SRS_NODE_IOTHUB_SERVICE_AMQP_WS_16_002: [** `AmqpWs` should inherit from `Amqp`. **]**
