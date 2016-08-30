# AmqpReceiver Requirements

## Overview
AmqpReceiver is a type representing an AMQP endpoint that can receive messages and settle them (completion/rejection/release). It emits events when a message or error is received.
The AmqpReceiver is initialized by the client with the receiver object created by the AMQP library.

## Example usage

```js
'use strict';
var AmqpReceiver = require('azure-iot-common').AmqpReceiver;

// this._amqp is the AMQP client object from the AMQP library
this._amqp.CreateReceiver('/messages/serviceBound/feedback').then(function(receiver){

var receiver = new AmqpReceiver(receiver);
receiver.on('message', function (msg){
	console.log('message: ' + msg);
	receiver.complete(msg, function() {
		console.log('message completed');
});
});
receiver.on('errorReceived', function(err){
	console.log('error: ' + err);
});
});
```

## Public Interface

### AmqpReceiver(receiver)
The AmqpReceiver constructor returns a new instance of an AmqpReceiver object that will use the receiver object passed as a parameter to receive and settle messages.

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_001: [** The AmqpReceiver method shall create a new instance of AmqpReceiver. **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_002: [** The created AmqpReceiver object shall emit a `message` event when a message is received. **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_014: [** The parameter to the message event emitted by the AmqpReceiver class should be an azure-iot-common:Message **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_13_001: [** If the AMQP message has values in it's `applicationProperties` property then those shall be added to the `properties` property of the newly created message object. **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_003: [** The created AmqpReceiver object shall emit a `errorReceived` event when an error is received. **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_004: [** If the receiver object passed as an argument is falsy, the AmqpReceiver should throw an exception of type ReferenceError **]**

### complete(message)
This method settles the `message` by sending a `complete` event to the AMQP stack, signifying that this `message` has been processed and can be removed from the message queue.

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_006: [** If the `message` object passed as an argument is falsy, an exception of type ReferenceError should be thrown **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_011: [** If the `message` is successfully settled the receiver should call the done callback with a null error object and a MessageCompleted result object **]**

### abandon(message)
This method settles the `message` by sending a `release` event to the AMQP stack, signifying that this `message` has not been processed and should be resent for processing. The `message` remains the in queue.

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_008: [** If the `message` object passed as an argument is falsy, an exception of type ReferenceError should be thrown **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_012: [** If the `message` is successfully settled the receiver should call the `done` callback with a null error object and a MessageAbandoned result object **]**

### reject(message)
This method settles the `message` by sending a `reject` event to the AMQP stack, signifying that this `message` has been processed and deemed.

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_010: [** If the `message` object passed as an argument is falsy, an exception of type ReferenceError should be thrown **]**

**SRS_NODE_IOTHUB_AMQPRECEIVER_16_013: [** If the `message` is successfully settled the receiver should call the `done` callback with a null error object and a MessageRejected result object **]**


