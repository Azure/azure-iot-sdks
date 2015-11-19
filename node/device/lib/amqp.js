// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-common').Amqp;
var Endpoint = require('azure-iot-common').endpoint;

/** 
 * @class Amqp
 * @classdesc Constructs an {@linkcode Amqp} object that can be used on a device to send 
 *            and receive messages to and from an IoT Hub instance, using the AMQP protocol.
 */
/*Codes_SRS_NODE_DEVICE_AMQP_16_001: [The Amqp constructor shall accept a config object with four properties:
host – (string) the fully-qualified DNS hostname of an IoT Hub
hubName - (string) the name of the IoT Hub instance (without suffix such as .azure-devices.net).
deviceId – (string) the identifier of a device registered with the IoT Hub
sharedAccessSignature – (string) the shared access signature associated with the device registration.] */
function Amqp(config) {
  this._config = config;
  var uri = 'amqps://';
  uri += encodeURIComponent(this._config.deviceId) +
         '@sas.' +
         this._config.hubName +
         ':' +
         encodeURIComponent(this._config.sharedAccessSignature) +
         '@' +
         this._config.host;
      
  this._amqp = new Base(uri, false);
}

/**
 * Establishes a connection with the IoT Hub instance.
 * @param {Function}   done   Called when the connection is established of if an error happened.
 */
/*Codes_SRS_NODE_DEVICE_AMQP_16_008: [The done callback method passed in argument shall be called if the connection is established]*/ 
/*Codes_SRS_NODE_DEVICE_AMQP_16_009: [The done callback method passed in argument shall be called with an error object if the connecion fails]*/ 
Amqp.prototype.connect = function connect(done) {
  this._amqp.connect(done);
};

/**
 * Disconnects the link to the IoT Hub instance.
 * @param {Function}   done   Called when disconnected of if an error happened.
 */
/*Codes_SRS_NODE_DEVICE_AMQP_16_010: [The done callback method passed in argument shall be called when disconnected]*/ 
/*Codes_SRS_NODE_DEVICE_AMQP_16_011: [The done callback method passed in argument shall be called with an error object if disconnecting fails]*/ 
Amqp.prototype.disconnect = function disconnect(done) {
  this._amqp.disconnect(done);
};

/**
 * Sends an event to the IoT Hub.
 * @param {Message}  message    The [message]{@linkcode module:common/message.Message}
 *                              to be sent.
 * @param {Function} done       The callback to be invoked when `sendEvent`
 *                              completes execution.
 */
/* Codes_SRS_NODE_DEVICE_AMQP_16_002: [The sendEvent method shall construct an AMQP request using the message passed in argument as the body of the message.] */ 
/* Codes_SRS_NODE_DEVICE_AMQP_16_003: [The sendEvent method shall call the done() callback with no arguments when the message has been successfully sent.] */ 
/* Codes_SRS_NODE_DEVICE_AMQP_16_004: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message). ] */  
Amqp.prototype.sendEvent = function sendEvent(message, done) {
  var endpoint = Endpoint.eventPath(this._config.deviceId);
  this._amqp.send(message, endpoint, endpoint, done);
};

/**
 * Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 * @param {Function}  done      Callback used to return the {@linkcode AmqpReceiver} object.
 */
/* Codes_SRS_NODE_DEVICE_AMQP_16_006: [If a receiver for this endpoint has already been created, the getReceiver method should call the done() method with the existing instance as an argument.]*/ 
/* Codes_SRS_NODE_DEVICE_AMQP_16_007: [If a receiver for this endpoint doesn’t exist, the getReceiver method should create a new AmqpReceiver object and then call the done() method with the object that was just created as an argument.]*/ 
Amqp.prototype.getReceiver = function getMessageReceiver(done) {
  var messageEndpoint = Endpoint.messagePath(encodeURIComponent(this._config.deviceId));
  this._amqp.getReceiver(messageEndpoint, done);
};


/**
 * The `sendEventBatch` method sends a list of event messages to the IoT Hub.
 * @param {array<Message>} messages   Array of [Message]{@linkcode module:common/message.Message}
 *                                    objects to be sent as a batch.
 * @param {Function}      done      The callback to be invoked when
 *                                  `sendEventBatch` completes execution.
 */
/* Codes_SRS_NODE_DEVICE_AMQP_16_005: [If sendEventBatch encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]  */
//Amqp.prototype.sendEventBatch = function (messages, done) { 
  // Placeholder - Not implemented yet.
//};

module.exports = Amqp;