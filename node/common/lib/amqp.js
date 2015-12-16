// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var amqp10 = require('amqp10');
var AmqpMessage = require('./amqp_message.js');
var errors = require('./errors.js');
var AmqpReceiver = require('./amqp_receiver.js');

/**
 * @class module:azure-iot-common.Amqp
 * @classdesc Basic AMQP functionality used by higher-level IoT Hub libraries.
 */

/*Codes_SRS_NODE_COMMON_AMQP_16_001: [The Amqp constructor shall accept two parameters:
A SASL-Plain URI to be used to connect to the IoT Hub instance
A Boolean indicating whether the client should automatically settle messages:
	True if the messages should be settled automatically
	False if the caller intends to manually settle messages] */
function Amqp(saslPlainUri, autoSettleMessages) {
  var autoSettleMode = autoSettleMessages ? amqp10.Constants.receiverSettleMode.autoSettle : amqp10.Constants.receiverSettleMode.settleOnDisposition;
  this._amqp = new amqp10.Client(amqp10.Policy.merge({
    senderLink: {
      reattach: {
        retries: 0,
        forever: false
      }
    },
    receiverLink: {
      attach: {
        receiverSettleMode: autoSettleMode,
      }
    }
  }, amqp10.Policy.EventHub));
  
  this._connectPromise = null;
  this._receiverPromise = null;
  this._receiver = null;
  this._sender = null;
  
  this.uri = saslPlainUri;
  
  if (this.uri.startsWith('wss')) {
    var wsTransport = require('amqp10-transport-ws');
    wsTransport.register(amqp10.TransportProvider);
  }
}

/**
 * Establishes a connection with the IoT Hub instance.
 * @param {Function}   done   Called when the connection is established of if an error happened.
 */
Amqp.prototype.connect = function connect(done) {
  if (!this._connectPromise) {
    this._amqp.on('client:errorReceived', function (err) {
      this.disconnect(function () {
        /*Codes_SRS_NODE_COMMON_AMQP_16_003: [The connect method shall call the done callback if the connection fails.] */
        if (done) done(translateError(err));
      });
    }.bind(this));
    this._connectPromise = this._amqp.connect(this.uri);
  }

  this._connectPromise
    .then(function () {
      /*Codes_SRS_NODE_COMMON_AMQP_16_002: [The connect method shall establish a connection with the IoT hub instance and call the done() callback if given as argument] */
      if (done) done();
      return null;
    })
    .catch(function (err) {
      /*Codes_SRS_NODE_COMMON_AMQP_16_003: [The connect method shall call the done callback if the connection fails.] */
      if (done) done(translateError(err));
    });
};

/**
 * Disconnects the link to the IoT Hub instance.
 * @param {Function}   done   Called when disconnected of if an error happened.
 */
Amqp.prototype.disconnect = function disconnect(done) {
  this._connectPromise = null;
  this._amqp.disconnect()
    .then(function () {
      /*Codes_SRS_NODE_COMMON_AMQP_16_004: [The disconnect method shall call the done callback when the application/service has been successfully disconnected from the service] */
      if (done) done();
      return null;
    })
    .catch(function (err) {
      /*SRS_NODE_COMMON_AMQP_16_005: [The disconnect method shall call the done callback and pass the error as a parameter if the disconnection is unsuccessful] */
      if (done) done(err);
    });
};

/**
 * Sends a message to the IoT Hub instance.
 * @param {Message}   message   The message to send.
 * @param {string}    endpoint  The endpoint to use when sending the message.
 * @param {string}    to        The destination of the message.
 * @param {Function}  done      Called when the message is sent or if an error happened.
 */
Amqp.prototype.send = function send(message, endpoint, to, done) {  
  /*Codes_SRS_NODE_COMMON_AMQP_16_006: [The send method shall construct an AMQP message using information supplied by the caller, as follows:
	The ‘to’ field of the message should be set to the ‘to’ argument.
	The ‘body’ of the message should be built using the message argument.] */
  
  var amqpMessage = AmqpMessage.fromMessage(message);
  amqpMessage.properties.to = to;
  
  /*Codes_SRS_NODE_COMMON_AMQP_16_008: [If the device or application is not connected to the IoT hub service, the send method should establish the connection before trying to send the message] */
  this.connect(function (err) {
    if (err) {
      if (done) done(err);
    }
    else {
      var sendAction = function (sender, msg, done) {
        sender.send(msg)
              .then(function (state) {
                  if (done) done(null, state);
              })
              .catch(function (err) {
                /*Codes_SRS_NODE_IOTHUB_AMQPCOMMON_16_007: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
                if (done) done(translateError(err));
              });
      };
      
      if (!this._sender) {
        this._amqp.createSender(endpoint)
          .then(function (sender) {
            this._sender = sender;
            /*Codes_SRS_NODE_COMMON_AMQP_16_007: [If send encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
            this._sender.on('errorReceived', function (err) {
              if (done) done(translateError(err));
              return null;
            });
            
            sendAction(this._sender, amqpMessage, done);
            return null;
          }.bind(this));
      } else {
        sendAction(this._sender, amqpMessage, done);
      }
    }
  }.bind(this));
};

/**
 * Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 * @param {string}    endpoint  Endpoint used for the receiving link.
 * @param {Function}  done      Callback used to return the {@linkcode AmqpReceiver} object.
 */
Amqp.prototype.getReceiver = function getReceiver(endpoint, done) {
  /*Codes_SRS_NODE_COMMON_AMQP_16_010: [If a receiver for this endpoint doesn’t exist, the getReceiver method should create a new AmqpReceiver object and then call the done() method with the object that was just created as an argument.] */
  if (!this._receiver) {
    this._setupReceiverLink(endpoint, done);
  }
  else {
    /*Codes_SRS_NODE_COMMON_AMQP_16_009: [If a receiver for this endpoint has already been created, the getReceiver method should call the done() method with the existing instance as an argument.] */
    done(null, this._receiver);
  }
};

Amqp.prototype._setupReceiverLink = function setupReceiverLink(endpoint, done) {
  this.connect(function (err) {
    if (err) {
      if (done) done(err);
    }
    else {
      if (!this._receiverPromise) {
        this._receiverPromise = this._amqp.createReceiver(endpoint);
      }

      this._receiverPromise
        .then(function (receiver) {
          this._receiver = new AmqpReceiver(receiver);
          done(null, this._receiver);
        }.bind(this))
        .catch(function (err) {
          if (done) done(translateError(err));
        });
    }
  }.bind(this));
};

function translateError(err) {
  var error = err;

  if (err.constructor.name === 'AMQPError') {
    if (err.condition.contents === 'amqp:resource-limit-exceeded') {
      error = new errors.DeviceMaximumQueueDepthExceededError(err.description);
    }
    else if (err.condition.contents === 'amqp:not-found') {
      error = new errors.DeviceNotFoundError(err.description);
    }
    else if (err.condition.contents === 'amqp:unauthorized-access') {
      error = new errors.UnauthorizedError(err.description);
    }
    else {
      error = new Error(err.description);
    }
    error.transport = err;
  }
  else if (err instanceof amqp10.Errors.AuthenticationError) {
    error = new errors.UnauthorizedError(err.message);
  }

  return error;
}

module.exports = Amqp;