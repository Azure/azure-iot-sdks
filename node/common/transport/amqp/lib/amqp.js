// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var amqp10 = require('amqp10');
var AmqpMessage = require('./amqp_message.js');
var AmqpReceiver = require('./amqp_receiver.js');
var errors = require('azure-iot-common').errors;
var results = require('azure-iot-common').results;
var debug = require('debug')('amqp-common');

/**
 * @class module:azure-iot-amqp-base.Amqp
 * @classdesc Basic AMQP functionality used by higher-level IoT Hub libraries.
 *            Usually you'll want to avoid using this class and instead rely on higher-level implementations
 *            of the AMQP transport (see [azure-iot-device-amqp.Amqp]{@link module:azure-iot-device-amqp.Amqp} for example).
 *
 * @param   {String}    saslPlainUri            URL to the IoT Hub instance including SASL-Plain credentials.
 * @param   {Boolean}   autoSettleMessages      Boolean indicating whether messages should be settled automatically or if the calling code will handle it.
 * @param   {String}    sdkVersionString        String identifying the SDK used (device or service).
 */

/*Codes_SRS_NODE_COMMON_AMQP_16_001: [The Amqp constructor shall accept three parameters:
    A SASL-Plain URI to be used to connect to the IoT Hub instance
    A Boolean indicating whether the client should automatically settle messages:
        True if the messages should be settled automatically
        False if the caller intends to manually settle messages
        A string containing the version of the SDK used for telemetry purposes] */
function Amqp(saslPlainUri, autoSettleMessages, sdkVersionString) {
  var autoSettleMode = autoSettleMessages ? amqp10.Constants.receiverSettleMode.autoSettle : amqp10.Constants.receiverSettleMode.settleOnDisposition;
  // node-amqp10 has an automatic reconnection/link re-attach feature that is enabled by default.
  // In our case we want to control the reconnection flow ourselves, so we need to disable it.
  this._amqp = new amqp10.Client(amqp10.Policy.merge({
    senderLink: {
      attach: {
        properties: {
          'com.microsoft:client-version': sdkVersionString
        },
        maxMessageSize: 9223372036854775807,
      },
      encoder: function(body) {
        if(typeof body === 'string') {
          return new Buffer(body, 'utf8');
        } else {
          return body;
        }
      },
      reattach: {
        retries: 0,
        forever: false
      }
    },
    receiverLink: {
      attach: {
        properties: {
          'com.microsoft:client-version': sdkVersionString
        },
        maxMessageSize: 65536,
        receiverSettleMode: autoSettleMode,
      },
      decoder: function(body) { return body; },
      reattach: {
        retries: 0,
        forever: false
      }
    },
    // reconnections will be handled at the client level, not the transport level.
    reconnect: {
      retries: 0,
      strategy: 'fibonnaci',
      forever: false
    }
  }, amqp10.Policy.EventHub));

  this._amqp.on('client:errorReceived', function (err) {
    debug('Error received from node-amqp10: ' + err.message);
  }.bind(this));

  this._receiverPromise = null;
  this._receiver = null;
  this._sender = null;
  this._connected = false;

  this.uri = saslPlainUri;

  if (this.uri.substring(0, 3) === 'wss') {
    var wsTransport = require('amqp10-transport-ws');
    wsTransport.register(amqp10.TransportProvider);
  }
}

/**
 * @method             module:azure-iot-amqp-base.Amqp#connect
 * @description        Establishes a connection with the IoT Hub instance.
 * @param {Function}   done   Called when the connection is established of if an error happened.
 */
Amqp.prototype.connect = function connect(done) {
  if (!this._connected) {
    this._amqp.connect(this.uri)
      .then(function (result) {
        debug('AMQP transport connected.');
        this._connected = true;
        /*Codes_SRS_NODE_COMMON_AMQP_16_002: [The connect method shall establish a connection with the IoT hub instance and call the done() callback if given as argument] */
        if (done) done(null, new results.Connected(result));
        return null;
      }.bind(this))
      .catch(function (err) {
        this._connected = false;
        /*Codes_SRS_NODE_COMMON_AMQP_16_003: [The connect method shall call the done callback if the connection fails.] */
        if (done) done(err);
      }.bind(this));
  } else {
    debug('connect called when already connected.');
    if (done) done(null, new results.Connected());
  }
};

/**
 * @method             module:azure-iot-amqp-base.Amqp#setDisconnectCallback
 * @description        Sets the callback that should be called in case of disconnection.
 * @param {Function}   disconnectCallback   Called when the connection disconnected.
 */
Amqp.prototype.setDisconnectHandler = function (disconnectCallback) {
  this._amqp.on('connection:closed', function (err) {
    this._connected = false;
    disconnectCallback(err);
  }.bind(this));
};


/**
 * @method             module:azure-iot-amqp-base.Amqp#disconnect
 * @description        Disconnects the link to the IoT Hub instance.
 * @param {Function}   done   Called when disconnected of if an error happened.
 */
Amqp.prototype.disconnect = function disconnect(done) {
  this._amqp.disconnect()
    .then(function (result) {
      this._connected = false;
      /*Codes_SRS_NODE_COMMON_AMQP_16_004: [The disconnect method shall call the done callback when the application/service has been successfully disconnected from the service] */
      if (done) done(null, result);
      return null;
    }.bind(this))
    .catch(function (err) {
      /*SRS_NODE_COMMON_AMQP_16_005: [The disconnect method shall call the done callback and pass the error as a parameter if the disconnection is unsuccessful] */
      if (done) done(err);
    });
};

/**
 * @method             module:azure-iot-amqp-base.Amqp#send
 * @description        Sends a message to the IoT Hub instance.
 *
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
  if (!this._connected) done(new errors.NotConnectedError('Cannot send while disconnected.'));

  var sendAction = function (sender, msg, done) {
    sender.send(msg)
      .then(function (state) {
        if (done) {
          var result = new results.MessageEnqueued(state);
          done(null, result);
        }
      })
      .catch(function (err) {
        /*Codes_SRS_NODE_IOTHUB_AMQPCOMMON_16_007: [If sendEvent encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
        if (done) done(err);
      });
  };

  if (!this._sender) {
    this._amqp.createSender(endpoint)
      .then(function (sender) {
        this._sender = sender;
        /*Codes_SRS_NODE_COMMON_AMQP_16_007: [If send encounters an error before it can send the request, it shall invoke the done callback function and pass the standard JavaScript Error object with a text description of the error (err.message).]*/
        this._sender.on('errorReceived', function (err) {
          if (done) done(err);
          return null;
        });

        sendAction(this._sender, amqpMessage, done);
        return null;
      }.bind(this));
  } else {
    sendAction(this._sender, amqpMessage, done);
  }
};

/**
 * @method             module:azure-iot-amqp-base.Amqp#getReceiver
 * @description        Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 *
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
  if (!this._receiverPromise) this._receiverPromise = this._amqp.createReceiver(endpoint);

  this._receiverPromise
    .then(function (receiver) {
      this._receiver = new AmqpReceiver(receiver);
      debug('AmqpReceiver object created');
      done(null, this._receiver);
    }.bind(this))
    .catch(function (err) {
      if (done) done(err);
    });
};

module.exports = Amqp;