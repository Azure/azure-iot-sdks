// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').authorization.anHourFromNow;
var ArgumentError = require('azure-iot-common').errors.ArgumentError;
var ConnectionString = require('./connection_string.js');
var DefaultTransport = require('./http.js');
var SharedAccessSignature = require('./shared_access_signature.js');

/**
 * @class           module:azure-iot-device.Client
 * @classdesc       Creates an IoT Hub device client. Normally, consumers will
 *                  call the factory method,
 *                  {@link module:azure-iot-device.Client.fromConnectionString|fromConnectionString},
 *                  to create an IoT Hub device client.
 * @param {Object}  transport   An object that implements the interface
 *                              expected of a transport object, e.g.,
 *                              {@link module:azure-iot-device~Http|Http}.
 */
var Client = function (transport) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
  if (!transport) throw new ReferenceError('transport is \'' + transport + '\'');
  this._transport = transport;
};

/**
 * @method            module:azure-iot-device.Client.fromConnectionString
 * @description       Creates an IoT Hub device client from the given
 *                    connection string using the given transport type, or
 *                    {@link module:azure-iot-device~Http|Http} if no transport
 *                    type is given.
 * @param {String}    value   A connection string which encapsulates "device
 *                            connect" permissions on an IoT hub.
 * @returns {module:azure-iothub.Client}
*/
Client.fromConnectionString = function fromConnectionString(value, Transport) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_003: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
  if (!value) throw new ReferenceError('value is \'' + value + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_05_004: [If Transport is falsy, fromConnectionString shall use the default transport, Http.]*/
  Transport = Transport || DefaultTransport;

  /*Tests_SRS_NODE_DEVICE_CLIENT_05_005: [fromConnectionString shall derive and transform the needed parts from the connection string in order to create a new instance of Transport.]*/
  var cn = ConnectionString.parse(value);
  var sas = SharedAccessSignature.create(cn.HostName, cn.DeviceId, cn.SharedAccessKey, anHourFromNow());

  var config = {
    host: cn.HostName,
    deviceId: cn.DeviceId,
    hubName: cn.HostName.split('.')[0],
    sharedAccessSignature: sas.toString(),
    gatewayHostName: cn.GatewayHostName
  };

  /*Codes_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
  return new Client(new Transport(config));
};

/**
 * @method            module:azure-iot-device.Client.connect
 * @description       Call the transport layer CONNECT function if the
 *                    transport layer implements it
 * @returns calls the given callback
 */
Client.prototype.open = function (done) {
  /* Codes_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport connect function] */
  if (typeof this._transport.connect === 'function') {
    this._transport.connect(function () {
      done();
    });
  } 
};

/*Codes_SRS_NODE_DEVICE_CLIENT_05_016: [When a Client method encounters an error in the transport, the callback function (indicated by the done argument) shall be invoked with the following arguments:
err - the standard JavaScript Error object, with a response property that points to a transport-specific response object, and a responseBody property that contains the body of the transport response.]*/
/*Codes_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
err - null
response - a transport-specific response object]*/


/**
 * The [sendEvent]{@linkcode Client#sendEvent} method sends an event message
 * to the IoT Hub as the device indicated by the connection string passed
 * via the constructor.
 * @param {Message}  message    The [message]{@linkcode module:common/message.Message}
 *                              to be sent.
 * @param {Function} done       The callback to be invoked when `sendEvent`
 *                              completes execution.
 * @see [Message]{@linkcode module:common/message.Message}
 */
Client.prototype.sendEvent = function(message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_007: [The sendEvent method shall send the event indicated by the message argument via the transport associated with the Client instance.]*/
  this._transport.sendEvent(message, done);
};

/**
 * The [sendEventBatch]{@linkcode Client#sendEventBatch} method sends a list
 * of event messages to the IoT Hub as the device indicated by the connection
 * string passed via the constructor.
 * @param {array<Message>} messages Array of [Message]{@linkcode module:common/message.Message}
 *                                  objects to be sent as a batch.
 * @param {Function}      done      The callback to be invoked when
 *                                  `sendEventBatch` completes execution.
 */
Client.prototype.sendEventBatch = function(messages, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_008: [The sendEventBatch method shall send the list of events (indicated by the messages argument) via the transport associated with the Client instance.]*/
  this._transport.sendEventBatch(messages, done);
};

/**
 * The receive method queries the IoT Hub (as the device indicated in the
 * constructor argument) for the next message in the queue.
 * @param {Function}  done      The callback to be invoked when `receive`
 *                              completes execution.
 */
Client.prototype.receive = function(done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_009: [The receive method shall query the IoT Hub for the next message via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_018: [When receive completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - null
  message - the received Message (for receive), otherwise null
  response - a transport-specific response object]*/
  this._transport.receive(done);
};

/**
 * The getReceiver method returns the receiver object that is used to get messages and settle them.
 * @param {Function} done The callback to be invoked with the receiver object as a parameter.
 */
Client.prototype.getReceiver = function (done) {
  this._transport.getReceiver(done);
};

/**
 * The `abandon` method directs the IoT Hub to re-enqueue a message
 * message so it may be received again later.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be re-enqueued.
 * @param {Function}  done      The callback to be invoked when `abandon`
 *                              completes execution.
 */
Client.prototype.abandon = function(message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_010: [If message has a lockToken property, the abandon method shall abandon message via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_011: [Otherwise it shall invoke the done callback with ArgumentError.]*/
  message = message || {};

  if (!message.lockToken) {
    done(new ArgumentError('invalid lockToken'));
  }
  else {
    this._transport.sendFeedback('abandon', message, done);
  }
};

/**
 * The `reject` method directs the IoT Hub to delete a message
 * from the queue and record that it was rejected.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be deleted.
 * @param {Function}  done      The callback to be invoked when `reject`
 *                              completes execution.
 */
Client.prototype.reject = function(message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_012: [If message has a lockToken property, the reject method shall reject message via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_013: [Otherwise is shall invoke the done callback with ArgumentError.]*/
  message = message || {};

  if (!message.lockToken) {
    done(new ArgumentError('invalid lockToken'));
  }
  else {
    this._transport.sendFeedback('reject', message, done);
  }
};

/**
 * The `complete` method directs the IoT Hub to delete a message
 * from the queue and record that it was accepted.
 * @param {Message}   message   The [message]{@linkcode module:common/message.Message}
 *                              to be accepted.
 * @param {Function}  done      The callback to be invoked when `complete`
 *                              completes execution.
 */
Client.prototype.complete = function(message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_014: [If message has a lockToken property, the complete method shall complete message via the transport associated with the Client instance.]*/
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_015: [Otherwise is shall invoke the done callback with ArgumentError.]*/
  message = message || {};

  if (!message.lockToken) {
    done(new ArgumentError('invalid lockToken'));
  }
  else {
    this._transport.sendFeedback('complete', message,  done);
  }
};

module.exports = Client;
