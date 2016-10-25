// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var util = require('util');
var Amqp = require('./amqp.js');
var errors = require('azure-iot-common').errors;
var translateCommonError = require('azure-iot-amqp-base').translateError;

var translateError = function translateError(message, amqpError) {
  var error;

  if (amqpError.constructor.name === 'AMQPError' && amqpError.condition.contents === 'amqp:resource-limit-exceeded') {
    /*Codes_SRS_NODE_DEVICE_AMQP_DEVICE_ERRORS_16_001: [`translateError` shall return an `IotHubQuotaExceededError` if the AMQP error condition is `amqp:resource-limit-exceeded`.]*/
    error = new errors.IotHubQuotaExceededError(message);
  } else {
    error = translateCommonError(message, amqpError);
  }

  error.amqpError = amqpError;

  return error;
};


var handleResult = function (errorMessage, done) {
  return function (err, result) {
    if (err) {
      done(translateError(errorMessage, err));
    } else {
      done(null, result);
    }
  };
};

/**
 * @class module:azure-iot-device-amqp.AmqpWs
 * @classdesc Constructs an {@linkcode AmqpWs} object that can be used on a device to send
 *            and receive messages to and from an IoT Hub instance, using the AMQP protocol over secure websockets.
 *            This class overloads the constructor of the base {@link module:azure-iot-device-amqp:Amqp} class from the AMQP transport, and inherits all methods from it.
 *
 * @augments module:azure-iot-device-amqp.Amqp
 *
 * @param {Object}  config   Configuration object generated from the connection string by the client.
 */
function AmqpWs(config) {
  Amqp.call(this, config);
}

util.inherits(AmqpWs, Amqp);

AmqpWs.prototype.connect = function connect(done) {
  var uri = 'wss://';
  if (!this._config.x509) {
    uri += encodeURIComponent(this._config.deviceId) +
           '@sas.' +
           this._config.hubName +
           ':' +
           encodeURIComponent(this._config.sharedAccessSignature) +
          '@';
  }
  uri += this._config.host + ':443/$iothub/websocket';

  var sslOptions = this._config.x509;

  this._amqp.connect(uri, sslOptions, handleResult('AMQP Transport: Could not connect', done));
};


module.exports = AmqpWs;