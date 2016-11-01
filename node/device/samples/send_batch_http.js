// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var clientFromConnectionString = require('azure-iot-device-http').clientFromConnectionString;
var Message = require('azure-iot-device').Message;

// It is NOT a good practice to put device credentials in the code as shown below.
// This is done in this sample for simplicity purposes.
// Good practices such as the use of TPM or other secure storage on the device are recommended to store device credentials.

// String containing Hostname, Device Id & Device Key in the following formats:
//  "HostName=<iothub_host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"
var connectionString = '[IoT Hub device connection string]';
var client = clientFromConnectionString(connectionString);

// Create two messages and send them to the IoT hub as a batch.
var data = [
  { id: 1, message: 'hello' },
  { id: 2, message: 'world' }
];

var messages = [];
data.forEach(function (value) {
  messages.push(new Message(JSON.stringify(value)));
});

console.log('sending ' + messages.length + ' events in a batch');

client.sendEventBatch(messages, printResultFor('send'));

function printResultFor(op) {
  return function printResult(err, res) {
    if (err) console.log(op + ' error: ' + err.toString());
    if (res) console.log(op + ' status: ' + res.statusCode + ' ' + res.statusMessage);
  };
}