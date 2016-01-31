// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var ConnectionString = require('./connection_string.js');
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
 * @method            module:azure-iot-device.Client#open
 * @description       Call the transport layer CONNECT function if the
 *                    transport layer implements it
 * 
 * @param {Function} done       The callback to be invoked when `sendEvent`
 *                              completes execution.
 * @returns calls the given callback
 */
Client.prototype.open = function (done) {
  /* Codes_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport’s connect function, if it exists.] */
  if (typeof this._transport.connect === 'function') {
    this._transport.connect(function (err, res) {
      done(err, res);
    });
  } else {
      // No connect method on the transport.
      done();
  } 
};

/*Codes_SRS_NODE_DEVICE_CLIENT_05_016: [When a Client method encounters an error in the transport, the callback function (indicated by the done argument) shall be invoked with the following arguments:
err - the standard JavaScript Error object, with a response property that points to a transport-specific response object, and a responseBody property that contains the body of the transport response.]*/
/*Codes_SRS_NODE_DEVICE_CLIENT_05_017: [With the exception of receive, when a Client method completes successfully, the callback function (indicated by the done argument) shall be invoked with the following arguments:
err - null
response - a transport-specific response object]*/


/**
 * @method            module:azure-iot-device.Client#sendEvent
 * @description       The [sendEvent]{@linkcode Client#sendEvent} method sends an event message
 *                    to the IoT Hub as the device indicated by the connection string passed
 *                    via the constructor.
 * 
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
 * @method            module:azure-iot-device.Client#sendEventBatch
 * @description       The [sendEventBatch]{@linkcode Client#sendEventBatch} method sends a list
 *                    of event messages to the IoT Hub as the device indicated by the connection
 *                    string passed via the constructor.
 * 
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
 * @method            module:azure-iot-device.Client#getReceiver
 * @description       The [getReceiver]{@linkcode Client#getReceiver} method returns the receiver object that is used to get messages and settle them.
 * 
 * @param {Function} done The callback to be invoked with the receiver object as a parameter.
 */
Client.prototype.getReceiver = function (done) {
  this._transport.getReceiver(done);
};

/**
 * The `close` method directs the transport to close the current connection to the IoT Hub instance
 * 
 * @param {Function}    done    The callback to be invoked when the connection has been closed.
 */
Client.prototype.close = function (done) {
    /* Codes_SRS_NODE_DEVICE_CLIENT_16_001: [The close function shall call the transport’s disconnect function if it exists.] */
    if (typeof this._transport.disconnect === 'function') {    
        this._transport.disconnect(done);
    }
};

module.exports = Client;
