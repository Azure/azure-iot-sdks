// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events');
var util = require('util');

/**
 * @class            module:azure-iot-amqp-base.AmqpReceiver
 * @classdesc        The `AmqpReceiver` class is used to receive and settle messages.
 * 
 * @param {Object}   amqpReceiver   The Receiver object that is created by the client using the `node-amqp10` library.
 * 
 * @fires module:azure-iot-amqp-base.AmqpReceiver#message
 * @fires module:azure-iot-amqp-base.AmqpReceiver#errorReceived
 */
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_001: [The AmqpReceiver method shall create a new instance of AmqpReceiver.]*/ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_002: [The created AmqpReceiver object shall emit a ‘message’ event when a message is received.]*/ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_003: [The created AmqpReceiver object shall emit a ‘errorReceived’ event when an error is received.]*/ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_004: [If the receiver object passed as an argument is falsy, the AmqpReceiver should throw an ReferenceError]*/ 
function AmqpReceiver(amqpReceiver) {
  if (!amqpReceiver) { throw new ReferenceError('amqpReceiver'); }
  EventEmitter.call(this);
  this._amqpReceiver = amqpReceiver;
  this._listenersInitialized = false;

  this.on('newListener', function () {
    // lazy-init AMQP event listeners
    if (!this._listenersInitialized) {
      this._setupAmqpReceiverListeners();
    }
  }.bind(this));

  this.on('removeListener', function () {
    // stop listening for AMQP events if our consumers stop listening for our events
    if (this._listenersInitialized) {
      this._removeAmqpReceiverListeners();
    }
  }.bind(this));
}

util.inherits(AmqpReceiver, EventEmitter);

AmqpReceiver.prototype._onAmqpErrorReceived = function (err) {
    /**
     * @event module:azure-iot-amqp-base.AmqpReceiver#errorReceived
     * @type {Error}
     */
  this.emit('errorReceived', err);
};
  
AmqpReceiver.prototype._onAmqpMessage = function (message) {
    /**
     * @event module:azure-iot-amqp-base.AmqpReceiver#message
     * @type {AmqpMessage}
     */
  this.emit('message', message);
};
  
AmqpReceiver.prototype._setupAmqpReceiverListeners = function () {
  this._amqpReceiver.on('errorReceived', this._onAmqpErrorReceived.bind(this));
  this._amqpReceiver.on('message', this._onAmqpMessage.bind(this));
  this._listenersInitialized = true;
};

AmqpReceiver.prototype._removeAmqpReceiverListeners = function () {
  this._amqpReceiver.removeListener('errorReceived', this._onAmqpErrorReceived.bind(this));
  this._amqpReceiver.removeListener('message', this._onAmqpMessage.bind(this));
  this._listenersInitialized = false;
};


/**
 * @method          module:azure-iot-amqp-base.AmqpReceiver#complete
 * @description     Sends a completion feedback message to the service.
 * @param {AmqpMessage}   message  The message that is being settled. 
 */ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_006: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/ 
AmqpReceiver.prototype.complete = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.accept(message);
  if(done) done();
};

/** 
 * @method          module:azure-iot-amqp-base.AmqpReceiver#abandon
 * @description     Sends an abandon/release feedback message 
 * @param {AmqpMessage}   message  The message that is being settled.
*/ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_008: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/ 
AmqpReceiver.prototype.abandon = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.release(message);
  if(done) done();
};

/** 
 * @method          module:azure-iot-amqp-base.AmqpReceiver#reject
 * @description     Sends an reject feedback message 
 * @param {AmqpMessage}   message  The message that is being settled.
*/ 
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_010: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/ 
AmqpReceiver.prototype.reject = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.reject(message);
  if(done) done();
};

module.exports = AmqpReceiver;