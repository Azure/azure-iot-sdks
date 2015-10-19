// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('./connection_string.js');
var Message = require('azure-iot-common').Message;
var ServiceToken = require('../node_modules/azure-iot-common/lib/authorization.js').ServiceToken;
var SharedAccessSignature = require('azure-iot-common').SharedAccessSignature;

var DefaultTransport = require('./transport.js');

function Client(transport) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
  if (!transport) throw new ReferenceError('transport is \'' + transport + '\'');
  this._transport = transport;
}

Client.fromConnectionString = function fromConnectionString(value) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_002: [The fromConnectionString method shall throw ReferenceError if the value argument is falsy.]*/
  if (!value) throw new ReferenceError('value is \'' + value + '\'');

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Transport).]*/
  var cn = ConnectionString.parse(value);
  var sas = new ServiceToken(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, anHourFromNow());

  var config = {
    hubName: cn.HostName.split('.', 1)[0],
    host: cn.HostName,
    keyName: cn.SharedAccessKeyName,
    sharedAccessSignature: sas
  };

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_004: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
  return new Client(new DefaultTransport(config));
};

Client.fromSharedAccessSignature = function fromSharedAccessSignature(value) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_005: [The fromSharedAccessSignature method shall throw ReferenceError if the value argument is falsy.]*/
  if (!value) throw new ReferenceError('value is \'' + value + '\'');

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_006: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Transport).]*/
  var sas = SharedAccessSignature.parse(value);

  var config = {
    hubName: sas.sr.split('.', 1)[0],
    host: sas.sr,
    keyName: sas.skn,
    sharedAccessSignature: sas
  };

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_007: [The fromSharedAccessSignature method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
  return new Client(new DefaultTransport(config));
};

Client.prototype.open = function open(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_008: [The open method shall open a connection to the IoT Hub that was identified when the Client object was created (e.g., in Client.fromConnectionString).]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_009: [When the open method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_010: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_011: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_012: [If the connection is already open when open is called, it shall have no effect—that is, the done callback shall be invoked immediately with a null argument.]*/
  this._transport.connect(done);
};

Client.prototype.close = function close(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_021: [The close method shall close the connection.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_022: [When the close method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_023: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_024: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_025: [If the connection is not open when close is called, it shall have no effect— that is, the done callback shall be invoked immediately with null arguments.]*/
  this._transport.disconnect(done);
};

Client.prototype.send = function send(deviceId, message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_013: [The send method shall throw ReferenceError if the deviceId or message arguments are falsy.]*/
  if (!deviceId) {
    throw new ReferenceError('deviceId is \'' + deviceId + '\'');
  }
  if (!message) {
    throw new ReferenceError('message is \'' + message + '\'');
  }
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_014: [The send method shall convert the message object to type azure-iot-common.Message if necessary.]*/
  if (!(message instanceof Message)) {
    message = new Message(message);
  }

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_015: [If the connection has not already been opened (e.g., by a call to open), the send method shall open the connection before attempting to send the message.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_016: [When the send method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)
  response - an implementation-specific response object returned by the underlying protocol, useful for logging and troubleshooting]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_017: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_018: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_019: [If the deviceId has not been registered with the IoT Hub, send shall return an instance of DeviceNotFoundError.]*/
  /*SRS_NODE_IOTHUB_CLIENT_05_020: [If the queue which receives messages on behalf of the device is full, send shall return and instance of DeviceMaximumQueueDepthExceededError.]*/
  this._transport.send(deviceId, message, done);
};

module.exports = Client;

function anHourFromNow () {
  var raw = (Date.now() / 1000) + 3600;
  return Math.ceil(raw);
}
