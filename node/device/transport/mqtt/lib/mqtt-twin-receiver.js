// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var EventEmitter = require('events').EventEmitter;
var util = require('util');

/**
 * @class        module:azure-iot-device-mqtt.MqttTwinReceiver
 * @classdesc    Acts as a receiver for device-twin traffic
 * 
 * @param {Object} config   configuration object
 *
 */
function MqttTwinReceiver(config) {
  EventEmitter.call(this);
}

util.inherits(MqttTwinReceiver, EventEmitter);

module.exports = MqttTwinReceiver;

