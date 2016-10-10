// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var Mqtt = require('./lib/mqtt.js');
var MqttWs = require('./lib/mqtt_ws.js');

/**
 * The `azure-iot-device-mqtt` module provides support the MQTT protocol to the device [client]{@link module:azure-iot-device.Client} using secure sockets.
 *
 * @module azure-iot-device-mqtt
 * @requires module:azure-iot-device
 */

module.exports = {
  Mqtt: Mqtt,
  MqttWs: MqttWs,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, Mqtt);
  }
};