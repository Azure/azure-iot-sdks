# AmqpMessage Requirements

## Overview
AmqpMessage is a type representing an AMQP message, including properties, annotations, and data as defined by the AMQP 1.0 spec, section 3.2.  It exposes a static factory method for creating an AMQP message from an IoT Hub Message, and exposes properties as mapped from the latter.

Example usage
```js
'use strict';
var Message = require('azure-iot-common').Message;
var AmqpMessage = require('./amqp_message.js');

var message = new Message('hello');
message.to = 'destination';
message.messageId = 'unique-message-id';
message.expiryTimeUtc = Date.now() + 60000; // 1 min from now
message.ack = 'negative';

var amqpMessage = AmqpMessage.fromMessage(message);
console.log(amqpMessage);

// output:
// AmqpMessage {
//   properties: {
//     to: 'destination',
//     absoluteExpiryTime: 1445537441835,
//     messageId: 'unique-message-id'
//   },
//   applicationProperties: {
//     'iothub-ack': 'negative'
//   },
//   body: 'hello'
// }
```

## Public Interface

### fromMessage(message) [static]
The fromMessage static method returns a new instance of the AmqpMessage object with properties corresponding to the properties found in message.
As this is an internal API, the input argument message can be assumed to be of type azure-iot-common.Message.

**SRS_NODE_IOTHUB_AMQPMSG_05_001: [** The `fromMessage` method shall create a new instance of `AmqpMessage`. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_002: [** The created `AmqpMessage` object shall have a property of type `Object` named `properties`. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_003: [** If the `message` argument has a `to` property, the `properties` property of the `AmqpMessage` object shall have a property named `to` with the same value. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_004: [** If the `message` argument has an `expiryTimeUtc` property, the `properties` property of the `AmqpMessage` object shall have a property named `absoluteExpiryTime` with the same value. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_007: [** If the `message` argument has a `messageId` property, the `properties` property of the `AmqpMessage` object shall have a property named `messageId` with the same value. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_008: [** If needed, the created `AmqpMessage` object shall have a property of type `Object` named `applicationProperties`. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_009: [** If the `message` argument has an `ack` property, the `applicationProperties` property of the `AmqpMessage` object shall have a property named `iothub-ack` with the same value. **]**

**SRS_NODE_IOTHUB_AMQPMSG_13_001: [** If `message.properties` is truthy, then all the properties in it shall be copied to the `applicationProperties` property of the `AmqpMessage` object. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_005: [** If `message.getData()` is truthy, the `AmqpMessage` object shall have a property named `body` with the value returned from `message.getData()`. **]**

**SRS_NODE_IOTHUB_AMQPMSG_05_006: [** The generated `AmqpMessage` object shall be returned to the caller. **]**
