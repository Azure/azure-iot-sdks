// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var Message = require('azure-iot-common').Message;
var results = require('azure-iot-common').results;

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

  var self = this;

  this.on('removeListener', function () {
    // stop listening for AMQP events if our consumers stop listening for our events
    if (self._listenersInitialized && self.listeners('message').length === 0) {
      self._removeAmqpReceiverListeners();
    }
  });

  this.on('newListener', function () {
    // lazy-init AMQP event listeners
    if (!self._listenersInitialized) {
      self._setupAmqpReceiverListeners();
    }
  });
}

util.inherits(AmqpReceiver, EventEmitter);

AmqpReceiver.prototype._onAmqpErrorReceived = function (err) {
  /**
   * @event module:azure-iot-amqp-base.AmqpReceiver#errorReceived
   * @type {Error}
   */
  this.emit('errorReceived', err);
};

AmqpReceiver.prototype._onAmqpMessage = function (amqpMessage) {
  /**
   * @event module:azure-iot-amqp-base.AmqpReceiver#message
   * @type {Message}
   */
  var msg = new Message();

  if (amqpMessage.properties.to) {
    msg.to = amqpMessage.properties.to;
  }

  if (amqpMessage.properties.absoluteExpiryTime) {
    msg.expiryTimeUtc = amqpMessage.properties.absoluteExpiryTime;
  }

  if (amqpMessage.properties.messageId) {
    msg.messageId = amqpMessage.properties.messageId;
  }

  if (amqpMessage.body) {
    msg.data = amqpMessage.body;
  }

  /*Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_13_001: [ If the AMQP message has values in it's applicationProperties property then those shall be added to the properties property of the newly created message object. ]*/
  if(amqpMessage.applicationProperties) {
    var appProps = amqpMessage.applicationProperties;
    for (var key in appProps) {
      if (appProps.hasOwnProperty(key)) {
        msg.properties.add(key, appProps[key], '');
      }
    }
  }

  msg.transportObj = amqpMessage;

  this.emit('message', msg);
};

AmqpReceiver.prototype._setupAmqpReceiverListeners = function () {
  this._listeners = [
    { eventName: 'errorReceived', listener: this._onAmqpErrorReceived.bind(this) },
    { eventName: 'message', listener: this._onAmqpMessage.bind(this) }
  ];
  for(var i = 0; i < this._listeners.length; ++i) {
    var listener = this._listeners[i];
    this._amqpReceiver.on(listener.eventName, listener.listener);
  }

  this._listenersInitialized = true;
};

AmqpReceiver.prototype._removeAmqpReceiverListeners = function () {
  if(this._listenersInitialized === true) {
    for(var i = 0; i < this._listeners.length; ++i) {
      var listener = this._listeners[i];
      this._amqpReceiver.removeListener(listener.eventName, listener.listener);
    }

    this._listenersInitialized = false;
  }
};

/**
 * @method          module:azure-iot-amqp-base.AmqpReceiver#complete
 * @description     Sends a completion feedback message to the service.
 * @param {AmqpMessage}   message  The message that is being settled.
 */
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_006: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/
AmqpReceiver.prototype.complete = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.accept(message.transportObj);
  if (done) done(null, new results.MessageCompleted());
};

/**
 * @method          module:azure-iot-amqp-base.AmqpReceiver#abandon
 * @description     Sends an abandon/release feedback message
 * @param {AmqpMessage}   message  The message that is being settled.
 */
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_008: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/
AmqpReceiver.prototype.abandon = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.release(message.transportObj);
  if (done) done(null, new results.MessageAbandoned());
};

/**
 * @method          module:azure-iot-amqp-base.AmqpReceiver#reject
 * @description     Sends an reject feedback message
 * @param {AmqpMessage}   message  The message that is being settled.
 */
/* Codes_SRS_NODE_IOTHUB_AMQPRECEIVER_16_010: [If the message object passed as an argument is falsy, a ReferenceError should be thrown]*/
AmqpReceiver.prototype.reject = function (message, done) {
  if (!message) { throw new ReferenceError('Invalid message object.'); }
  this._amqpReceiver.reject(message.transportObj);
  if (done) done(null, new results.MessageRejected());
};

module.exports = AmqpReceiver;