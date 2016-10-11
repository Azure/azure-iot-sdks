// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var anHourFromNow = require('azure-iot-common').anHourFromNow;
var EventEmitter = require('events').EventEmitter;
var util = require('util');
var Base = require('azure-iot-amqp-base').Amqp;
var endpoint = require('azure-iot-common').endpoint;
var PackageJson = require('../package.json');
var translateError = require('./amqp_service_errors.js');

/**
 * @class       module:azure-iothub.Amqp
 * @classdesc   Constructs an {@linkcode Amqp} object that can be used in an application
 *              to connect to IoT Hub instance, using the AMQP protocol.
 *
 * @params {Object}  config    The configuration object that should be used to connect to the IoT Hub service.
 * @params {Object}  amqpBase  OPTIONAL: The Base AMQP transport object. Amqp will use azure-iot-common.Amqp if no argument is provided.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_001: [The Amqp constructor shall accept a config object with those 4 properties:
host – (string) the fully-qualified DNS hostname of an IoT Hub
hubName - (string) the name of the IoT Hub instance (without suffix such as .azure-devices.net).
keyName – (string) the name of a key that can be used to communicate with the IoT Hub instance
sharedAccessSignature – (string) the key associated with the key name.] */
function Amqp(config, amqpBase) {
  EventEmitter.call(this);
  this._amqp = amqpBase ? amqpBase : new Base(true, PackageJson.name + '/' + PackageJson.version);
  this._config = config;
  this._renewalInterval = null;
  this._amqp.setDisconnectHandler(function (err) {
    this.emit('disconnect', err);
  }.bind(this));
}

util.inherits(Amqp, EventEmitter);

var handleResult = function (errorMessage, done) {
  return function (err, result) {
    /*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_021: [** All asynchronous instance methods shall not throw if `done` is not specified or falsy]*/
    if (done) {
      if (err) {
        /*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_018: [All asynchronous instance methods shall call the `done` callback with either no arguments or a first null argument and a second argument that is the result of the operation if the operation succeeded.]*/
        done(translateError(errorMessage, err));
      } else {
        /*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_017: [All asynchronous instance methods shall call the `done` callback with a single parameter that is derived from the standard Javascript `Error` object if the operation failed.]*/
        done(null, result);
      }
    }
  };
};

Amqp.prototype._handleSASRenewal = function() {
  this.disconnect(handleResult('SAS Renewal could not disconnect', function (err) {
    if (!err) {
      this.connect(handleResult('SAS Renewal could not connect', function(err) {
        if (err) {
          console.log("Unable to re-establish the connection following SAS renewal");
        }
      }));
    }
  }.bind(this)));
};

/**
 * @method             module:azure-iothub.Amqp#connect
 * @description        Establishes a connection with the IoT Hub instance.
 * @param {Function}   done   Called when the connection is established of if an error happened.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_019: [The `connect` method shall call the `connect` method of the base AMQP transport and translate its result to the caller into a transport-agnostic object.]*/
Amqp.prototype.connect = function connect(done) {
  var uri = 'amqps://' +
         encodeURIComponent(this._config.keyName) +
         '%40sas.root.' +
         this._config.hubName +
         ':' +
         encodeURIComponent((typeof(this._config.sharedAccessSignature) === 'string') ? this._config.sharedAccessSignature : this._config.sharedAccessSignature.extend(anHourFromNow())) + '@' + this._config.host;

  this._amqp.connect(uri, undefined, handleResult('AMQP Transport: Could not connect', function (err, result) {
      //
      // If the base transport completes the connection without error AND we
      // aren't using a SAS token that was passed in by the appliction then set
      // up an interval timer to renew the sas every 45 minutes.
      //
      if (!err) {
        if (typeof(this._config.sharedAccessSignature) !== 'string') {
          this._renewalInterval = setInterval(this._handleSASRenewal.bind(this), 2700000);
        }
      }
      if (done) {
        done(err, result);
      }
  }.bind(this)));
};

/**
 * @method             module:azure-iothub.Amqp#disconnect
 * @description        Disconnects the link to the IoT Hub instance.
 * @param {Function}   done   Called when disconnected of if an error happened.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_020: [** The `disconnect` method shall call the `disconnect` method of the base AMQP transport and translate its result to the caller into a transport-agnostic object.]*/
Amqp.prototype.disconnect = function disconnect(done) {
  if (this._renewalInterval) {
    clearInterval(this._renewalInterval);
  }
  this._amqp.disconnect(handleResult('AMQP Transport: Could not disconnect', done));
};

/**
 * @method             module:azure-iothub.Amqp#send
 * @description        Sends a message to the IoT Hub.
 * @param {Message}  message    The [message]{@linkcode module:common/message.Message}
 *                              to be sent.
 * @param {Function} done       The callback to be invoked when `send`
 *                              completes execution.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_002: [The send method shall construct an AMQP request using the message passed in argument as the body of the message.]*/
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_003: [The message generated by the send method should have its “to” field set to the device ID passed as an argument.]*/
Amqp.prototype.send = function send(deviceId, message, done) {
  var serviceEndpoint = '/messages/devicebound';
  var deviceEndpoint = endpoint.messagePath(encodeURIComponent(deviceId));
  this._amqp.send(message, serviceEndpoint, deviceEndpoint, handleResult('AMQP Transport: Could not send message', done));
};

/**
 * @deprecated
 * @method             module:azure-iothub.Amqp#getReceiver
 * @description        Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 * @param {Function}   done      Callback used to return the {@linkcode AmqpReceiver} object.
 */
Amqp.prototype.getReceiver = function getFeedbackReceiver(done) {
  var feedbackEndpoint = '/messages/serviceBound/feedback';
  this._amqp.getReceiver(feedbackEndpoint, handleResult('AMQP Transport: Could not get receiver', done));
};

/**
 * @method             module:azure-iothub.Amqp#getFeedbackReceiver
 * @description        Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 * @param {Function}   done      Callback used to return the {@linkcode AmqpReceiver} object.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_013: [The `getFeedbackReceiver` method shall request an `AmqpReceiver` object from the base AMQP transport for the `/messages/serviceBound/feedback` endpoint.]*/
Amqp.prototype.getFeedbackReceiver = function getFeedbackReceiver(done) {
  var feedbackEndpoint = '/messages/serviceBound/feedback';
  this._amqp.getReceiver(feedbackEndpoint, handleResult('AMQP Transport: Could not get receiver', done));
};

/**
 * @method             module:azure-iothub.Amqp#getFileNotificationReceiver
 * @description        Gets the {@linkcode AmqpReceiver} object that can be used to receive messages from the IoT Hub instance and accept/reject/release them.
 * @param {Function}   done      Callback used to return the {@linkcode AmqpReceiver} object.
 */
/*Codes_SRS_NODE_IOTHUB_SERVICE_AMQP_16_016: [The `getFeedbackReceiver` method shall request an `AmqpReceiver` object from the base AMQP transport for the `/messages/serviceBound/filenotifications` endpoint.]*/
Amqp.prototype.getFileNotificationReceiver = function getFileNotificationReceiver(done) {
  var fileNotificationEndpoint = '/messages/serviceBound/filenotifications';
  this._amqp.getReceiver(fileNotificationEndpoint, handleResult('AMQP Transport: Could not get file notification receiver', done));
};

module.exports = Amqp;