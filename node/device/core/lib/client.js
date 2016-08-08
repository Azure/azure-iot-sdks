// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var results = require('azure-iot-common').results;
var errors = require('azure-iot-common').errors;
var ConnectionString = require('./connection_string.js');
var SharedAccessSignature = require('./shared_access_signature.js');
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var debug = require('debug')('azure-iot-device.Client');
var BlobUploadClient = require('./blob_upload').BlobUploadClient;

/**
 * @class           module:azure-iot-device.Client
 * @classdesc       Creates an IoT Hub device client. Normally, consumers will
 *                  call the factory method,
 *                  {@link module:azure-iot-device.Client.fromConnectionString|fromConnectionString},
 *                  to create an IoT Hub device client.
 * @param {Object}  transport         An object that implements the interface
 *                                    expected of a transport object, e.g.,
 *                                    {@link module:azure-iot-device~Http|Http}.
 * @param {String}  connStr           A connection string (optional: when not provided, updateSharedAccessSignature must be called to set the SharedAccessSignature token directly).
 * @param {Object}  blobUploadClient  An object that is capable of uploading a stream to a blob.
 */
var Client = function (transport, connStr, blobUploadClient) {
  EventEmitter.call(this);
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_001: [The Client constructor shall throw ReferenceError if the transport argument is falsy.]*/
  if (!transport) throw new ReferenceError('transport is \'' + transport + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_026: [The Client constructor shall accept a connection string as an optional second argument] */
  this._connectionString = connStr;

  if (this._connectionString && ConnectionString.parse(this._connectionString).SharedAccessKey) {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_027: [If a connection string argument is provided and is using SharedAccessKey authentication, the Client shall automatically generate and renew SAS tokens.] */
    this._sharedAccessSignatureRenewalInterval = setInterval(this._renewSharedAccessSignature.bind(this), 2700000); // SAS token created by the client have a lifetime of 60 minutes, renew every 45 minutes
  }

  this.blobUploadClient = blobUploadClient;

  this._transport = transport;
  this._receiver = null;

  this.on('removeListener', function (eventName) {
    if (this._receiver && eventName === 'message' && this.listeners('message').length === 0) {
      this._disconnectReceiver();
    }
  });

  this.on('newListener', function (eventName) {
    if (!this._receiver && eventName === 'message') {
      this._connectReceiver();
    }
  });
};

util.inherits(Client, EventEmitter);

Client.prototype._connectReceiver = function () {
  debug('Getting receiver object from the transport');
  this._transport.getReceiver(function (err, receiver) {
    if (!err) {
      debug('Subscribing to message events from the receiver object of the transport');
      this._receiver = receiver;
      this._receiver.on('message', function (msg) {
        this.emit('message', msg);
      }.bind(this));
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_006: [The ‘error’ event shall be emitted when an error occurred within the client code.] */
      this._receiver.on('errorReceived', function (err) {
        this.emit('error', err);
      }.bind(this));
    } else {
      throw new Error('Transport failed to start receiving messages: ' + err.message);
    }
  }.bind(this));
};

Client.prototype._disconnectReceiver = function () {
  if (this._receiver) {
    this._receiver.removeAllListeners('message');
    this._receiver = null;
  }
};

Client.prototype._renewSharedAccessSignature = function () {
  var cn = ConnectionString.parse(this._connectionString);
  var sas = SharedAccessSignature.create(cn.HostName, cn.DeviceId, cn.SharedAccessKey, anHourFromNow());

  this.updateSharedAccessSignature(sas.toString(), function (err) {
    if (err) {
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_006: [The ‘error’ event shall be emitted when an error occurred within the client code.] */
      this.emit('error', err);
    }
  }.bind(this));
};

/**
 * @method            module:azure-iot-device.Client.fromConnectionString
 * @description       Creates an IoT Hub device client from the given
 *                    connection string using the given transport type.
 *
 * @param {String}    connStr       A connection string which encapsulates "device
 *                                  connect" permissions on an IoT hub.
 * @param {Function}  Transport     A transport constructor.
 *
 * @throws {ReferenceError}         If the connStr parameter is falsy.
 *
 * @returns {module:azure-iothub.Client}
*/
Client.fromConnectionString = function fromConnectionString(connStr, Transport) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_003: [The fromConnectionString method shall throw ReferenceError if the connStr argument is falsy.]*/
  if (!connStr) throw new ReferenceError('connStr is \'' + connStr + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_05_005: [fromConnectionString shall derive and transform the needed parts from the connection string in order to create a new instance of Transport.]*/
  var cn = ConnectionString.parse(connStr);

  var config = {
    host: cn.HostName,
    deviceId: cn.DeviceId,
    hubName: cn.HostName.split('.')[0]
  };

  if (cn.hasOwnProperty('SharedAccessKey')) {
    var sas = SharedAccessSignature.create(cn.HostName, cn.DeviceId, cn.SharedAccessKey, anHourFromNow());
    config.sharedAccessSignature = sas.toString();
  }

  /*Codes_SRS_NODE_DEVICE_CLIENT_05_006: [The fromConnectionString method shall return a new instance of the Client object, as by a call to new Client(new Transport(...)).]*/
  return new Client(new Transport(config), connStr, new BlobUploadClient(config));
};

/**
 * @method            module:azure-iot-device.Client.fromSharedAccessSignature
 * @description       Creates an IoT Hub device client from the given
 *                    shared access signature using the given transport type.
 *
 * @param {String}    sharedAccessSignature      A shared access signature which encapsulates "device
 *                                  connect" permissions on an IoT hub.
 * @param {Function}  Transport     A transport constructor.
 *
 * @throws {ReferenceError}         If the connStr parameter is falsy.
 *
 * @returns {module:azure-iothub.Client}
*/
Client.fromSharedAccessSignature = function (sharedAccessSignature, Transport) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_029: [The fromSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature argument is falsy.] */
  if (!sharedAccessSignature) throw new ReferenceError('sharedAccessSignature is \'' + sharedAccessSignature + '\'');

  var sas = SharedAccessSignature.parse(sharedAccessSignature);
  var decodedUri = decodeURIComponent(sas.sr);
  var uriSegments = decodedUri.split('/');
  var config = {
    host: uriSegments[0],
    deviceId: uriSegments[uriSegments.length - 1],
    hubName: uriSegments[0].split('.')[0],
    sharedAccessSignature: sharedAccessSignature
  };

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_030: [The fromSharedAccessSignature method shall return a new instance of the Client object] */
  return new Client(new Transport(config), null, new BlobUploadClient(config));
};

/**
 * @method            module:azure-iot-device.Client#updateSharedAccessSignature
 * @description       Updates the Shared Access Signature token used by the transport to authenticate with the IoT Hub service.
 *
 * @param {String}   sharedAccessSignature   The new SAS token to use.
 * @param {Function} done       The callback to be invoked when `updateSharedAccessSignature`
 *                              completes execution.
 *
 * @throws {ReferenceError}     If the sharedAccessSignature parameter is falsy.
 */
Client.prototype.updateSharedAccessSignature = function (sharedAccessSignature, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_031: [The updateSharedAccessSignature method shall throw a ReferenceError if the sharedAccessSignature parameter is falsy.]*/
  if (!sharedAccessSignature) throw new ReferenceError('sharedAccessSignature is falsy');

  this.blobUploadClient.updateSharedAccessSignature(sharedAccessSignature);

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_032: [The updateSharedAccessSignature method shall call the updateSharedAccessSignature method of the transport currently inuse with the sharedAccessSignature parameter.]*/
  this._transport.updateSharedAccessSignature(sharedAccessSignature, function (err, result) {
    if (err) {
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_035: [The updateSharedAccessSignature method shall call the `done` callback with an error object if an error happened while renewng the token.]*/
      done(err);
    } else {
      debug('sas token updated: ' + result.constructor.name + ' needToReconnect: ' + result.needToReconnect);
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_033: [The updateSharedAccessSignature method shall reconnect the transport to the IoTHub service if it was connected before before the method is clled.]*/
      /*Codes_SRS_NODE_DEVICE_CLIENT_16_034: [The updateSharedAccessSignature method shall not reconnect the transport if the transport was disconnected to begin with.]*/
      if (result.needToReconnect) {
        if (this._receiver) this._disconnectReceiver();
        this.open(function (openErr) { // open will also automatically reconnect the receiver if still subscribed to the 'message' event
          if (openErr) {
            /*Codes_SRS_NODE_DEVICE_CLIENT_16_035: [The updateSharedAccessSignature method shall call the `done` callback with an error object if an error happened while renewng the token.]*/
            done(openErr);
          } else {
            /*Codes_SRS_NODE_DEVICE_CLIENT_16_036: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a result of type SharedAccessSignatureUpdated if the oken was updated successfully.]*/
            done(null, new results.SharedAccessSignatureUpdated(false));
          }
        });
      } else {
        /*Codes_SRS_NODE_DEVICE_CLIENT_16_036: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a result of type SharedAccessSignatureUpdated if the oken was updated successfully.]*/
        done(null, new results.SharedAccessSignatureUpdated(false));
      }
    }
  }.bind(this));
};

Client.prototype._disconnectHandler = function (err) {
  this.emit('disconnect', new results.Disconnected(err));
};

/**
 * @method            module:azure-iot-device.Client#open
 * @description       Call the transport layer CONNECT function if the
 *                    transport layer implements it
 *
 * @param {Function} done       The callback to be invoked when `open`
 *                              completes execution.
 */
Client.prototype.open = function (done) {
  var self = this;
  var connectReceiverIfListening = function () {
    if (self.listeners('message').length > 0) {
      debug('Connecting the receiver since there\'s already someone listening on the \'message\' event');
      self._connectReceiver();
    }
  };

  /* Codes_SRS_NODE_DEVICE_CLIENT_12_001: [The open function shall call the transport’s connect function, if it exists.] */
  if (typeof self._transport.connect === 'function') {
    self._transport.connect(function (err, res) {
      if (err) {
        done(err);
      } else {
        debug('Open transport successful');
        /*Codes_SRS_NODE_DEVICE_CLIENT_16_045: [If the transport successfully establishes a connection the `open` method shall subscribe to the `disconnect` event of the transport.]*/
        self._transport.on('disconnect', self._disconnectHandler.bind(self));
        connectReceiverIfListening();
        done(null, res);
      }
    });
  } else {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_020: [The ‘open’ function should start listening for C2D messages if there are listeners on the ‘message’ event] */
    connectReceiverIfListening();
    done(null, new results.Connected());
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
Client.prototype.sendEvent = function (message, done) {
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
Client.prototype.sendEventBatch = function (messages, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_05_008: [The sendEventBatch method shall send the list of events (indicated by the messages argument) via the transport associated with the Client instance.]*/
  this._transport.sendEventBatch(messages, done);
};

/**
 * @method           module:azure-iot-device.Client#close
 * @description      The `close` method directs the transport to close the current connection to the IoT Hub instance

 * @param {Function} done    The callback to be invoked when the connection has been closed.
 */
Client.prototype.close = function (done) {
  if (this._sharedAccessSignatureRenewalInterval) clearInterval(this._sharedAccessSignatureRenewalInterval);
  /* Codes_SRS_NODE_DEVICE_CLIENT_16_001: [The close function shall call the transport’s disconnect function if it exists.] */
  if (typeof this._transport.disconnect === 'function') {
    this._transport.disconnect(function (err, result) {
      if (err) {
        done(err);
      } else {
        /*Codes_SRS_NODE_DEVICE_CLIENT_16_046: [The `close` method shall remove the listener that has been attached to the transport `disconnect` event.]*/
        this._transport.removeAllListeners('disconnect');
        done(null, result);
      }
    }.bind(this));
  } else {
    this._disconnectReceiver();
    done(null, new results.Disconnected());
  }
};

/**
 * @deprecated      Use Client.setOptions instead.
 * @method          module:azure-iot-device.Client#setTransportOptions
 * @description     The `setTransportOptions` method configures transport-specific options for the client and its underlying transport object.
 *
 * @param {Object}      options     The options that shall be set (see transports documentation).
 * @param {Function}    done        The callback that shall be invoked with either an error or a result object.
 */
Client.prototype.setTransportOptions = function (options, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_024: [The ‘setTransportOptions’ method shall throw a ‘ReferenceError’ if the options object is falsy] */
  if (!options) throw new ReferenceError('options cannot be falsy.');
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_025: [The ‘setTransportOptions’ method shall throw a ‘NotImplementedError’ if the transport doesn’t implement a ‘setOption’ method.] */
  if (typeof this._transport.setOptions !== 'function') throw new errors.NotImplementedError('setOptions does not exist on this transport');

  var clientOptions = {
    http: {
      receivePolicy: options
    }
  };

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_021: [The ‘setTransportOptions’ method shall call the ‘setOptions’ method on the transport object.]*/
  this._transport.setOptions(clientOptions, function(err) {
    if (err) {
      done(err);
    } else {
      done(null, new results.TransportConfigured());
    }
  });
};

/**
 * @method          module:azure-iot-device.Client#setOptions
 * @description     The `setOptions` method let the user configure the client.
 * 
 * @param  {Object}    options  The options structure
 * @param  {Function}  done     The callback that shall be called when setOptions is finished.
 * 
 * @throws {ReferenceError}     If the options structure is falsy
 */

Client.prototype.setOptions = function (options, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_042: [The `setOptions` method shall throw a `ReferenceError` if the options object is falsy.]*/
  if (!options) throw new ReferenceError('options cannot be falsy.');

  this._transport.setOptions(options, function(err) {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_043: [The `done` callback shall be invoked no parameters when it has successfully finished setting the client and/or transport options.]*/
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_044: [The `done` callback shall be invoked with a standard javascript `Error` object and no result object if the client could not be configured as requested.]*/
    if (done) {
      done(err);
    }
  });
};

/**
 * @method           module:azure-iot-device.Client#complete
 * @description      The `complete` method directs the transport to settle the message passed as argument as 'completed'.
 *
 * @param {Message}  message    The message to settle.
 * @param {Function} done       The callback to call when the message is completed.
 *
 * @throws {ReferenceError} If the message is falsy.
 */
Client.prototype.complete = function (message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_016: [The ‘complete’ method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
  if (!message) throw new ReferenceError('message is \'' + message + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_007: [The ‘complete’ method shall call the ‘complete’ method of the transport with the message as an argument]*/
  this._transport.complete(message, function (err, result) {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_008: [The ‘done’ callback shall be called with a null error object and a ‘MessageCompleted’ result once the transport has completed the message.]*/
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_009: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not complete the message.]*/
    if (err) {
      done(err);
    } else {
      done(null, result);
    }
  });
};

/**
 * @method           module:azure-iot-device.Client#reject
 * @description      The `reject` method directs the transport to settle the message passed as argument as 'rejected'.
 *
 * @param {Message}  message    The message to settle.
 * @param {Function} done       The callback to call when the message is rejected.
 *
 * @throws {ReferenceException} If the message is falsy.
 */
Client.prototype.reject = function (message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_018: [The reject method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
  if (!message) throw new ReferenceError('message is \'' + message + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_010: [The reject method shall call the reject method of the transport with the message as an argument]*/
  this._transport.reject(message, function (err, result) {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_011: [The ‘done’ callback shall be called with a null error object and a ‘MessageRejected’ result once the transport has rejected the message.]*/
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_012: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not reject the message.]*/
    if (err) {
      done(err);
    } else {
      done(null, result);
    }
  });
};

/**
 * @method           module:azure-iot-device.Client#abandon
 * @description      The `abandon` method directs the transport to settle the message passed as argument as 'abandoned'.
 *
 * @param {Message}  message    The message to settle.
 * @param {Function} done       The callback to call when the message is abandoned.
 *
 * @throws {ReferenceException} If the message is falsy.
 */
Client.prototype.abandon = function (message, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_017: [The abandon method shall throw a ReferenceError if the ‘message’ parameter is falsy.] */
  if (!message) throw new ReferenceError('message is \'' + message + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_013: [The abandon method shall call the abandon method of the transport with the message as an argument]*/
  this._transport.abandon(message, function (err, result) {
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_014: [The ‘done’ callback shall be called with a null error object and a ‘MessageAbandoned’ result once the transport has abandoned the message.]*/
    /*Codes_SRS_NODE_DEVICE_CLIENT_16_015: [The ‘done’ callback shall be called with a standard javascript Error object and no result object if the transport could not abandon the message.]*/
    if (err) {
      done(err);
    } else {
      done(null, result);
    }
  });
};

/**
 * @method           module:azure-iot-device.Client#uploadToBlob
 * @description      The `uploadToBlob` method uploads a stream to a blob.
 *
 * @param {String}   blobName         The name to use for the blob that will be created with the content of the stream.
 * @param {Stream}   stream           The data to that should be uploaded to the blob.
 * @param {Number}   streamLength     The size of the data to that should be uploaded to the blob.
 * @param {Function} done             The callback to call when the upload is complete.
 *
 * @throws {ReferenceException} If blobName or stream or streamLength is falsy.
 */
Client.prototype.uploadToBlob = function (blobName, stream, streamLength, done) {
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_037: [The `uploadToBlob` method shall throw a `ReferenceError` if `blobName` is falsy.]*/
  if (!blobName) throw new ReferenceError('blobName cannot be \'' + blobName + '\'');
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_038: [The `uploadToBlob` method shall throw a `ReferenceError` if `stream` is falsy.]*/
  if (!stream) throw new ReferenceError('stream cannot be \'' + stream + '\'');
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_039: [The `uploadToBlob` method shall throw a `ReferenceError` if `streamLength` is falsy.]*/
  if (!streamLength) throw new ReferenceError('streamLength cannot be \'' + streamLength + '\'');

  /*Codes_SRS_NODE_DEVICE_CLIENT_16_040: [The `uploadToBlob` method shall call the `done` callback with an `Error` object if the upload fails.]*/
  /*Codes_SRS_NODE_DEVICE_CLIENT_16_041: [The `uploadToBlob` method shall call the `done` callback no parameters if the upload succeeds.]*/
  this.blobUploadClient.uploadToBlob(blobName, stream, streamLength, done);
};

module.exports = Client;
