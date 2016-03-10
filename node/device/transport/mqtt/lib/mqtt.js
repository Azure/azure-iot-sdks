// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-mqtt-base').Mqtt;
var results = require('azure-iot-common').results;
var errors = require('azure-iot-common').errors;
var EventEmitter = require('events').EventEmitter;
var util = require('util');

/**
 * @class        module:azure-iot-device-mqtt.Mqtt
 * @classdesc    Provides MQTT transport for the device [client]{@link module:azure-iot-device.Client} class.
 *
 * @param   {Object}    config  Configuration object derived from the connection string by the client.
 */
/*
 Codes_SRS_NODE_DEVICE_HTTP_12_001: [The Mqtt  shall accept the transport configuration structure
 Codes_SRS_NODE_DEVICE_HTTP_12_002: [The Mqtt  shall store the configuration structure in a member variable
 Codes_SRS_NODE_DEVICE_HTTP_12_003: [The Mqtt  shall create an MqttTransport object and store it in a member variable
*/
function Mqtt(config) {
  EventEmitter.call(this);
  this._config = config;
  this._mqtt = new Base(config);
}

util.inherits(Mqtt, EventEmitter);

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#connect
 * @description         Establishes the connection to the Azure IoT Hub instance using the MQTT protocol.
 *
 * @param {Function}    done   callback that shall be called when the connection is established.
 */
/* Codes_SRS_NODE_DEVICE_HTTP_12_004: [The connect method shall call the connect method on MqttTransport */
Mqtt.prototype.connect = function (done) {
  this._mqtt.connect(function (err, result) {
    if (err) {
      if (done) done(err);
    } else {
      this._mqtt.client.on('disconnect', function (err) {
        this.emit('disconnect', new results.Disconnected(err));
      }.bind(this));
      if (done) done(null, result);
    }
  }.bind(this));
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#disconnect
 * @description         Terminates the connection to the IoT Hub instance.
 *
 * @param {Function}    done      Callback that shall be called when the connection is terminated.
 */
/* Codes_SRS_NODE_DEVICE_HTTP_16_001: [The disconnect method should call the disconnect method on MqttTransport.] */
Mqtt.prototype.disconnect = function (done) {
  this._mqtt.disconnect(function (err, result) {
    if (done) done(err, result);
  });
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#sendEvent
 * @description         Sends an event to the server.
 *
 * @param {Message}     message   Message used for the content of the event sent to the server.
 */
/* Codes_SRS_NODE_DEVICE_HTTP_12_005: [The sendEvent method shall call the publish method on MqttTransport */
Mqtt.prototype.sendEvent = function (message, done) {
  this._mqtt.publish(message, done);
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#getReceiver
 * @description         Gets a receiver object that is used to receive and settle messages.
 *
 * @param {Function}    done   callback that shall be called with a receiver object instance.
 */

Mqtt.prototype.getReceiver = function (done) {
  this._mqtt.getReceiver(done);
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#complete
 * @description         Settles the message as complete and calls the done callback with the result.
 *
 * @param {Message}     message     The message to settle as complete.
 * @param {Function}    done        The callback that shall be called with the error or result object.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_005: [The ‘complete’ method shall call the callback given as argument immediately since all messages are automatically completed.]*/
Mqtt.prototype.complete = function (message, done) {
  done(null, new results.MessageCompleted());
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#reject
 * @description         Settles the message as rejected and calls the done callback with the result.
 *
 * @throws {Error}      The MQTT transport does not support rejecting messages.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_006: [The ‘reject’ method shall throw because MQTT doesn’t support rejecting messages.] */
Mqtt.prototype.reject = function () {
  throw new errors.NotImplementedError('the MQTT transport does not support rejecting messages.');
};

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#abandon
 * @description         Settles the message as abandoned and calls the done callback with the result.
 *
 * @throws {Error}      The MQTT transport does not support rejecting messages.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_16_004: [The ‘abandon’ method shall throw because MQTT doesn’t support abandoning messages.] */
Mqtt.prototype.abandon = function () {
  throw new errors.NotImplementedError('The MQTT transport does not support abandoning messages.');
};

/**
 * @method          module:azure-iot-device-mqtt.Mqtt#updateSharedAccessSignature
 * @description     This methods sets the SAS token used to authenticate with the IoT Hub service.
 *
 * @param {String}        sharedAccessSignature  The new SAS token.
 * @param {Function}      done      The callback to be invoked when `updateSharedAccessSignature` completes.
 */
Mqtt.prototype.updateSharedAccessSignature = function (sharedAccessSignature, done) {
  /*Codes_SRS_NODE_DEVICE_MQTT_16_008: [The updateSharedAccessSignature method shall disconnect the current connection operating with the deprecated token, and re-iniialize the transport object with the new connection parameters.]*/
  this._mqtt.disconnect(function (err) {
    if (err) {
      /*Codes_SRS_NODE_DEVICE_MQTT_16_009: [The updateSharedAccessSignature method shall call the `done` method with an Error object if updating the configuration or re-initializing the transport object.]*/
      if (done) done(err);
    } else {
      /*Codes_SRS_NODE_DEVICE_MQTT_16_007: [The updateSharedAccessSignature method shall save the new shared access signature given as a parameter to its configuration.]*/
      this._config.sharedAccessSignature = sharedAccessSignature;
      this._mqtt = new Base(this._config);
      /*Codes_SRS_NODE_DEVICE_MQTT_16_010: [The updateSharedAccessSignature method shall call the `done` callback with a null error object and a SharedAccessSignatureUpdated object as a result, indicating hat the client needs to reestablish the transport connection when ready.]*/
      done(null, new results.SharedAccessSignatureUpdated(true));
    }
  }.bind(this));
};

module.exports = Mqtt;