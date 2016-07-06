// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

'use strict';

var ConnectionString = require('azure-iot-common').ConnectionString;
var Amqp = require('../lib/amqp.js');
var runTests = require('./_client_common_testrun.js');
var fs = require('fs');

var host = ConnectionString.parse(process.env.IOTHUB_CONNECTION_STRING).HostName;
var deviceId = process.env.IOTHUB_DEVICE_ID;
var key = process.env.IOTHUB_DEVICE_KEY;

function makeConnectionString(host, device, key) {
  return 'HostName=' + host + ';DeviceId=' + device + ';SharedAccessKey=' + key;
}

var connectionString = makeConnectionString(host, deviceId, key);

var badConnStrings = [
  makeConnectionString('bad', deviceId, key),
  makeConnectionString(host, 'bad', key),
  makeConnectionString(host, deviceId, 'bad')
];

describe('Over real AMQPS with a Shared Access Key', function () {
  this.timeout(60000);
  runTests(Amqp, connectionString, badConnStrings);
});
var x509DeviceId = process.env.IOTHUB_X509_DEVICE_ID;
var x509Certificate = fs.readFileSync(process.env.IOTHUB_X509_CERTIFICATE);
var x509Key = fs.readFileSync(process.env.IOTHUB_X509_KEY);
var x509Passphrase = process.env.IOTHUB_X509_PASSPHRASE;
var x509ConnectionString = 'HostName=' + host + ';DeviceId=' + x509DeviceId + ';x509=true';


describe('Over real AMQPS with an x509 certificate', function() {
  this.timeout(60000);
  runTests(Amqp, x509ConnectionString, badConnStrings, x509Certificate, x509Key, x509Passphrase);
});

module.exports = runTests;