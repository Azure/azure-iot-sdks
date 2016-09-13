// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iot-common').ConnectionString;
var runTests = require('azure-iot-device-amqp/test/_client_common_testrun.js');
var AmqpWs = require('../lib/amqp_ws.js');

var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

function makeConnectionString(host, device, key) {
  return 'HostName=' + host + ';DeviceId=' + device + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString(host, deviceId, key);

var badConnStrings = [
  makeConnectionString('bad', 'device', 'key'),
  makeConnectionString('host', 'bad', 'key'),
  makeConnectionString('host', 'device', 'bad')
];

describe.skip('Over AMQP/WS', function () {
  this.timeout(60000);
  runTests(AmqpWs, connectionString, badConnStrings);
});