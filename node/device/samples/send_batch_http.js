// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var device = require('../device.js'); // require('azure-iot-device');

var connectionString = '[IoT Device Connection String]';

var client = new device.Client(connectionString, new device.Https());

// Create two messages and send them to the IoT Hub as a batch.
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