// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iot-common').ConnectionString;
var runTests = require('azure-iot-device/test/_client_common_testrun.js');
var Http = require('../lib/http.js');

var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

var connectionString = 'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=' + key;
var badConnStrings = [
  'HostName=bad;DeviceId=' + deviceId + ';SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=bad;SharedAccessKey=' + key,
  'HostName=' + host + ';DeviceId=' + deviceId + ';SharedAccessKey=bad;'
];

describe('Over real HTTPS', function () {
  this.timeout(60000);
  runTests(Http, connectionString, badConnStrings);
});
