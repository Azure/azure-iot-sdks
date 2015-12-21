// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');
var Http = require('./lib/http.js');

module.exports = {
  Http: Http,
  clientFromConnectionString: function (connectionString) {
    return device.Client.fromConnectionString(connectionString, Http);
  }
};