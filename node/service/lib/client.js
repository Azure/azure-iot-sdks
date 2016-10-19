// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');
var anHourFromNow = require('azure-iot-common').anHourFromNow;
var results = require('azure-iot-common').results;
var ConnectionString = require('./connection_string.js');
var DefaultTransport = require('./amqp.js');
var Message = require('azure-iot-common').Message;
var SharedAccessSignature = require('./shared_access_signature.js');
var DeviceMethod = require('./device_method.js');
var RestApiClient = require('./rest_api_client.js');

/**
 * @class           module:azure-iothub.Client
 * @classdesc       Creates an IoT Hub service client. Normally, consumers will
 *                  call one of the factory methods,
 *                  {@link module:azure-iothub.Client.fromConnectionString|fromConnectionString} or
 *                  {@link module:azure-iothub.Client.fromSharedAccessSignature|fromSharedAccessSignature},
 *                  to create an IoT Hub service Client.
 * @param {Object}       transport   An object that implements the interface
 *                                   expected of a transport object, e.g.,
 *                                   {@link module:azure-iothub~Transport|Transport}.
  * @prop {AmqpReceiver} FeedbackReceiver
 */
function Client(transport, restApiClient) {
  EventEmitter.call(this);
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
  if (!transport) throw new ReferenceError('transport is \'' + transport + '\'');
  this._transport = transport;

  this._restApiClient = restApiClient;

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_030: [The FeedbackReceiver class shall inherit EventEmitter to provide consumers the ability to listen for (and stop listening for) events.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_031: [FeedbackReceiver shall expose the 'errorReceived' event, whose handler shall be called with the following arguments:
  err – standard JavaScript Error object (or subclass)]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_032: [FeedbackReceiver shall expose the 'message' event, whose handler shall be called with the following arguments when a new feedback message is received from the IoT Hub:
  message – a JavaScript object containing a batch of one or more feedback records]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_033: [getFeedbackReceiver shall return the same instance of Client.FeedbackReceiver every time it is called with a given instance of Client.]*/
  Object.defineProperty(this, 'FeedbackReceiver', { value: transport.FeedbackReceiver });
}

util.inherits(Client, EventEmitter);

/**
 * @method            module:azure-iothub.Client.fromConnectionString
 * @description       Creates an IoT Hub service client from the given
 *                    connection string using the default transport
 *                    ({@link module:azure-iothub~Transport|Transport}).
 *
 * @param {String}    connStr       A connection string which encapsulates "device
 *                                  connect" permissions on an IoT hub.
 * @param {Function}  Transport     A transport constructor.
 *
 * @returns {module:azure-iothub.Client}
*/
Client.fromConnectionString = function fromConnectionString(connStr, Transport) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_002: [The fromConnectionString method shall throw ReferenceError if the connStr argument is falsy.]*/
  if (!connStr) throw new ReferenceError('connStr is \'' + connStr + '\'');

  if(!Transport){
      Transport = DefaultTransport;
  }

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_003: [Otherwise, it shall derive and transform the needed parts from the connection string in order to create a new instance of the default transport (azure-iothub.Transport).]*/
  var cn = ConnectionString.parse(connStr);

  var config = {
    hubName: cn.HostName.split('.', 1)[0],
    host: cn.HostName,
    keyName: cn.SharedAccessKeyName,
    sharedAccessSignature: SharedAccessSignature.create(cn.HostName, cn.SharedAccessKeyName, cn.SharedAccessKey, anHourFromNow())
  };

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_004: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
  return new Client(new Transport(config), new RestApiClient(config));
};

/**
 * @method            module:azure-iothub.Client.fromSharedAccessSignature
 * @description       Creates an IoT Hub service client from the given
 *                    shared access signature using the default transport
 *                    ({@link module:azure-iothub~Transport|Transport}).
 *
 * @param {String}    sharedAccessSignature   A shared access signature which encapsulates
 *                            "service connect" permissions on an IoT hub.
 * @param {Function}  Transport     A transport constructor.
 *
 * @returns {module:azure-iothub.Client}
 */
Client.fromSharedAccessSignature = function fromSharedAccessSignature(sharedAccessSignature, Transport) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_005: [The fromSharedAccessSignature method shall throw ReferenceError if the sharedAccessSignature argument is falsy.]*/
  if (!sharedAccessSignature) throw new ReferenceError('sharedAccessSignature is \'' + sharedAccessSignature + '\'');

  if(!Transport){
      Transport = DefaultTransport;
  }
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_006: [Otherwise, it shall derive and transform the needed parts from the shared access signature in order to create a new instance of the default transport (azure-iothub.Transport).]*/
  var sas = SharedAccessSignature.parse(sharedAccessSignature);
  var decodedUri = decodeURIComponent(sas.sr);

  var config = {
    hubName: decodedUri.split('.', 1)[0],
    host: decodedUri,
    keyName: sas.skn,
    sharedAccessSignature: sas.toString()
  };

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_007: [The fromSharedAccessSignature method shall return a new instance of the Client object, as by a call to new Client(transport).]*/
  return new Client(new Transport(config));
};

Client.prototype._disconnectHandler = function (reason) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_004: [** The `disconnect` event shall be emitted when the client is disconnected from the server.]*/
  var evt = new results.Disconnected();
  evt.reason = reason;
  this.emit('disconnect', evt);
};

/**
 * @method            module:azure-iothub.Client#open
 * @description       Opens the connection to an IoT hub.
 * @param {Function}  done    The function to call when the operation is
 *                            complete. `done` will be passed an Error object
 *                            argument, which will be null if the operation
 *                            completed successfully.
 */
Client.prototype.open = function open(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_008: [The open method shall open a connection to the IoT Hub that was identified when the Client object was created (e.g., in Client.fromConnectionString).]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_009: [When the open method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_010: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_011: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_012: [If the connection is already open when open is called, it shall have no effect—that is, the done callback shall be invoked immediately with a null argument.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_006: [The `open` method should not throw if the `done` callback is not specified.]*/
  var self = this;
  this._transport.connect(function(err, result) {
    if (err) {
      if (done) done(err);
    } else {
      /*Codes_SRS_NODE_IOTHUB_CLIENT_16_002: [If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport.]*/
      self._transport.on('disconnect', self._disconnectHandler.bind(self));
      if (done) done(null, result);
    }
  });
};

/**
 * @method            module:azure-iothub.Client#close
 * @description       Closes the connection to an IoT hub.
 * @param {Function}  done    The function to call when the operation is
 *                            complete. `done` will be passed an Error object
 *                            argument, which will be null if the operation
 *                            completed successfully.
 */
Client.prototype.close = function close(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_021: [The close method shall close the connection.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_022: [When the close method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_023: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_024: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_025: [If the connection is not open when close is called, it shall have no effect— that is, the done callback shall be invoked immediately with null arguments.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_005: [The `close` method should not throw if the `done` callback is not specified.]*/
  this._transport.disconnect(function (err, result) {
      if (err) {
        if (done) done(err);
      } else {
        /*Codes_SRS_NODE_IOTHUB_CLIENT_16_003: [The `close` method shall remove the listener that has been attached to the transport `disconnect` event.]*/
        this._transport.removeAllListeners('disconnect');
        if (done) done(null, result);
      }
    }.bind(this));
};

/**
 * @method            module:azure-iothub.Client#send
 * @description       Sends a message to a device.
 * @param {String}    deviceId  The identifier of an existing device identity.
 * @param {}          message   The body of the message to send to the device.
 *                              If `message` is not of type
 *                              {@link module:azure-iot-common.Message|Message},
 *                              it will be converted.
 * @param {Function}  done      The function to call when the operation is
 *                              complete. `done` will be called with two
 *                              arguments: an Error object (can be null) and a
 *                              transport-specific response object useful for
 *                              logging or debugging.
 */
Client.prototype.send = function send(deviceId, message, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_013: [The send method shall throw ReferenceError if the deviceId or message arguments are falsy.]*/
  if (!deviceId) {
    throw new ReferenceError('deviceId is \'' + deviceId + '\'');
  }
  if (!message) {
    throw new ReferenceError('message is \'' + message + '\'');
  }
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_014: [The send method shall convert the message object to type azure-iot-common.Message if necessary.]*/
  if (message.constructor.name !== 'Message') {
    message = new Message(message);
  }

  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_015: [If the connection has not already been opened (e.g., by a call to open), the send method shall open the connection before attempting to send the message.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_016: [When the send method completes, the callback function (indicated by the done argument) shall be invoked with the following arguments:
  err - standard JavaScript Error object (or subclass)
  response - an implementation-specific response object returned by the underlying protocol, useful for logging and troubleshooting]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_017: [The argument err passed to the callback done shall be null if the protocol operation was successful.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_018: [Otherwise the argument err shall have a transport property containing implementation-specific response information for use in logging and troubleshooting.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_019: [If the deviceId has not been registered with the IoT Hub, send shall return an instance of DeviceNotFoundError.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_020: [If the queue which receives messages on behalf of the device is full, send shall return and instance of DeviceMaximumQueueDepthExceededError.]*/
  this._transport.send(deviceId, message, done);
};

/**
 * @method            module:azure-iothub.Client#invokeDeviceMethod
 * @description       Invokes a method on a particular device.
 * @param {String}    deviceId            The identifier of an existing device identity.
 * @param {Object}    params              An object describing the method and shall have the following properties: 
 *                                        - methodName          The name of the method that shall be invoked.
 *                                        - payload             [optional] The payload to use for the method call.
 *                                        - timeoutInSeconds    [optional] The number of seconds IoT Hub shall wait for the device 
 *                                                              to send a response before deeming the method execution a failure.
 * @param {Function}  done                The callback to call with the result of the method execution.
 * 
 * @throws {ReferenceError}  If one of the required parameters is null, undefined or empty.
 * @throws {TypeError}       If one of the parameters is of the wrong type.
 */
Client.prototype.invokeDeviceMethod = function (deviceId, methodParams, done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_014: [The `invokeDeviceMethod` method shall throw a `ReferenceError` if `deviceId` is `null`, `undefined` or an empty string.]*/
  if (deviceId === undefined || deviceId === null || deviceId === '') throw new ReferenceError('deviceId cannot be \'' + deviceId + '\'');

  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_009: [The `invokeDeviceMethod` method shall initialize a new `DeviceMethod` instance with the `methodName`, `payload` and `timeout` values passed in the arguments.]*/
  var method = new DeviceMethod(methodParams, this._restApiClient);

  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_010: [The `invokeDeviceMethod` method shall use the newly created instance of `DeviceMethod` to invoke the method on the device specified with the `deviceid` argument .]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_012: [The `invokeDeviceMethod` method shall call the `done` callback with a standard javascript `Error` object if the request failed.]*/
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_013: [The `invokeDeviceMethod` method shall call the `done` callback with a `null` first argument, the result of the method execution in the second argument, and the transport-specific response object as a third argument.]*/
  method.invokeOn(deviceId, done);
};


/**
 * @method            module:azure-iothub.Client#getFeedbackReceiver
 * @description       Returns a AmqpReceiver object which emits events when new feedback messages are received by the client.
 * @param {Function}  done      The function to call when the operation is
 *                              complete. `done` will be called with two
 *                              arguments: an Error object (can be null) and a
 *                              AmqpReceiver object.
 */
Client.prototype.getFeedbackReceiver = function getFeedbackReceiver(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_05_027: [When the `getFeedbackReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
  - `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
  - `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed]*/

  this._transport.getFeedbackReceiver(done);
};

/**
 * @method            module:azure-iothub.Client#getFileNotificationReceiver
 * @description       Returns a AmqpReceiver object which emits events when new file upload notifications are received by the client.
 * @param {Function}  done      The function to call when the operation is
 *                              complete. `done` will be called with two
 *                              arguments: an Error object (can be null) and a
 *                              AmqpReceiver object.
 */
Client.prototype.getFileNotificationReceiver = function getFileNotificationReceiver(done) {
  /*Codes_SRS_NODE_IOTHUB_CLIENT_16_001: [When the `getFileNotificationReceiver` method completes, the callback function (indicated by the `done` argument) shall be invoked with the following arguments:
- `err` - standard JavaScript `Error` object (or subclass): `null` if the operation was successful
- `receiver` - an `AmqpReceiver` instance: `undefined` if the operation failed]*/
  this._transport.getFileNotificationReceiver(done);
};

module.exports = Client;
