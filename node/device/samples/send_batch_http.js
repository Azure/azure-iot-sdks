// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('azure-iot-device');

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '[IoT Hub device connection string]';
var client = device.Client.fromConnectionString(connectionString);

// Create two messages and send them to the IoT hub as a batch.
var data = [
  { id: 1, message: 'hello' },
  { id: 2, message: 'world' }
];

var messages = [];
data.forEach(function (value) {
  messages.push(new device.Message(JSON.stringify(value)));
});

console.log('sending ' + messages.length + ' events in a batch');

client.sendEventBatch(messages, printResultFor('send'));

function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
  };
}