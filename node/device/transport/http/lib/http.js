// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-http-base').Http;
var endpoint = require('azure-iot-common').endpoint;
var HttpReceiver = require('./http_receiver.js');
var PackageJson = require('../package.json');
var results = require('azure-iot-common').results;
var translateError = require('./http_errors.js');

/*Codes_SRS_NODE_DEVICE_HTTP_05_009: [When any Http method receives an HTTP response with a status code >= 300, it shall invoke the done callback function with the following arguments:
err - the standard JavaScript Error object, with the Node.js http.ServerResponse object attached as the property response]*/
/*Codes_SRS_NODE_DEVICE_HTTP_05_010: [When any Http method receives an HTTP response with a status code < 300, it shall invoke the done callback function with the following arguments:
err - null
body - the body of the HTTP response
response - the Node.js http.ServerResponse object returned by the transport]*/
function handleResponse(done) {
  return function onResponse(err, body, response) {
    if (!err) {
      done(null, new results.MessageEnqueued(response));
    } else {
      var error = response ? translateError('Could not send message: ' + err.message, body, response) : err;
      done(error);
    }
  };
}

/**
 * @class module:azure-iot-device-http.Http
 * @classdesc       Provide HTTP transport to the device [client]{@link module:azure-iot-device.Client}.
 *
 * @param   {Object}    config  Configuration object derived from the connection string by the client.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_05_001: [The Http constructor shall accept an object with three properties:
host - (string) the fully-qualified DNS hostname of an IoT hub
deviceId - (string) the name of the IoT hub, which is the first segment of hostname
sharedAccessSignature - (string) a shared access signature generated from the credentials of a policy with the "device connect" permissions.]*/
function Http(config) {
  this._config = config;
  this._http = new Base();
}

/**
 * @method          module:azure-iot-device-http.Http#sendEvent
 * @description     This method sends an event to the IoT Hub as the device indicated in the
 *                  `config` parameter.
 *
 * @param {Message}  message    The [message]{@linkcode module:common/message.Message}
 *                              to be sent.
 * @param {Object}  config      This is a dictionary containing the following keys
 *                              and values:
 *
 * | Key     | Value                                                   |
 * |---------|---------------------------------------------------------|
 * | host    | The host URL of the Azure IoT Hub                       |
 * | hubName | The name of the Azure IoT Hub                           |
 * | keyName | The identifier of the device that is being connected to |
 * | key     | The shared access key auth                              |
 *
 * @param {Function} done       The callback to be invoked when `sendEvent`
 *                              completes execution.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_05_002: [The sendEvent method shall construct an HTTP request using information supplied by the caller, as follows:
POST <config.host>/devices/<config.deviceId>/messages/events?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
iothub-to: /devices/<config.deviceId>/messages/events
Host: <config.host>
User-Agent: <version string>

<message>
]*/
Http.prototype.sendEvent = function (message, done) {
  var config = this._config;
  var path = endpoint.eventPath(config.deviceId);
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature.toString(),
    'iothub-to': path,
    'User-Agent': 'azure-iot-device/' + PackageJson.version
  };
  for (var i = 0; i < message.properties.count(); i++) {
    var propItem = message.properties.getItem(i);
    httpHeaders[propItem.key] = propItem.value;
  }
  var request = this._http.buildRequest('POST', path + endpoint.versionQueryString(), httpHeaders, config.host, handleResponse(done));
  request.write(message.getBytes());
  request.end();
};

function constructBatchBody(messages) {
  var body = '[';

  messages.forEach(function (message, index) {
    var buffMsg = new Buffer(message.getData());

    if (index > 0) body += ',';

    body += '{\"body\":\"' + buffMsg.toString('base64') + '\"';
    // Get the properties
    var propertyIdx = 0;
    var property = ',\"properties\":{';
    for (propertyIdx = 0; propertyIdx < message.properties.count(); propertyIdx++) {
      if (propertyIdx > 0)
        property += ',';
      var propItem = message.properties.getItem(propertyIdx);
      property += '\"' + propItem.key + '\":\"' + propItem.value + '\"';
    }
    if (propertyIdx > 0) {
      property += '}';
      body += property;
    }
    body += "}";
  });
  body += ']';
  return body;
}

/**
 * @method          module:azure-iot-device-http.Http#sendEventBatch
 * @description     The `sendEventBatch` method sends a list of event messages to the IoT Hub
 *                  as the device indicated in the `config` parameter.
 * @param {array<Message>} messages   Array of [Message]{@linkcode module:common/message.Message}
 *                                    objects to be sent as a batch.
 * @param {Object}  config            This is a dictionary containing the
 *                                    following keys and values:
 *
 * | Key     | Value                                                   |
 * |---------|---------------------------------------------------------|
 * | host    | The host URL of the Azure IoT Hub                       |
 * | hubName | The name of the Azure IoT Hub                           |
 * | keyName | The identifier of the device that is being connected to |
 * | key     | The shared access key auth                              |
 *
 * @param {Function}      done      The callback to be invoked when
 *                                  `sendEventBatch` completes execution.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_05_003: [The sendEventBatch method shall construct an HTTP request using information supplied by the caller, as follows:
POST <config.host>/devices/<config.deviceId>/messages/events?api-version=<version> HTTP/1.1
Authorization: <config.sharedAccessSignature>
iothub-to: /devices/<config.deviceId>/messages/events
Content-Type: application/vnd.microsoft.iothub.json
Host: <config.host>
User-Agent: <version string>

{"body":"<Base64 Message1>","properties":{"<key>":"<value>"}},
{"body":"<Base64 Message1>"}...
]*/
Http.prototype.sendEventBatch = function (messages, done) {
  var config = this._config;
  var path = endpoint.eventPath(config.deviceId);
  var httpHeaders = {
    'Authorization': config.sharedAccessSignature.toString(),
    'iothub-to': path,
    'Content-Type': 'application/vnd.microsoft.iothub.json',
    'User-Agent': 'azure-iot-device/' + PackageJson.version
  };

  var request = this._http.buildRequest('POST', path + endpoint.versionQueryString(), httpHeaders, config.host, handleResponse(done));
  var body = constructBatchBody(messages);
  request.write(body);
  request.end();
};

/**
 * @method          module:azure-iot-device-http.Http#getReceiver
 * @description     This methods gets the unique instance of the receiver that is used to asynchronously retrieve messages from the IoT Hub service.
 *
 * @param {Function}      done      The callback to be invoked when `getReceiver` completes execution, passing the receiver object as an argument.
 */
Http.prototype.getReceiver = function getReceiver(done) {
  if (!this._receiver) {
    this._receiver = new HttpReceiver(this._config, this._http);
  }

  done(null, this._receiver);
};

/**
 * @method          module:azure-iot-device-http.Http#setOptions
 * @description     This methods sets the HTTP specific options of the transport.
 *
 * @param {Function}      done      The callback to be invoked when `setOptions` completes.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_16_004: [The ‘setOptions’ method shall call the setOptions method of the HTTP Receiver with the options parameter.]*/
/*Codes_SRS_NODE_DEVICE_HTTP_16_005: [The ‘setOptions’ method shall call the ‘done’ callback when finished.]*/
Http.prototype.setOptions = function (options, done) {
  this._receiver.setOptions(options);
  done(null, new results.TransportConfigured());
};

/**
 * @method              module:azure-iot-device-http.Http#complete
 * @description         Settles the message as complete and calls the done callback with the result.
 *
 * @param {Message}     message     The message to settle as complete.
 * @param {Function}    done        The callback that shall be called with the error or result object.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_16_002: [The ‘complete’ method shall call the ‘complete’ method of the receiver object and pass it the message and the callback given as parameters.] */
Http.prototype.complete = function (message, done) {
  this.getReceiver(function (err, receiver) {
    receiver.complete(message, done);
  });
};

/**
 * @method              module:azure-iot-device-http.Http#reject
 * @description         Settles the message as rejected and calls the done callback with the result.
 *
 * @param {Message}     message     The message to settle as rejected.
 * @param {Function}    done        The callback that shall be called with the error or result object.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_16_003: [The ‘reject’ method shall call the ‘reject’ method of the receiver object and pass it the message and the callback given as parameters.] */
Http.prototype.reject = function (message, done) {
  this.getReceiver(function (err, receiver) {
    receiver.reject(message, done);
  });
};

/**
 * @method              module:azure-iot-device-http.Http#abandon
 * @description         Settles the message as abandoned and calls the done callback with the result.
 *
 * @param {Message}     message     The message to settle as abandoned.
 * @param {Function}    done        The callback that shall be called with the error or result object.
 */
/*Codes_SRS_NODE_DEVICE_HTTP_16_001: [The ‘abandon’ method shall call the ‘abandon’ method of the receiver object and pass it the message and the callback given as parameters.] */
Http.prototype.abandon = function (message, done) {
  this.getReceiver(function (err, receiver) {
    receiver.abandon(message, done);
  });
};

/**
 * @method          module:azure-iot-device-http.Http#updateSharedAccessSignature
 * @description     This methods sets the SAS token used to authenticate with the IoT Hub service.
 *
 * @param {String}        sharedAccessSignature  The new SAS token.
 * @param {Function}      done      The callback to be invoked when `updateSharedAccessSignature` completes.
 */
Http.prototype.updateSharedAccessSignature = function (sharedAccessSignature, done) {
  /*Codes_SRS_NODE_DEVICE_HTTP_16_006: [The updateSharedAccessSignature method shall save the new shared access signature given as a parameter to its configuration.] */
  this._config.sharedAccessSignature = sharedAccessSignature;

  /*Codes_SRS_NODE_DEVICE_HTTP_16_008: [The updateSharedAccessSignature method shall call the `updateSharedAccessSignature` method of the current receiver object if it exists.] */
  if (this._receiver) {
    this._receiver.updateSharedAccessSignature(sharedAccessSignature);
  }

  /*Codes_SRS_NODE_DEVICE_HTTP_16_007: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a SharedAccessSignatureUpdated object as a result, indicating that the client does not need to reestablish the transport connection.] */
  done(null, new results.SharedAccessSignatureUpdated(false));
};

module.exports = Http;