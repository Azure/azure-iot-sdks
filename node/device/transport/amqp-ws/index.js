// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var AmqpWs = require('./lib/amqp_ws.js');

module.exports = {
  AmqpWs: AmqpWs,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, AmqpWs);
  }
};