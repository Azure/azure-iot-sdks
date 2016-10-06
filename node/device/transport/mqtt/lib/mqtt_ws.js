// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var Base = require('./mqtt.js');
var util = require('util');

/**
 * @class        module:azure-iot-device-mqtt.MqttWs
 * @classdesc    Provides MQTT over WebSockets transport for the device [client]{@link module:azure-iot-device.Client} class.
 *
 * @param   {Object}    config  Configuration object derived from the connection string by the client.
 */
/*Codes_SRS_NODE_DEVICE_MQTT_12_001: [The constructor shall accept the transport configuration structure.]*/
/*Codes_SRS_NODE_DEVICE_MQTT_12_002: [The constructor shall store the configuration structure in a member variable.]*/
/*Codes_SRS_NODE_DEVICE_MQTT_12_003: [The constructor shall create an base transport object and store it in a member variable.]*/
function MqttWs(config) {
  Base.call(this, config);
  /*Codes_SRS_NODE_DEVICE_MQTT_16_017: [The `MqttWs` constructor shall initialize the `uri` property of the `config` object to `wss://<host>:443/$iothub/websocket`.]*/
  this._config.uri  = 'wss://' + config.host + ':443/$iothub/websocket';
}

util.inherits(MqttWs, Base);

module.exports = MqttWs;