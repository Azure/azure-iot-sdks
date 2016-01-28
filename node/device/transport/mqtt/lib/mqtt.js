// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('azure-iot-mqtt-base').Mqtt;

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
  this._config = config;
  this._mqtt = new Base(config);
  this._receiver = null;
}

/**
 * @method              module:azure-iot-device-mqtt.Mqtt#connect
 * @description         Establishes the connection to the Azure IoT Hub instance using the MQTT protocol.
 * 
 * @param {Function}    done   callback that shall be called when the connection is established. 
 */
/* Codes_SRS_NODE_DEVICE_HTTP_12_004: [The connect method shall call the connect method on MqttTransport */
Mqtt.prototype.connect = function (done) {
  this._mqtt.connect(function (err, result) {
    if (done) done(err, result);
  });
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

module.exports = Mqtt;