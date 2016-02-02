// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var AmqpWs = require('./lib/amqp_ws.js');

/**
 * The `azure-iot-device-amqp-ws` module provides support for the AMQP protocol over secure 
 * websockets to the device [client]{@link module:azure-iot-device.Client}.
 *
 * @module azure-iot-device-amqp-ws
 * @requires module:azure-iot-device
 */

module.exports = {
  AmqpWs: AmqpWs,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, AmqpWs);
  }
};