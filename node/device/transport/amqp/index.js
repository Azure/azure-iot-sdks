// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var Amqp = require('./lib/amqp.js');

/**
 * The `azure-iot-device-amqp-ws` module provides support for the AMQP protocol over secure 
 * sockets to the device [client]{@link module:azure-iot-device.Client}.
 *
 * @module azure-iot-device-amqp
 * @requires module:azure-iot-device
 */

module.exports = {
  Amqp: Amqp,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, Amqp);
  }
};