// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var runTests = require('./_client_test.js');
var Https = require('azure-iot-common').Https;

function createTransport() {
  return new Https();
}

var host = process.env.IOTHUB_NAME + '.' + process.env.IOTHUB_SUFFIX;
var keyName = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;
var connectionString = 'HostName='+host+';DeviceId='+keyName+';SharedAccessKey='+key;
var badConnStrings = [
  'HostName=bad;DeviceId='+keyName+';SharedAccessKey='+key,
  'HostName='+host+';DeviceId=bad;SharedAccessKey='+key,
  'HostName='+host+';DeviceId='+keyName+';SharedAccessKey=bad;'
];

describe('Over real HTTPS', function () {
  this.timeout(15000);
  runTests(createTransport, connectionString, badConnStrings);
});
