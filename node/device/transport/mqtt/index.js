// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var Mqtt = require('./lib/mqtt.js');

module.exports = {
  Mqtt: Mqtt,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, Mqtt);
  }
};